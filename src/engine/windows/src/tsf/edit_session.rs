use std::{cell::Cell, mem::ManuallyDrop};

use log::{error, trace};
use windows::{
    Win32::{
        Foundation::{BOOL, FALSE, RECT, S_OK},
        UI::TextServices::{
            GUID_PROP_ATTRIBUTE, ITfComposition, ITfCompositionSink, ITfContext,
            ITfContextComposition, ITfEditSession, ITfEditSession_Impl, ITfInsertAtSelection,
            ITfRange, TF_AE_NONE, TF_ANCHOR_END, TF_ES_READWRITE, TF_IAS_QUERYONLY, TF_SELECTION,
            TF_SELECTIONSTYLE, TF_ST_CORRECTION,
        },
    },
    core::{AsImpl, Interface, Result, VARIANT, implement},
};

//----------------------------------------------------------------------------
//
//  Edit of any kind must be operated in edit sessions.
//  It's for safety reasons I guess.
//  But it's a pain in the ass to use such sessions so let's hide them under functions.
//
//----------------------------------------------------------------------------

pub fn start_composition(
    tid: u32,
    context: &ITfContext,
    composition_sink: &ITfCompositionSink,
) -> Result<ITfComposition> {
    //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

    trace!("start_composition");
    #[implement(ITfEditSession)]
    struct Session<'a> {
        context: &'a ITfContext,
        composition_sink: &'a ITfCompositionSink,
        composition: Cell<Option<ITfComposition>>, // out
    }

    impl ITfEditSession_Impl for Session<'_> {
        #[allow(non_snake_case)]
        fn DoEditSession(&self, ec: u32) -> Result<()> {
            // to get the current range (namely the selected text or simply the cursor) you insert "nothing"
            // which genius came up with these APIs?
            let range = unsafe {
                self.context
                    .cast::<ITfInsertAtSelection>()?
                    .InsertTextAtSelection(ec, TF_IAS_QUERYONLY, &[])?
            };
            let context_composition = self.context.cast::<ITfContextComposition>()?;
            let composition =
                unsafe { context_composition.StartComposition(ec, &range, self.composition_sink)? };
            self.composition.set(Some(composition));
            Ok(())
        }
    }

    let session = ITfEditSession::from(Session {
        context,
        composition_sink,
        composition: Cell::new(None),
    });

    unsafe {
        let result = context.RequestEditSession(tid, &session, TF_ES_READWRITE)?;
        if result != S_OK {
            Err(result.into())
        } else {
            let session: &Session = session.as_impl();
            Ok(session.composition.take().expect("Composition is None."))
        }
    }
}

pub fn end_composition(tid: u32, context: &ITfContext, composition: &ITfComposition) -> Result<()> {
    //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

    trace!("end_composition");
    #[implement(ITfEditSession)]
    struct Session<'a>(&'a ITfComposition);
    impl ITfEditSession_Impl for Session<'_> {
        #[allow(non_snake_case)]
        fn DoEditSession(&self, ec: u32) -> Result<()> {
            unsafe { self.0.EndComposition(ec) }
        }
    }
    let session = ITfEditSession::from(Session(composition));
    unsafe {
        let result = context.RequestEditSession(tid, &session, TF_ES_READWRITE)?;
        if result != S_OK {
            Err(result.into())
        } else {
            Ok(())
        }
    }
}

pub fn set_text(
    tid: u32,
    context: &ITfContext,
    range: ITfRange,
    text: &[u16],
    dispaly_attribute: Option<&VARIANT>,
) -> Result<()> {
    //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

    #[implement(ITfEditSession)]
    struct Session<'a> {
        context: &'a ITfContext,
        range: ITfRange,
        text: &'a [u16],
        dispaly_attribute: Option<&'a VARIANT>,
    }

    impl ITfEditSession_Impl for Session<'_> {
        #[allow(non_snake_case)]
        fn DoEditSession(&self, ec: u32) -> Result<()> {
            unsafe {
                // apply underscore
                if let Some(display_attribute) = self.dispaly_attribute {
                    self.range.SetText(ec, TF_ST_CORRECTION, self.text)?;
                    let prop = self.context.GetProperty(&GUID_PROP_ATTRIBUTE)?;
                    if let Err(e) = prop.SetValue(ec, &self.range, display_attribute) {
                        error!("Failed to set display attribute. {}", e);
                    }
                } else {
                    // using 0 for dwflag will remove the propety
                    self.range.SetText(ec, 0, self.text)?;
                }
                if self.text.is_empty() {
                    return Ok(());
                }
                // move the cursor to the end
                self.range.Collapse(ec, TF_ANCHOR_END)?;
                let selection = TF_SELECTION {
                    range: ManuallyDrop::new(Some(self.range.clone())),
                    style: TF_SELECTIONSTYLE {
                        ase: TF_AE_NONE,
                        fInterimChar: FALSE,
                    },
                };
                self.context.SetSelection(ec, &[selection])?;
                Ok(())
            }
        }
    }

    let session = ITfEditSession::from(Session {
        context,
        range,
        text,
        dispaly_attribute,
    });
    unsafe {
        let result = context.RequestEditSession(tid, &session, TF_ES_READWRITE)?;
        if result != S_OK {
            Err(result.into())
        } else {
            Ok(())
        }
    }
}

pub fn insert_text(tid: u32, context: &ITfContext, text: &[u16]) -> Result<()> {
    //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

    #[implement(ITfEditSession)]
    struct Session<'a> {
        context: &'a ITfContext,
        text: &'a [u16],
    }

    impl ITfEditSession_Impl for Session<'_> {
        #[allow(non_snake_case)]
        fn DoEditSession(&self, ec: u32) -> Result<()> {
            unsafe {
                let range = self
                    .context
                    .cast::<ITfInsertAtSelection>()?
                    .InsertTextAtSelection(ec, TF_IAS_QUERYONLY, &[])?;
                // insert text via InsertTextAtSelection directly would crash the client
                // what's wrong with these magical APIs
                range.SetText(ec, TF_ST_CORRECTION, self.text)?;
                range.Collapse(ec, TF_ANCHOR_END)?;
                let selection = TF_SELECTION {
                    range: ManuallyDrop::new(Some(range.clone())),
                    style: TF_SELECTIONSTYLE {
                        ase: TF_AE_NONE,
                        fInterimChar: FALSE,
                    },
                };
                self.context.SetSelection(ec, &[selection])
            }
        }
    }

    let session = ITfEditSession::from(Session { context, text });
    unsafe {
        let result = context.RequestEditSession(tid, &session, TF_ES_READWRITE)?;
        if result != S_OK {
            Err(result.into())
        } else {
            Ok(())
        }
    }
}

pub fn get_pos(tid: u32, context: &ITfContext, range: &ITfRange) -> Result<(i32, i32)> {
    //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

    #[implement(ITfEditSession)]
    struct Session<'a> {
        context: &'a ITfContext,
        range: &'a ITfRange,
        pos: Cell<(i32, i32)>,
    }

    impl ITfEditSession_Impl for Session<'_> {
        #[allow(non_snake_case)]
        fn DoEditSession(&self, ec: u32) -> Result<()> {
            unsafe {
                let mut rect = RECT::default();
                let mut clipped = BOOL::default();
                let view = self.context.GetActiveView()?;
                view.GetTextExt(ec, self.range, &mut rect, &mut clipped)?;
                self.pos.set((rect.left, rect.bottom));
                Ok(())
            }
        }
    }

    let session = ITfEditSession::from(Session {
        context,
        range,
        pos: Cell::new((0, 0)),
    });
    unsafe {
        let result = context.RequestEditSession(tid, &session, TF_ES_READWRITE)?;
        if result != S_OK {
            Err(result.into())
        } else {
            let session: &Session = session.as_impl();
            Ok(session.pos.take())
        }
    }
}
