use std::sync::atomic::{AtomicBool, Ordering::*};

use windows::{
    Win32::{
        Foundation::{E_INVALIDARG, E_NOTIMPL},
        UI::TextServices::{
            IEnumTfDisplayAttributeInfo, IEnumTfDisplayAttributeInfo_Impl, ITfDisplayAttributeInfo,
            ITfDisplayAttributeInfo_Impl, ITfDisplayAttributeProvider_Impl, TF_ATTR_INPUT,
            TF_DA_COLOR, TF_DISPLAYATTRIBUTE, TF_LS_SOLID,
        },
    },
    core::{BSTR, GUID, Result, implement},
};

use super::TextService;
use crate::{DISPLAY_ATTR_ID, global};

//---------------------------------------------------------------------------------
//
//  To provide display atrributes you need to implement ITfDisplayAttributeProvider
//  and "make it available from the class factory" according to MSDN. They lied.
//  Windows will nerver acquire the instance from class factory. It will only
//  implicitly cast the ITfTextInputProcessor instance to ITfDisplayAttributeProvider.
//
//---------------------------------------------------------------------------------

#[allow(non_snake_case)]
impl ITfDisplayAttributeProvider_Impl for TextService {
    fn EnumDisplayAttributeInfo(&self) -> Result<IEnumTfDisplayAttributeInfo> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        Ok(EnumDisplayAttributeInfo::create())
    }
    fn GetDisplayAttributeInfo(&self, guid: *const GUID) -> Result<ITfDisplayAttributeInfo> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        if unsafe { *guid == global::DISPLAY_ATTR_ID } {
            Ok(DisplayAttributeInfo::create())
        } else {
            Err(E_INVALIDARG.into())
        }
    }
}

//----------------------------------------------------------------------------
//
//  An enumerator that enumerates through all possible display atrributes.
//  The input method has only one display attribute so this is kinda dumb.
//
//----------------------------------------------------------------------------

#[implement(IEnumTfDisplayAttributeInfo)]
struct EnumDisplayAttributeInfo {
    enumerated: AtomicBool,
}
impl EnumDisplayAttributeInfo {
    fn create() -> IEnumTfDisplayAttributeInfo {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        IEnumTfDisplayAttributeInfo::from(Self {
            enumerated: AtomicBool::new(false),
        })
    }
}

#[allow(non_snake_case)]
impl IEnumTfDisplayAttributeInfo_Impl for EnumDisplayAttributeInfo {
    fn Clone(&self) -> Result<IEnumTfDisplayAttributeInfo> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        Err(E_NOTIMPL.into())
    }

    fn Next(
        &self,
        _count: u32,
        info: *mut Option<ITfDisplayAttributeInfo>,
        fetched: *mut u32,
    ) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        // Dear MS please fix these raw pointers thanks
        unsafe {
            if self.enumerated.fetch_and(true, Relaxed) {
                *info = Some(DisplayAttributeInfo::create());
                *fetched = 1;
            } else {
                *fetched = 0;
            }
        }
        Ok(())
    }

    fn Reset(&self) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        self.enumerated.fetch_and(false, Relaxed);
        Ok(())
    }

    fn Skip(&self, count: u32) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        if count > 0 {
            self.enumerated.fetch_and(true, Relaxed);
        }
        Ok(())
    }
}

//----------------------------------------------------------------------------
//
//  Our one and only display attribute that does nothing but adding underlines
//
//----------------------------------------------------------------------------

#[implement(ITfDisplayAttributeInfo)]
#[derive(Default)]
pub struct DisplayAttributeInfo;
impl DisplayAttributeInfo {
    pub fn create() -> ITfDisplayAttributeInfo {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        ITfDisplayAttributeInfo::from(Self {})
    }
}

#[allow(non_snake_case)]
impl ITfDisplayAttributeInfo_Impl for DisplayAttributeInfo {
    fn GetGUID(&self) -> Result<GUID> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        Ok(DISPLAY_ATTR_ID)
    }

    fn GetDescription(&self) -> Result<BSTR> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        Err(E_INVALIDARG.into())
    }

    fn GetAttributeInfo(&self, attr: *mut TF_DISPLAYATTRIBUTE) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        unsafe {
            *attr = TF_DISPLAYATTRIBUTE {
                crText: TF_DA_COLOR::default(),
                crBk: TF_DA_COLOR::default(),
                crLine: TF_DA_COLOR::default(),
                lsStyle: TF_LS_SOLID,
                fBoldLine: false.into(),
                bAttr: TF_ATTR_INPUT,
            };
        }
        Ok(())
    }

    fn SetAttributeInfo(&self, _attr: *const TF_DISPLAYATTRIBUTE) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        Err(E_NOTIMPL.into())
    }

    fn Reset(&self) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        Err(E_NOTIMPL.into())
    }
}
