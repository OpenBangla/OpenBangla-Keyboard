#![allow(non_snake_case)]

use std::{ffi::c_void, ptr::null_mut};
use windows::{
    core::{Error, IUnknown, Interface, GUID, HRESULT},
    Win32::{
        Foundation::*,
        System::{Com::IClassFactory, SystemServices::DLL_PROCESS_ATTACH},
    },
};

mod registry;

pub const TEXT_SERVICE: GUID = GUID::from_u128(0x7ad69a9a_2fe2_4381_91e7_b812e3fc6c2e);
pub const LANG_PROFILE: GUID = GUID::from_u128(0xb226c8f4_d348_45ae_9e78_04cfccb13271);

pub static mut INSTANCE: HINSTANCE = HINSTANCE(0);

#[no_mangle]
pub unsafe extern "stdcall" fn DLLMain(
    hmodule: HINSTANCE,
    reason: u32,
    _lpReserved: *mut c_void,
) -> BOOL {
    match reason {
        DLL_PROCESS_ATTACH => {
            INSTANCE = hmodule;
        }

        _ => (),
    }

    true.into()
}

#[no_mangle]
pub extern "stdcall" fn DllCanUnloadNow() -> HRESULT {
    S_OK
}

#[no_mangle]
pub unsafe extern "stdcall" fn DllGetClassObject(
    _rclsid: *const GUID,
    riid: *const GUID,
    ppvObj: *mut *mut c_void,
) -> HRESULT {
    if *riid == IClassFactory::IID || *riid == IUnknown::IID {
        S_OK
    } else {
        ppvObj.write(null_mut());
        CLASS_E_CLASSNOTAVAILABLE
    }
}

#[no_mangle]
pub extern "stdcall" fn DllUnregisterServer() -> HRESULT {
    registry::unregister_profile().ok();
    registry::unregister_categories().ok();
    registry::unregister_server().ok();
    S_OK
}

#[no_mangle]
pub unsafe extern "stdcall" fn DllRegisterServer() -> HRESULT {
    let register = || -> windows::core::Result<()> {
        registry::register_server(INSTANCE)
            .map_err(|_| Error::new(E_FAIL, "Failed to register server".into()))?;
        registry::register_profile(INSTANCE)?;
        registry::register_categories()?;
        Ok(())
    };

    register().into()
}

/*
unsafe fn to_string(s: windows::core::PWSTR) -> String {
    if s.0.is_null() {
        return String::new();
    }

    let mut end = s.0;

    while *end != 0 {
        end = end.add(1);
    }

    let slice = std::slice::from_raw_parts(s.0, end.offset_from(s.0) as _);

    CoTaskMemFree(s.0 as _);
    String::from_utf16_lossy(slice)
}*/
