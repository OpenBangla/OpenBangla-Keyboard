use std::{ffi::OsString, sync::OnceLock};

use log::{debug, error};
use windows::{
    Win32::{
        Foundation::{GetLastError, HINSTANCE},
        System::{
            LibraryLoader::GetModuleFileNameA,
            SystemServices::{LANG_BANGLA, SUBLANG_BANGLA_BANGLADESH},
        },
        UI::TextServices::HKL,
    },
    core::GUID,
};

use crate::Result;

pub fn setup(dll_module: HINSTANCE) {
    DLL_MODULE.get_or_init(|| dll_module);
}

// global variables
static DLL_MODULE: OnceLock<HINSTANCE> = OnceLock::new();
pub fn dll_module() -> HINSTANCE {
    DLL_MODULE.get().copied().unwrap()
}

pub fn dll_path() -> Result<OsString> {
    let mut buf: Vec<u8> = vec![0; 512];
    unsafe { GetModuleFileNameA(dll_module(), &mut buf) };
    if buf[0] == 0 {
        let err = unsafe { GetLastError() };
        error!("Failed to find the dll path. {:?}", err);
        return Err(err.into());
    }
    let mut from = 0;
    let mut to = buf.len();
    while to != from + 1 {
        let i = (to + from) / 2;
        if buf[i] == 0 {
            to = i;
        } else {
            from = i;
        }
    }
    buf.truncate(to);
    let path = unsafe { OsString::from_encoded_bytes_unchecked(buf) };
    debug!("Found DLL in {}", path.to_string_lossy());
    Ok(path)
}

// pub fn hkl_or_us() -> HKL {
//     //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

//     static INSTANCE: OnceLock<HKL> = OnceLock::new();
//     *INSTANCE.get_or_init(|| {
//         // I need try block
//         let result: Result<HKL> = (|| {
//             let hkl = PathBuf::from(env::var("LOCALAPPDATA")?)
//                 .join(IME_NAME)
//                 .join("install.dat");
//             let hkl = fs::read_to_string(hkl)?;
//             let hkl = u32::from_str_radix(&hkl, 16).map_err(Error::InstallDatCorrupted)?;
//             let hkl = HKL(hkl as isize);
//             Ok(hkl)
//         })();
//         result.log_err().unwrap_or(HKL(LanguageID::US as isize))
//     })
// }

// registration stuff
pub const IME_NAME: &str = "OpenBangla";
pub const IME_NAME_ASCII: &str = "OpenBangla";
pub const IME_ID: GUID = GUID::from_u128(0x9AC475F8_4229_47F5_A08A_8A68D3AB1318);
pub const LANG_PROFILE_ID: GUID = GUID::from_u128(0x77598B34_42C3_4EBC_A0F8_7A7769CA44CD);
pub const LANGBAR_ITEM_ID: GUID = GUID::from_u128(0x997E9F8B_BB33_43DA_9FB2_5271BAD7C556);
pub const DISPLAY_ATTR_ID: GUID = GUID::from_u128(0xB0ADCBF2_E221_4CF0_AFED_7C3F7C7AD328);
pub const TEXTSERVICE_LANGID: u16 = (SUBLANG_BANGLA_BANGLADESH << 10 | LANG_BANGLA) as u16;
pub const IME_KEYBOARD_US: HKL = HKL(0x00000409);
pub const ICON_INDEX: u32 = 0;
// customization
pub const CANDI_NUM: usize = 9;
pub const CANDI_INDEXES: [&str; CANDI_NUM] = ["1", "2", "3", "4", "5", "6", "7", "8", "9"];
pub const CANDI_INDEX_SUFFIX: &str = ".";
