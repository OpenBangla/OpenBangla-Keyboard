mod composition;
pub mod display_attribute_provider;
mod edit_session;
pub(super) mod key_event_sink;
pub mod keycode;
mod langbar_item;
pub mod text_input_processor;
mod thread_mgr_event_sink;

use std::time::{Duration, Instant};

use log::{debug, error, warn};
use log_derive::logfn;
use parking_lot::{RwLock, RwLockWriteGuard};
use riti::{context::RitiContext, suggestion::Suggestion};
use windows::{
    Win32::{
        Foundation::E_FAIL,
        UI::{
            TextServices::{
                HKL, ITfComposition, ITfCompositionSink, ITfContext, ITfDisplayAttributeProvider,
                ITfKeyEventSink, ITfLangBarItem, ITfTextInputProcessor, ITfTextInputProcessorEx,
                ITfThreadMgr, ITfThreadMgrEventSink,
            },
            WindowsAndMessaging::HICON,
        },
    },
    core::{AsImpl, Interface, Result, VARIANT, implement},
};

use crate::{conf::{Settings, set_riti_config}, global::IME_KEYBOARD_US, ui::candidate_list::CandidateList};

//----------------------------------------------------------------------------
//
//  A text service is required to implement ITfTextInputProcessor and provide
//  a few other interfaces in ITfTextInputProcessor::Activate. The common
//  approach is to let the text service implement every interfaces needed and
//  return self whenever required.
//
//----------------------------------------------------------------------------

/// Methods of TSF interfaces don't allow mutation of any kind. Thus all mutable
/// states are hidden behind a lock. The lock is supposed to be light-weight since
/// inputs from users can be frequent.
#[implement(
    ITfTextInputProcessor,
    ITfTextInputProcessorEx,
    ITfThreadMgrEventSink,
    ITfKeyEventSink,
    ITfCompositionSink,
    ITfLangBarItem,
    ITfDisplayAttributeProvider
)]
pub struct TextService {
    inner: RwLock<TextServiceInner>,
}
struct TextServiceInner {
    riti: RitiContext,
    // Some basic info about the clinet (the program where user is typing)
    tid: u32,
    thread_mgr: Option<ITfThreadMgr>,
    context: Option<ITfContext>,
    // ThreadMrgEventSink
    cookie: Option<u32>,
    // KeyEventSink
    hkl: HKL,
    // Composition
    composition: Option<ITfComposition>,
    suggestions: Option<Suggestion>,
    preedit: String,
    // display attribute provider
    display_attribute: Option<VARIANT>,
    // UI
    candidate_list: Option<CandidateList>,
    icon: HICON,
    // An Arc-like smart pointer pointing to TextService
    interface: Option<ITfTextInputProcessor>,
}

impl TextService {
    #[logfn(err = "Error")]
    pub fn create() -> Result<ITfTextInputProcessor> {
        let Ok(settings) = Settings::load_or_create() else {
            panic!("Failed to load settings from registry.");
        };

        let config = set_riti_config(&settings);

        let inner = TextServiceInner {
            riti: RitiContext::new_with_config(&config),
            tid: 0,
            thread_mgr: None,
            context: None,
            hkl: IME_KEYBOARD_US,
            cookie: None,
            composition: None,
            suggestions: None,
            preedit: String::with_capacity(32),
            icon: HICON::default(),
            candidate_list: None,
            display_attribute: None,
            interface: None,
        };
        let text_service = TextService {
            inner: RwLock::new(inner),
        };
        // from takes ownership of the object and returns a smart pointer
        let interface = ITfTextInputProcessor::from(text_service);
        // inject the smart pointer back to the object
        let text_service: &TextService = unsafe { interface.as_impl() };
        text_service.write()?.interface = Some(interface.clone());
        // cast the interface to desired type
        interface.cast()
    }

    fn write(&self) -> Result<RwLockWriteGuard<'_, TextServiceInner>> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        self.inner
            .try_write()
            .or_else(|| {
                warn!("RwLock::try_write returned None.");
                let timeout = Instant::now() + Duration::from_millis(50);
                self.inner.try_write_until(timeout)
            })
            .ok_or_else(|| {
                error!("Failed to obtain write lock.");
                E_FAIL.into()
            })
    }

    fn try_write(&self) -> Result<RwLockWriteGuard<'_, TextServiceInner>> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        self.inner.try_write().ok_or_else(|| E_FAIL.into())
    }
}

impl TextServiceInner {
    fn interface<I: Interface>(&self) -> Result<I> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        // guarenteed to be Some by TextService::create
        self.interface.as_ref().unwrap().cast()
    }

    fn thread_mgr(&self) -> Result<&ITfThreadMgr> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        self.thread_mgr.as_ref().ok_or_else(|| {
            error!("Thread manager is None.");
            E_FAIL.into()
        })
    }

    fn context(&self) -> Result<&ITfContext> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        self.context.as_ref().ok_or_else(|| {
            error!("Context is None.");
            E_FAIL.into()
        })
    }

    fn candidate_list(&self) -> Result<&CandidateList> {
        self.candidate_list.as_ref().ok_or(E_FAIL.into())
    }

    fn create_candidate_list(&mut self) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        let parent_window = unsafe {
            self.thread_mgr()?
                .GetFocus()?
                .GetTop()?
                .GetActiveView()?
                .GetWnd()?
        };
        self.candidate_list = Some(CandidateList::create(parent_window)?);
        Ok(())
    }

    fn assure_candidate_list(&mut self) -> Result<()> {
        if self.candidate_list.is_some() {
            Ok(())
        } else {
            debug!("Previous creation of candidate list failed. Recreating now.");
            self.create_candidate_list()
        }
    }

    fn update_engine(&mut self) {
        let Ok(settings) = Settings::load_or_create() else {
            panic!("Failed to load settings from registry.");
        };

        let config = set_riti_config(&settings);
        self.riti.update_engine(&config);

        if let Some(candidate_list) = &self.candidate_list {
            candidate_list.set_vertical(!settings.get_candidate_win_horizontal());
        }
    }
}

//----------------------------------------------------------------------------
//
//  Now see tsf/text_input_processor.rs for the implementation.
//
//----------------------------------------------------------------------------
