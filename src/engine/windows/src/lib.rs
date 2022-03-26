#![allow(non_snake_case)]

use std::{ffi::c_void, ptr::null_mut};
use windows::{
    core::{GUID, HRESULT, Interface, IUnknown},
    Win32::{Foundation::*, System::{SystemServices::DLL_PROCESS_ATTACH, Com::IClassFactory}},
};

pub extern "stdcall" fn DLLMain(hmodule: HINSTANCE, reason: u32, lpReserved: *mut c_void) -> BOOL {
    match reason {
        DLL_PROCESS_ATTACH => {}

        _ => (),
    }

    true.into()
}

pub extern "stdcall" fn DllCanUnloadNow() -> HRESULT {
    S_OK
}

pub unsafe extern "stdcall" fn DllGetClassObject(
    _rclsid: *const GUID,
    riid: *const GUID,
    ppvObj: *mut *mut c_void,
) -> HRESULT {
    if *riid == IClassFactory::IID || *riid == IUnknown::IID {
        S_OK
    } else {
        *ppvObj = null_mut();
        CLASS_E_CLASSNOTAVAILABLE
    }
}

pub extern "stdcall" fn DllUnregisterServer() -> HRESULT {
    S_OK
}

pub extern "stdcall" fn DllRegisterServer() -> HRESULT {
    S_OK
}
