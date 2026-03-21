use std::ffi::OsString;

use log::{debug, trace};
use windows::{
    Win32::{
        Foundation::E_FAIL,
        UI::TextServices::{ITfComposition, ITfCompositionSink_Impl},
    },
    core::Result,
};

use super::{TextService, TextServiceInner, edit_session};
use crate::extend::OsStrExt2;

//----------------------------------------------------------------------------
//
//  Composition is the texts held by the input method waiting to be "composed"
//  into proper output, or more straightforwardly, those quirky underscored
//  text.
//
//----------------------------------------------------------------------------

impl TextServiceInner {
    // there are only two states: composing or not
    pub fn start_composition(&mut self) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        let composition =
            edit_session::start_composition(self.tid, self.context()?, &self.interface()?)?;
        self.composition = Some(composition);
        if let Some((x, y)) = self.get_pos() {
            self.candidate_list()?.locate(x, y)?;
        }
        Ok(())
    }

    pub fn end_composition(&mut self) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        // clean up the shit as clean as possbile instead of question-markin' all the way thru
        if let (Some(context), Some(composition)) =
            (self.context.as_ref(), self.composition.as_ref())
        {
            let _ = edit_session::end_composition(self.tid, context, composition);
        }
        self.riti.finish_input_session();
        self.composition = None;
        self.preedit.clear();
        self.suggestions = None;
        self.candidate_list()?.hide();
        Ok(())
    }

    fn update_preedit(&mut self) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());
        let range = unsafe { self.composition()?.GetRange()? };
        let text = OsString::from(&self.preedit).to_wchars();
        log::trace!("Preedit wchar text: {:?}", text);
        edit_session::set_text(
            self.tid,
            self.context()?,
            range,
            &text,
            self.display_attribute.as_ref(),
        )
    }

    fn update_candidate_list(&mut self) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        self.assure_candidate_list()?;
        let candidate_list = self.candidate_list()?;
        if self.suggestions.as_ref().unwrap().is_empty() {
            candidate_list.hide();
        } else {
            candidate_list.show(self.suggestions.as_ref().unwrap().get_suggestions())?;
            if let Some((x, y)) = self.get_pos() {
                candidate_list.locate(x, y)?;
            }
        }
        Ok(())
    }

    fn set_text(&self, text: &str) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        let text = OsString::from(text).to_wchars();
        let range = unsafe { self.composition()?.GetRange()? };
        edit_session::set_text(self.tid, self.context()?, range, &text, None)
    }

    fn get_pos(&self) -> Option<(i32, i32)> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        let range = unsafe { self.composition().ok()?.GetRange().ok()? };
        let pos = edit_session::get_pos(self.tid, self.context().ok()?, &range).ok()?;
        if pos.0 <= 0 && pos.1 <= 0 {
            debug!("Abnormal position: ({}, {})", pos.0, pos.1);
            None
        } else {
            Some(pos)
        }
    }

    fn composition(&self) -> Result<&ITfComposition> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        self.composition.as_ref().ok_or(E_FAIL.into())
    }
}

// handle input and transit state
// calling these function while not composing would cause the program to crash
impl TextServiceInner {
    pub fn keypress(&mut self, key: u16, modifier: u8) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());
        let mut selected: u8 = 0;

        if let Ok(candidate_list) = self.candidate_list() {
            selected = candidate_list.get_highlighted_index() as u8;
        }

        let suggestion = self.riti.get_suggestion_for_key(key, modifier, selected);

        if suggestion.is_lonely() {
            self.preedit = suggestion.get_pre_edit_text(0);
            self.update_preedit()?;
            self.suggestions = Some(suggestion);
        } else {
            self.preedit = suggestion.get_auxiliary_text().to_string();
            let prev = suggestion.previously_selected_index();

            self.suggestions = Some(suggestion);
            self.update_preedit()?;

            self.update_candidate_list()?;

            if prev != 0 {
                self.candidate_list()?.set_highlight(prev);
            }
        };

        Ok(())
    }

    pub fn pop(&mut self, ctrl: bool) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());
        log::info!("Backspace ctrl: {ctrl}");
        let suggestion = self.riti.backspace_event(ctrl);

        // todo pop can be used to revert selection
        if suggestion.is_empty() {
            self.preedit.clear();
            return self.abort();
        }

        if suggestion.is_lonely() {
            self.preedit = suggestion.get_pre_edit_text(0);
            self.suggestions = Some(suggestion);
            self.update_preedit()?;
        } else {
            self.preedit = suggestion.get_auxiliary_text().to_string();
            self.suggestions = Some(suggestion);
            self.update_preedit()?;
            self.update_candidate_list()?;
        };

        Ok(())
    }

    /// Commit the 1st suggestion, keeping the unrecognizable trailing characters
    pub fn commit(&mut self, append: Option<char>) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        let mut selected = 0;

        if !self.suggestions.as_ref().unwrap().is_lonely()
            && let Ok(candidate_list) = self.candidate_list()
        {
            selected = candidate_list.get_highlighted_index();
        }

        self.select(selected, append)
    }

    /// Commit the 1st suggestion and release the unrecognizable trailing characters.
    pub fn force_commit(&mut self, ch: char) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        if self.suggestions.as_ref().unwrap().is_empty() {
            self.force_release(ch)
        } else {
            let sugg = self.suggestions.as_ref().unwrap().get_pre_edit_text(0);

            self.set_text(&sugg)?;
            self.end_composition()
        }
    }

    /// Select the desired suggestion by pressing numbers. (from the Candidate list)
    pub fn select(&mut self, index: usize, append: Option<char>) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        if !self.suggestions.as_ref().unwrap().is_lonely()
            && index >= self.suggestions.as_ref().unwrap().len()
        {
            return Ok(());
        }

        let sugg = self.suggestions.as_ref().unwrap().get_pre_edit_text(index);

        self.riti.candidate_committed(index);

        let sugg = if let Some(c) = append {
            &format!("{}{}", sugg, c)
        } else {
            &sugg
        };

        self.set_text(&sugg)?;

        self.end_composition()
    }

    // Release the raw ascii chars
    pub fn release(&mut self) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        self.set_text(&self.preedit)?;
        self.end_composition()
    }

    fn force_release(&mut self, ch: char) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());
        self.preedit.push(ch);
        self.set_text(&self.preedit)?;
        self.end_composition()
    }

    // Interupted. Abort everything.
    pub fn abort(&mut self) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        let _ = self.set_text(&self.preedit);

        self.end_composition()
    }
}

#[allow(non_snake_case)]
impl ITfCompositionSink_Impl for TextService {
    fn OnCompositionTerminated(
        &self,
        _ecwrite: u32,
        _composition: Option<&ITfComposition>,
    ) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        trace!("OnCompositionTerminated");
        // popping out the last letter will trigger this method.
        // `self.write()` causes deadlock(?) in such circumstances
        // because `pop` waits for the completion of this method
        // and this method waits for the releasing of the lock held by `pop`.
        // `self.try_lock()` avoids such issue
        self.try_write()?.abort()
    }
}
