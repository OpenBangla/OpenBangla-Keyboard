use std::{
    char::DecodeUtf16Error,
    ffi::OsStr,
    iter::{self},
    os::windows::ffi::OsStrExt,
};

use windows::{
    Win32::
        UI::Input::KeyboardAndMouse::{GetKeyState, VIRTUAL_KEY}
    ,
    core::GUID,
};

pub trait ResultExt {
    fn log_err(self) -> Self;
}

impl<T, E: std::error::Error> ResultExt for std::result::Result<T, E> {
    fn log_err(self) -> Self {
        if let Err(e) = self.as_ref() {
            log::error!("{e:#}")
        }
        self
    }
}

// pub trait HKLExt {
//     fn langid(self) -> u16;
// }

// impl HKLExt for HKL {
//     fn langid(self) -> u16 {
//         (self.0 & 0xFFFF) as u16
//     }
// }

pub trait GUIDExt {
    fn to_rfc4122(&self) -> String;
}

impl GUIDExt for GUID {
    fn to_rfc4122(&self) -> String {
        let mut buf = String::new();
        let hex = format!("{:032X}", self.to_u128());
        buf.push_str(&hex[0..8]);
        buf.push('-');
        buf.push_str(&hex[8..12]);
        buf.push('-');
        buf.push_str(&hex[12..16]);
        buf.push('-');
        buf.push_str(&hex[16..20]);
        buf.push('-');
        buf.push_str(&hex[20..32]);
        buf
    }
}

pub trait OsStrExt2 {
    fn to_wchars(&self) -> Vec<u16>;
    fn to_null_terminated_wchars(&self) -> Vec<u16>;
}

impl OsStrExt2 for OsStr {
    fn to_wchars(&self) -> Vec<u16> {
        self.encode_wide().collect()
    }
    fn to_null_terminated_wchars(&self) -> Vec<u16> {
        self.encode_wide().chain(iter::once(0)).collect()
    }
}

pub trait CharExt {
    fn try_from_utf16(value: u16) -> Result<char, DecodeUtf16Error>;
}

impl CharExt for char {
    fn try_from_utf16(value: u16) -> Result<char, DecodeUtf16Error> {
        char::decode_utf16(iter::once(value)).next().unwrap()
    }
}

pub trait VKExt {
    fn is_down(&self) -> bool;
    fn is_toggled(&self) -> bool;
}

impl VKExt for VIRTUAL_KEY {
    fn is_down(&self) -> bool {
        unsafe { GetKeyState(self.0 as i32) as u16 & 0x8000 != 0 }
    }

    fn is_toggled(&self) -> bool {
        unsafe { GetKeyState(self.0 as i32) as u16 & 1 != 0 }
    }
}
