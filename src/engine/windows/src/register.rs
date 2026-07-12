use std::ffi::OsStr;

use log_derive::logfn;
use windows::{
    Win32::{
        System::Com::{CLSCTX_INPROC_SERVER, CoCreateInstance},
        UI::TextServices::{
            self, CLSID_TF_CategoryMgr, CLSID_TF_InputProcessorProfiles, HKL, ITfCategoryMgr,
            ITfInputProcessorProfileMgr,
        },
    },
    core::GUID,
};
use winreg::{RegKey, enums::HKEY_LOCAL_MACHINE};

use crate::{
    Result,
    extend::{GUIDExt, OsStrExt2},
    global::*,
};

//----------------------------------------------------------------------------
//
//  Registation for standard COM in-proc servers of any kind.
//  An IME is one of these servers.
//
//----------------------------------------------------------------------------

#[logfn(err = "Error")]
pub fn register_server() -> Result<()> {
    // Register the IME's ASCII name under HKLM\SOFTWARE\Classes\CLSID\{IME_ID}
    let hklm = RegKey::predef(HKEY_LOCAL_MACHINE);
    let path = format!("SOFTWARE\\Classes\\CLSID\\{{{}}}", IME_ID.to_rfc4122());
    let (clsid, _) = hklm.create_subkey(path)?;
    clsid.set_value("", &IME_NAME_ASCII)?;
    // Register the dll's path under HKLM\SOFTWARE\Classes\CLSID\{IME_ID}\InprocServer32
    let (inproc_server_32, _) = clsid.create_subkey("InprocServer32")?;
    inproc_server_32.set_value("", &dll_path()?)?;
    // Register the threading model under HKLM\SOFTWARE\Classes\CLSID\{IME_ID}\InprocServer32
    inproc_server_32.set_value("ThreadingModel", &"Apartment")?;
    Ok(())
}

#[logfn(err = "Error")]
pub fn unregister_server() -> Result<()> {
    let hklm = RegKey::predef(HKEY_LOCAL_MACHINE);
    let path = format!("SOFTWARE\\Classes\\CLSID\\{{{}}}", IME_ID.to_rfc4122());
    hklm.delete_subkey_all(path)?;
    Ok(())
}

//----------------------------------------------------------------------------
//
//  Registration for an IME.
//
//----------------------------------------------------------------------------

const SUPPORTED_CATEGORIES: [GUID; 8] = [
    TextServices::GUID_TFCAT_TIP_KEYBOARD,
    TextServices::GUID_TFCAT_DISPLAYATTRIBUTEPROVIDER,
    TextServices::GUID_TFCAT_TIPCAP_UIELEMENTENABLED,
    TextServices::GUID_TFCAT_TIPCAP_SECUREMODE,
    TextServices::GUID_TFCAT_TIPCAP_COMLESS,
    TextServices::GUID_TFCAT_TIPCAP_INPUTMODECOMPARTMENT,
    TextServices::GUID_TFCAT_TIPCAP_IMMERSIVESUPPORT,
    TextServices::GUID_TFCAT_TIPCAP_SYSTRAYSUPPORT,
];

#[logfn(err = "Error")]
pub fn register_ime() -> Result<()> {
    unsafe {
        let profile_mgr: ITfInputProcessorProfileMgr =
            CoCreateInstance(&CLSID_TF_InputProcessorProfiles, None, CLSCTX_INPROC_SERVER)?;
        let category_mgr: ITfCategoryMgr =
            CoCreateInstance(&CLSID_TF_CategoryMgr, None, CLSCTX_INPROC_SERVER)?;
        let ime_name: Vec<u16> = OsStr::new(IME_NAME).to_null_terminated_wchars();
        let icon_file: Vec<u16> = dll_path()?.to_null_terminated_wchars();
        profile_mgr.RegisterProfile(
            &IME_ID,
            TEXTSERVICE_LANGID,
            &LANG_PROFILE_ID,
            &ime_name,
            &icon_file,
            ICON_INDEX,
            HKL::default(),
            0,
            true,
            0,
        )?;
        log::info!("Registered the language profile.");
        for rcatid in SUPPORTED_CATEGORIES {
            category_mgr.RegisterCategory(&IME_ID, &rcatid, &IME_ID)?;
        }
        log::info!("Registered the categories.");
        Ok(())
    }
}

#[logfn(err = "Error")]
pub fn unregister_ime() -> Result<()> {
    unsafe {
        let profile_mgr: ITfInputProcessorProfileMgr =
            CoCreateInstance(&CLSID_TF_InputProcessorProfiles, None, CLSCTX_INPROC_SERVER)?;
        let category_mgr: ITfCategoryMgr =
            CoCreateInstance(&CLSID_TF_CategoryMgr, None, CLSCTX_INPROC_SERVER)?;
        for rcatid in SUPPORTED_CATEGORIES {
            category_mgr.UnregisterCategory(&IME_ID, &rcatid, &IME_ID)?;
        }
        log::info!("Unregistered the categories.");
        profile_mgr.UnregisterProfile(&IME_ID, TEXTSERVICE_LANGID, &LANG_PROFILE_ID, 0)?;
        log::info!("Unregistered the language profile.");
        Ok(())
    }
}
