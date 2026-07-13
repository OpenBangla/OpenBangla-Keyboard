use Input::*;
use log::{trace, warn};
use riti::context::{MODIFIER_ALT_GR, MODIFIER_SHIFT};
use windows::{
    Win32::{
        Foundation::{BOOL, E_FAIL, FALSE, LPARAM, TRUE, WPARAM},
        UI::{
            Input::KeyboardAndMouse::{
                GetKeyboardState, MAPVK_VK_TO_VSC, MapVirtualKeyExW, ToUnicodeEx, VK_CONTROL,
                VK_LCONTROL, VK_LMENU, VK_LSHIFT, VK_MENU, VK_RCONTROL, VK_RMENU, VK_RSHIFT,
                VK_SHIFT,
            },
            TextServices::{
                ITfContext, ITfKeyEventSink_Impl, ITfKeystrokeMgr, TF_MOD_ALT, TF_MOD_CONTROL,
                TF_MOD_SHIFT, TF_PRESERVEDKEY,
            },
        },
    },
    core::{GUID, Result},
};

use super::{TextService, TextServiceInner};
use crate::{
    extend::{CharExt, GUIDExt, VKExt},
    tsf::keycode::{UNKNOWN_KEYCODE, to_keycode},
};
//----------------------------------------------------------------------------
//
//  A "sink" for key events. From here on the processing begins.
//  First thing first is to simplify the overly complicated key events to "inputs"
//
//----------------------------------------------------------------------------

#[allow(non_snake_case)]
impl ITfKeyEventSink_Impl for TextService {
    /// The return value suggests if the key event **will be** eaten or not **if** `OnKeyDown` is called.
    ///
    /// If `true`, the client **may** ignore the actual return value of `OnTestKeyDown` afterwards.
    /// Thus you cannot always return `true` to "capture" every event and expect to "release" them later
    /// in `OnKeyDown` by returning `false`.
    ///
    /// If `false`, the client **may** not call `OnKeyDown` afterwards.
    /// Thus try to gather any needed infomations and states in `OnTestKeyDown` if possible since it
    /// may be your only chance.
    ///
    /// `wparam` indicates the key that is pressed.
    /// The 0-15 bits of `_lparam` indicates the repeat count (ignored here because it's actually always 1).
    /// (See https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-keydown for detail).
    fn OnTestKeyDown(
        &self,
        _context: Option<&ITfContext>,
        wparam: WPARAM,
        lparam: LPARAM,
    ) -> Result<BOOL> {
        trace!("OnTestKeyDown({:#04X})", wparam.0);

        let inner = self.write()?;

        let input = inner.parse_input(wparam.0 as u32, lparam.0 as u32)?;

        inner.test_input(input)
    }

    /// The return value suggests if the key event **is** eaten or not.
    /// The client might call `OnKeyDown` directly without calling `OnTestKeyDown` beforehand.
    /// The client might call `OnKeyDown` even if `OnTestKeyDown` returned `false`.
    /// The client can be an asshole. Remember that.
    fn OnKeyDown(
        &self,
        context: Option<&ITfContext>,
        wparam: WPARAM,
        lparam: LPARAM,
    ) -> Result<BOOL> {
        trace!("OnKeyDown({:#04X})", wparam.0);

        let mut inner = self.write()?;

        let input = inner.parse_input(wparam.0 as u32, lparam.0 as u32)?;

        inner.handle_input(input, context)
    }

    /// Flip the modifiers back
    fn OnTestKeyUp(
        &self,
        _context: Option<&ITfContext>,
        wparam: WPARAM,
        _lparam: LPARAM,
    ) -> Result<BOOL> {
        trace!("OnTestKeyUp({:#04X})", wparam.0);

        Ok(FALSE)
    }

    fn OnKeyUp(
        &self,
        _context: Option<&ITfContext>,
        wparam: WPARAM,
        _lparam: LPARAM,
    ) -> Result<BOOL> {
        trace!("OnKeyUp({:#04X})", wparam.0);

        Ok(FALSE)
    }

    fn OnPreservedKey(&self, context: Option<&ITfContext>, rguid: *const GUID) -> Result<BOOL> {
        let guid = unsafe { rguid.as_ref() }.ok_or(E_FAIL)?;
        let Some((vkey, shift)) = decode_preserved_key_guid(guid) else {
            trace!("OnPreservedKey: unknown GUID {:?}", GUID::to_rfc4122(guid));
            return Ok(FALSE);
        };

        trace!("OnPreservedKey: vkey={:#04X}, shift={}", vkey, shift);

        let mut inner = self.write()?;
        let scancode = unsafe { MapVirtualKeyExW(vkey, MAPVK_VK_TO_VSC, inner.hkl) };
        let char_key = inner.parse_character_key(vkey, scancode)?;

        let input = match char_key {
            Key(key) => {
                if shift {
                    ShiftAltGr(key)
                } else {
                    AltGrKey(key)
                }
            }
            Number(n) => {
                if shift {
                    ShiftAltGr(n as u16)
                } else {
                    AltGrKey(n as u16)
                }
            }
            _ => return Ok(FALSE),
        };

        inner.handle_input(input, context)
    }

    fn OnSetFocus(&self, foreground: BOOL) -> Result<()> {
        trace!("OnSetFocus({})", foreground.as_bool());
        if !foreground.as_bool() {
            self.write()?.abort()
        } else {
            Ok(())
        }
    }
}

impl TextServiceInner {
    fn parse_input(&self, keycode: u32, scancode: u32) -> Result<Input> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());
        let ctrl = VK_CONTROL.is_down() || VK_LCONTROL.is_down() || VK_RCONTROL.is_down();

        let input = match keycode {
            0x08 => Backspace(ctrl),
            0x09 => Tab,
            0x0D => Enter,
            0x20 => Space,
            0x25 => Left,
            0x26 => Up,
            0x27 => Right,
            0x28 => Down,
            keycode @ 0x00..0x20 | keycode @ 0x7F => Unknown(keycode),
            keycode => {
                let key = self.parse_character_key(keycode, scancode)?;

                // Encode the key as unknown if Ctrl is pressed
                if let Key(key) = key && ctrl {
                    Unknown(key as u32)
                } else if let Number(key) = key && ctrl {
                    Unknown(key as u32)
                } else {
                    key
                }
            }
        };
        Ok(input)
    }

    fn parse_character_key(&self, keycode: u32, scancode: u32) -> Result<Input> {
        let hkl = self.hkl;
        let mut buf = [0; 8];
        let mut keyboard_state = [0; 256];
        let ret = unsafe {
            GetKeyboardState(&mut keyboard_state)?;
            // Clear Ctrl and Alt so ToUnicodeEx translates the base character.
            // We detect these modifiers separately via VK_CONTROL/VK_MENU.is_down().
            keyboard_state[VK_CONTROL.0 as usize] = 0;
            keyboard_state[VK_LCONTROL.0 as usize] = 0;
            keyboard_state[VK_RCONTROL.0 as usize] = 0;
            keyboard_state[VK_MENU.0 as usize] = 0;
            keyboard_state[VK_LMENU.0 as usize] = 0;
            keyboard_state[VK_RMENU.0 as usize] = 0;
            ToUnicodeEx(keycode, scancode, &keyboard_state, &mut buf, 0, hkl)
        };
        if ret == 0 {
            return Ok(Unknown(keycode));
        }
        let Ok(ch) = char::try_from_utf16(buf[0]) else {
            return Ok(Unknown(keycode));
        };
        match ch {
            number @ '0'..='9' => Ok(Number(number as usize - '0' as usize)),
            _ => {
                let kc = to_keycode(ch, keycode);
                if kc != UNKNOWN_KEYCODE {
                    Ok(Key(kc))
                } else {
                    Ok(Unknown(kc as u32))
                }
            }
        }
    }
}

/// Inputs that are easier to understand and handle.
/// See https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes for keycodes.
#[derive(Debug, Clone, Copy)]
enum Input {
    Number(usize),
    Key(u16),
    AltGrKey(u16),
    ShiftAltGr(u16),
    Space,
    Backspace(bool), // is Ctrl
    Enter,
    Tab,
    Left,
    Up,
    Right,
    Down,
    Unknown(#[allow(dead_code)] u32),
}

//----------------------------------------------------------------------------
//
//  After simplifying the overly-complicated events, we can start handling them.
//  Everthing after this point happens in "inner". Locking is nomore needed.
//
//----------------------------------------------------------------------------

impl TextServiceInner {
    fn test_input(&self, input: Input) -> Result<BOOL> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        trace!("test_input({:#04X?})", input);
        if self.composition.is_none() {
            match input {
                Key(_) => Ok(TRUE),
                AltGrKey(_) => Ok(TRUE),
                ShiftAltGr(_) => Ok(TRUE),
                _ => Ok(FALSE),
            }
        } else {
            Ok(TRUE)
        }
    }

    fn handle_input(&mut self, input: Input, context: Option<&ITfContext>) -> Result<BOOL> {
        log::info!(
            "[{}:{};{}] {}()",
            file!(),
            line!(),
            column!(),
            crate::function!()
        );

        trace!("handle_input({:?})", input);
        let Some(context) = context else {
            warn!("Context is None");
            return Ok(FALSE);
        };
        self.context = Some(context.clone());
        if self.composition.is_none() {
            match input {
                // letters start compositions. punctuators need to be re-mapped.
                Key(key) => {
                    log::trace!("Starting composition");
                    self.update_engine();

                    self.start_composition()?;
                    self.keypress(key, 0)?
                }
                AltGrKey(key) => {
                    log::trace!("Starting composition");
                    self.update_engine();

                    self.start_composition()?;
                    self.keypress(key, MODIFIER_ALT_GR)?
                }
                ShiftAltGr(key) => {
                    log::trace!("Starting composition");
                    self.update_engine();

                    self.start_composition()?;
                    self.keypress(key, MODIFIER_SHIFT ^ MODIFIER_ALT_GR)?
                }
                _ => return Ok(FALSE),
            }
        } else {
            match input {
                Number(0) => (),
                Number(number) => self.select(number - 1, None)?,
                Key(key) => self.keypress(key, 0)?,
                AltGrKey(key) => self.keypress(key, MODIFIER_ALT_GR)?,
                ShiftAltGr(key) => self.keypress(key, MODIFIER_SHIFT ^ MODIFIER_ALT_GR)?,
                Space => {
                    self.commit(Some(' '))?;
                }
                Enter => {
                    // self.release()?;
                    self.commit(Some('\n'))?;
                }
                Backspace(ctrl) => self.pop(ctrl)?,
                Left | Up => {
                    if let Ok(candidate_list) = self.candidate_list() {
                        candidate_list.move_highlight_prev();
                    }
                }
                Tab | Right | Down => {
                    if let Ok(candidate_list) = self.candidate_list() {
                        candidate_list.move_highlight_next();
                    }
                }
                _ => {
                    return Ok(FALSE);
                }
            }
        }
        Ok(TRUE)
    }

    // fn insert_char(&mut self, ch: char) -> Result<()> {
    //     //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

    //     self.char_buf.clear();
    //     self.char_buf.push(ch);
    //     let text = OsString::from(&self.char_buf).to_wchars();
    //     edit_session::insert_text(self.tid, self.context()?, &text)
    // }
}

//----------------------------------------------------------------------------
//
//  Preserved keys: Ctrl+Alt combinations are delivered via WM_SYSKEYDOWN
//  which TSF does not forward to the key event sink. We register them as
//  preserved keys so they arrive via OnPreservedKey instead.
//
//----------------------------------------------------------------------------

/// Encode virtual key code and shift flag into a GUID for preserved key identification.
const fn preserved_key_guid(vkey: u8, shift: bool) -> GUID {
    GUID {
        data1: 0xCAFEBA0E,
        data2: vkey as u16,
        data3: if shift { 1 } else { 0 },
        data4: [0x9A, 0xC4, 0x75, 0xF8, 0x42, 0x29, 0x47, 0xF5],
    }
}

/// Decode a preserved key GUID back to (vkey, shift).
fn decode_preserved_key_guid(guid: &GUID) -> Option<(u32, bool)> {
    if guid.data1 == 0xCAFEBA0E && guid.data4 == [0x9A, 0xC4, 0x75, 0xF8, 0x42, 0x29, 0x47, 0xF5] {
        Some((guid.data2 as u32, guid.data3 != 0))
    } else {
        None
    }
}

/// Virtual key codes to register as Ctrl+Alt preserved keys.
const PRESERVED_VKEYS: &[u32] = &[
    // A-Z
    0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
    0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 
    // 0-9
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    // OEM keys: ;: =+ ,< -_ .> /? `~ [{ \| ]} '"
    0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xDB, 0xDC, 0xDD, 0xDE,
];

pub fn register_preserved_keys(keystroke_mgr: &ITfKeystrokeMgr, tid: u32) {
    for &vkey in PRESERVED_VKEYS {
        for shift in [false, true] {
            let guid = preserved_key_guid(vkey as u8, shift);
            let mut modifiers = TF_MOD_CONTROL | TF_MOD_ALT;
            if shift {
                modifiers |= TF_MOD_SHIFT;
            }
            let prekey = TF_PRESERVEDKEY {
                uVKey: vkey,
                uModifiers: modifiers,
            };
            if let Err(e) = unsafe { keystroke_mgr.PreserveKey(tid, &guid, &prekey, &[]) } {
                trace!(
                    "Failed to register preserved key vkey={:#04X} shift={}: {}",
                    vkey, shift, e
                );
            }
        }
    }
}

pub fn unregister_preserved_keys(keystroke_mgr: &ITfKeystrokeMgr) {
    for &vkey in PRESERVED_VKEYS {
        for shift in [false, true] {
            let guid = preserved_key_guid(vkey as u8, shift);
            let mut modifiers = TF_MOD_CONTROL | TF_MOD_ALT;
            if shift {
                modifiers |= TF_MOD_SHIFT;
            }
            let prekey = TF_PRESERVEDKEY {
                uVKey: vkey,
                uModifiers: modifiers,
            };
            let _ = unsafe { keystroke_mgr.UnpreserveKey(&guid, &prekey) };
        }
    }
}
