#![allow(non_snake_case)]
#![feature(let_else)]

use std::{ffi::c_void, ptr::{null_mut, null}, mem::zeroed};
use windows::{
    core::{GUID, HRESULT, Interface, IUnknown},
    Win32::{Foundation::*, System::{SystemServices::DLL_PROCESS_ATTACH, Com::{IClassFactory, StringFromCLSID, CoTaskMemFree}, LibraryLoader::GetModuleFileNameW, Registry::{RegCreateKeyExW, HKEY_CLASSES_ROOT, REG_OPTION_NON_VOLATILE, KEY_WRITE}}},
};

pub const TEXT_SERVICE: GUID = GUID::from_u128(0x7ad69a9a_2fe2_4381_91e7_b812e3fc6c2e);
pub const LANG_PROFILE: GUID = GUID::from_u128(0xb226c8f4_d348_45ae_9e78_04cfccb13271);

pub static mut INSTANCE: HINSTANCE = HINSTANCE(0);

pub unsafe extern "stdcall" fn DLLMain(hmodule: HINSTANCE, reason: u32, _lpReserved: *mut c_void) -> BOOL {
    match reason {
        DLL_PROCESS_ATTACH => {
            INSTANCE = hmodule;
        }

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

pub unsafe extern "stdcall" fn DllRegisterServer() -> HRESULT {
    let mut filename: [u16; 260] = zeroed();

    GetModuleFileNameW(INSTANCE, &mut filename);

    let Ok(clsid) = StringFromCLSID(&TEXT_SERVICE) else {
        return E_FAIL;
    };

    let mut reg_path = String::from("CLSID\\");
    reg_path += &to_string(clsid);

    let mut hkey = zeroed(); 

    RegCreateKeyExW(HKEY_CLASSES_ROOT, reg_path, 0, None, REG_OPTION_NON_VOLATILE, KEY_WRITE, null(), &mut hkey, null_mut());

    S_OK
}

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
}
