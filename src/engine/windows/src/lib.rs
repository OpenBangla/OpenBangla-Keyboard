#![allow(non_camel_case_types)]
mod conf;
mod extend;
mod global;
mod logger;
mod register;
mod tsf;
mod ui;

use std::{ffi::c_void, num::ParseIntError};

use global::*;
use register::*;
use ui::candidate_list;
use windows::{
    Win32::{
        Foundation::{BOOL, E_FAIL, HINSTANCE, S_FALSE, WIN32_ERROR},
        System::{
            Com::{IClassFactory, IClassFactory_Impl},
            SystemServices::DLL_PROCESS_ATTACH,
        },
    },
    core::{GUID, HRESULT, IUnknown, Interface, implement},
};

use crate::tsf::TextService;

#[macro_export]
macro_rules! function {
    () => {{
        fn f() {}
        fn type_name_of<T>(_: T) -> &'static str {
            std::any::type_name::<T>()
        }
        let name = type_name_of(f);
        name.strip_suffix("::f").unwrap()
    }};
}

//----------------------------------------------------------------------------
//
//  Entry for the DLL
//
//----------------------------------------------------------------------------

#[unsafe(no_mangle)]
extern "system" fn DllMain(dll_module: HINSTANCE, call_reason: u32, _reserved: *mut ()) -> bool {
    if call_reason != DLL_PROCESS_ATTACH {
        return true;
    }
    logger::setup();
    global::setup(dll_module);
    candidate_list::setup().is_ok()
}

//----------------------------------------------------------------------------
//
//  The four exposed functions.
//
//----------------------------------------------------------------------------

// Register the IME into the OS. See register.rs.
#[unsafe(no_mangle)]
extern "system" fn DllRegisterServer() -> HRESULT {
    fn reg() -> windows::core::Result<()> {
        register_server()?;
        register_ime()?;
        Ok(())
    }
    reg().into()
}

// Unregister the IME from the OS. See register.rs.
#[unsafe(no_mangle)]
extern "system" fn DllUnregisterServer() -> HRESULT {
    fn unreg() -> windows::core::Result<()> {
        unregister_ime()?;
        unregister_server()?;
        Ok(())
    }
    unreg().into()
}

// Returns the required object. For a COM dll like an IME, the required object is always a class factory.
#[unsafe(no_mangle)]
extern "system" fn DllGetClassObject(
    _rclsid: *const GUID,
    riid: *const GUID,
    ppv: *mut *mut c_void,
) -> HRESULT {
    // SomeInterface::from will move the object, thus we don't need to worry about the object's lifetime
    // the return value is a C++ vptr pointing to the moved object under the hood
    // *ppv = mem::transmute(&ClassFactory::new()) is incorrect and causes the Grey Screen of Death.
    unsafe { IUnknown::from(ClassFactory::new()).query(riid, ppv) }
}

#[unsafe(no_mangle)]
extern "system" fn DllCanUnloadNow() -> HRESULT {
    // todo: add ref count.
    // it seems not that of a important thing to do according to
    // https://github.com/microsoft/windows-rs/issues/2472 tho
    S_FALSE
}

//----------------------------------------------------------------------------
//
//  ClassFactory. It creates nothing but IME instances.
//
//----------------------------------------------------------------------------

#[implement(IClassFactory)]
struct ClassFactory;

impl ClassFactory {
    fn new() -> ClassFactory {
        ClassFactory {}
    }
}

impl IClassFactory_Impl for ClassFactory {
    fn CreateInstance(
        &self,
        _punkouter: Option<&IUnknown>,
        riid: *const GUID,
        ppvobject: *mut *mut c_void,
    ) -> windows::core::Result<()> {
        unsafe { TextService::create()?.query(riid, ppvobject).ok() }
    }

    fn LockServer(&self, _flock: BOOL) -> windows::core::Result<()> {
        Ok(())
    }
}

//----------------------------------------------------------------------------
//
//  Error
//
//----------------------------------------------------------------------------

pub type Result<T> = std::result::Result<T, Error>;

#[derive(thiserror::Error, Debug)]
pub enum Error {
    #[error(transparent)]
    Win(#[from] windows::core::Error),
    #[error(transparent)]
    Io(#[from] std::io::Error),
    #[error(transparent)]
    Var(#[from] std::env::VarError),
    // custom ones
    #[error("Language ID is missing from 'install.toml'.")]
    LangidMissing,
    #[error("Keyboad layout is is missing from 'install.toml'.")]
    LayoutMissing,
    #[error("Requested keyboard layout is invalid.")]
    LayoutInvalid,
    #[error("install.dat is corrupted. {0}")]
    InstallDatCorrupted(ParseIntError),
}

// bonus From<E> for alternative windows Error types
impl From<WIN32_ERROR> for Error {
    fn from(value: WIN32_ERROR) -> Self {
        Self::Win(value.into())
    }
}
impl From<HRESULT> for Error {
    fn from(value: HRESULT) -> Self {
        Self::Win(value.into())
    }
}

// cast to windows Error when required, keeping the original error message
impl From<Error> for windows::core::Error {
    fn from(value: Error) -> Self {
        match value {
            Error::Win(e) => e,
            other => windows::core::Error::new(E_FAIL, other.to_string()),
        }
    }
}
impl From<Error> for HRESULT {
    fn from(value: Error) -> Self {
        windows::core::Error::from(value).into()
    }
}

//----------------------------------------------------------------------------
//
//  See tsf/mod.rs for the IME's implementation
//
//----------------------------------------------------------------------------
