use windows::{
    core::{Interface, GUID},
    Win32::{
        Foundation::HINSTANCE,
        System::{
            Com::{CoCreateInstance, CLSCTX_INPROC_SERVER},
            LibraryLoader::GetModuleFileNameW,
            SystemServices::{LANG_BANGLA, SUBLANG_NEUTRAL},
        },
        UI::TextServices::{
            CLSID_TF_CategoryMgr, CLSID_TF_InputProcessorProfiles, ITfCategoryMgr,
            ITfInputProcessorProfileMgr, GUID_TFCAT_DISPLAYATTRIBUTEPROVIDER,
            GUID_TFCAT_TIPCAP_COMLESS, GUID_TFCAT_TIPCAP_IMMERSIVESUPPORT,
            GUID_TFCAT_TIPCAP_INPUTMODECOMPARTMENT, GUID_TFCAT_TIPCAP_SECUREMODE,
            GUID_TFCAT_TIPCAP_SYSTRAYSUPPORT, GUID_TFCAT_TIPCAP_UIELEMENTENABLED,
            GUID_TFCAT_TIP_KEYBOARD, HKL,
        },
    },
};
use winreg::{enums::HKEY_CLASSES_ROOT, RegKey};

use crate::{LANG_PROFILE, TEXT_SERVICE};

const TEXT_SERVICE_DESC: &str = "OpenBangla Keyboard";
const OPENBANGLA_ICON_INDEX: u32 = 6;
const TEXTSERVICE_LANGID: u16 = (SUBLANG_NEUTRAL << 10 | LANG_BANGLA) as u16;

pub fn create_instance<T: Interface>(clsid: &GUID) -> windows::core::Result<T> {
    unsafe { CoCreateInstance(clsid, None, CLSCTX_INPROC_SERVER) }
}

pub fn register_profile(handle: HINSTANCE) -> windows::core::Result<()> {
    let manager: ITfInputProcessorProfileMgr = create_instance(&CLSID_TF_InputProcessorProfiles)?;

    let mut icon_path = [0u16; 260];
    let path_len = unsafe { GetModuleFileNameW(handle, &mut icon_path) };
    let description: Vec<u16> = TEXT_SERVICE_DESC.encode_utf16().collect();

    unsafe {
        manager.RegisterProfile(
            &TEXT_SERVICE,
            TEXTSERVICE_LANGID,
            &LANG_PROFILE,
            &description,
            &icon_path[..path_len as _],
            OPENBANGLA_ICON_INDEX,
            HKL::default(),
            0,
            true,
            0,
        )?;
    }

    Ok(())
}

pub fn unregister_profile() -> Result<(), windows::core::Error> {
    let profile_manager: ITfInputProcessorProfileMgr =
        create_instance(&CLSID_TF_InputProcessorProfiles)?;

    unsafe {
        profile_manager.UnregisterProfile(&TEXT_SERVICE, TEXTSERVICE_LANGID, &LANG_PROFILE, 0)?;
    }

    Ok(())
}

static SUPPORT_CATEGORIES: [GUID; 8] = [
    GUID_TFCAT_TIP_KEYBOARD,
    GUID_TFCAT_DISPLAYATTRIBUTEPROVIDER,
    GUID_TFCAT_TIPCAP_UIELEMENTENABLED,
    GUID_TFCAT_TIPCAP_SECUREMODE,
    GUID_TFCAT_TIPCAP_COMLESS,
    GUID_TFCAT_TIPCAP_INPUTMODECOMPARTMENT,
    GUID_TFCAT_TIPCAP_IMMERSIVESUPPORT,
    GUID_TFCAT_TIPCAP_SYSTRAYSUPPORT,
];

pub fn register_categories() -> windows::core::Result<()> {
    let category_manager: ITfCategoryMgr = create_instance(&CLSID_TF_CategoryMgr)?;

    for guid in SUPPORT_CATEGORIES {
        unsafe {
            category_manager.RegisterCategory(&TEXT_SERVICE, &guid, &TEXT_SERVICE)?;
        }
    }

    Ok(())
}

pub fn unregister_categories() -> windows::core::Result<()> {
    let category_manager: ITfCategoryMgr = create_instance(&CLSID_TF_CategoryMgr)?;

    for guid in SUPPORT_CATEGORIES {
        unsafe {
            category_manager.UnregisterCategory(&TEXT_SERVICE, &guid, &TEXT_SERVICE)?;
        }
    }

    Ok(())
}

pub fn register_server(handle: HINSTANCE) -> std::io::Result<()> {
    let mut filename = [0u16; 260];

    unsafe {
        GetModuleFileNameW(handle, &mut filename);
    }

    let filename =
        String::from_utf16_lossy(&filename[..filename.iter().position(|&x| x == 0).unwrap()]);

    let reg_path = format!("CLSID\\{{{TEXT_SERVICE:?}}}");

    let (key, _) = RegKey::predef(HKEY_CLASSES_ROOT).create_subkey(reg_path)?;
    key.set_value("", &TEXT_SERVICE_DESC)?;

    let (inproc_key, _) = key.create_subkey("InProcServer32")?;
    inproc_key.set_value("", &filename)?;
    inproc_key.set_value("ThreadingModel", &"Apartment")?;

    Ok(())
}

pub fn unregister_server() -> std::io::Result<()> {
    let reg_path = format!("CLSID\\{{{TEXT_SERVICE:?}}}");
    RegKey::predef(HKEY_CLASSES_ROOT).delete_subkey_all(reg_path)
}
