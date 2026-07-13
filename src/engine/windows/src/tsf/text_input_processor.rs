use log::{debug, trace, warn};
use windows::{
    Win32::{
        Foundation::E_FAIL,
        System::Com::{CLSCTX_INPROC_SERVER, CoCreateInstance},
        UI::TextServices::{
            CLSID_TF_CategoryMgr, ITfCategoryMgr, ITfKeyEventSink, ITfKeystrokeMgr, ITfSource,
            ITfTextInputProcessor_Impl, ITfTextInputProcessorEx_Impl, ITfThreadMgr,
            ITfThreadMgrEventSink,
        },
    },
    core::{Interface, Result, VARIANT},
};

use super::TextService;
use super::key_event_sink::{register_preserved_keys, unregister_preserved_keys};
use crate::DISPLAY_ATTR_ID;

#[allow(non_snake_case)]
impl ITfTextInputProcessor_Impl for TextService {
    fn Activate(&self, thread_mgr: Option<&ITfThreadMgr>, tid: u32) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        trace!("Activate({tid})");
        let mut inner = self.write()?;
        let thread_mgr = thread_mgr.ok_or(E_FAIL)?;
        inner.tid = tid;
        inner.thread_mgr = Some(thread_mgr.clone());
        unsafe {
            // Use self as event sink to subscribe to events
            let keystroke_mgr = thread_mgr.cast::<ITfKeystrokeMgr>()?;
            keystroke_mgr.AdviseKeyEventSink(
                tid,
                &inner.interface::<ITfKeyEventSink>()?,
                true,
            )?;
            debug!("Added key event sink.");
            register_preserved_keys(&keystroke_mgr, tid);
            debug!("Registered preserved keys.");
            inner.cookie = Some(thread_mgr.cast::<ITfSource>()?.AdviseSink(
                &ITfThreadMgrEventSink::IID,
                &inner.interface::<ITfThreadMgrEventSink>()?,
            )?);
            debug!("Added thread manager event sink.");
            let _ = inner.create_candidate_list();
            // thread_mgr.cast::<ITfLangBarItemMgr>()?.AddItem(
            //     &inner.interface::<ITfLangBarItem>()?)?;
            // debug!("Added langbar item.");
            if inner.display_attribute.is_none() {
                let category_mgr: ITfCategoryMgr =
                    CoCreateInstance(&CLSID_TF_CategoryMgr, None, CLSCTX_INPROC_SERVER)?;
                let guid_atom = category_mgr.RegisterGUID(&DISPLAY_ATTR_ID)?;
                inner.display_attribute = Some(VARIANT::from(guid_atom as i32));
            }
            Ok(())
        }
    }

    fn Deactivate(&self) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        trace!("Deactivate");
        let mut inner = self.write()?;
        let thread_mgr = inner.thread_mgr()?;
        unsafe {
            let keystroke_mgr = thread_mgr.cast::<ITfKeystrokeMgr>()?;
            unregister_preserved_keys(&keystroke_mgr);
            keystroke_mgr.UnadviseKeyEventSink(inner.tid)?;
            debug!("Removed key event sink.");
            if let Some(cookie) = inner.cookie {
                thread_mgr.cast::<ITfSource>()?.UnadviseSink(cookie)?;
                inner.cookie = None;
                debug!("Removed thread manager event sink.");
            } else {
                warn!("Cookie for thread manager event sink is None.");
            }
            if let Some(candidate_list) = inner.candidate_list.as_ref() {
                candidate_list.destroy()?;
            }
            // thread_mgr.cast::<ITfLangBarItemMgr>()?.RemoveItem(&inner.interface::<ITfLangBarItem>()?)?;
            // debug!("Removed langbar item.")
        }
        inner.thread_mgr = None;
        inner.candidate_list = None;
        Ok(())
    }
}

#[allow(non_snake_case)]
impl ITfTextInputProcessorEx_Impl for TextService {
    fn ActivateEx(&self, thread_mgr: Option<&ITfThreadMgr>, tid: u32, _dwflags: u32) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        self.Activate(thread_mgr, tid)
    }
}

//----------------------------------------------------------------------------
//
//  Now see tsf/key_event_sink.rs
//
//----------------------------------------------------------------------------
