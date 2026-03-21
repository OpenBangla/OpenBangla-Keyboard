use riti::config::Config;

use crate::Result;

use winreg::RegKey;
use winreg::enums::*;

/// Reads settings from Windows Registry (QSettings format)
pub struct Settings {
    base_key: RegKey,
}

impl Settings {
    /// Creates a new Settings instance, creating the key if it doesn't exist
    pub fn load_or_create() -> Result<Self> {
        let hkcu = RegKey::predef(HKEY_CURRENT_USER);
        let (base_key, _) = hkcu.create_subkey(r"Software\OpenBangla\Keyboard")?;
        Ok(Self { base_key })
    }

    // Helper methods for reading values
    fn get_string_from_key(&self, key: &RegKey, name: &str) -> Option<String> {
        key.get_value::<String, _>(name).ok()
    }

    fn get_bool(&self, subkey: &str, name: &str, default: bool) -> bool {
        self.base_key
            .open_subkey(subkey)
            .ok()
            .and_then(|key| self.get_string_from_key(&key, name))
            .map(|v| v == "true")
            .unwrap_or(default)
    }

    fn get_string(&self, subkey: &str, name: &str, default: &str) -> String {
        self.base_key
            .open_subkey(subkey)
            .ok()
            .and_then(|key| self.get_string_from_key(&key, name))
            .unwrap_or_else(|| default.to_string())
    }

    pub fn get_enter_key_closes_prev_win(&self) -> bool {
        // self.get_bool_direct("EnterKeyClosesPrevWin", false)
        self.get_bool(r"settings", "EnterKeyClosesPrevWin", false)
    }

    pub fn get_ansi_encoding(&self) -> bool {
        // self.get_bool_direct("ANSI", false)
        self.get_bool(r"settings", "ANSI", false)
    }

    pub fn get_smart_quoting(&self) -> bool {
        // self.get_bool_direct("SmartQuoting", true)
        self.get_bool(r"settings", "SmartQuoting", true)
    }

    // Layout settings
    pub fn get_layout_path(&self) -> String {
        self.get_string("layout", "path", "avro_phonetic")
    }

    // Fixed Layout settings
    pub fn get_show_prev_win_fixed(&self) -> bool {
        self.get_bool(r"settings\FixedLayout", "ShowPrevWin", true)
    }

    pub fn get_auto_vowel_form_fixed(&self) -> bool {
        self.get_bool(r"settings\FixedLayout", "AutoVowelForm", true)
    }

    pub fn get_auto_chandra_pos_fixed(&self) -> bool {
        self.get_bool(r"settings\FixedLayout", "AutoChandraPos", true)
    }

    pub fn get_traditional_kar_fixed(&self) -> bool {
        self.get_bool(r"settings\FixedLayout", "TraditionalKar", false)
    }

    pub fn get_number_pad_fixed(&self) -> bool {
        self.get_bool(r"settings\FixedLayout", "NumberPad", true)
    }

    pub fn get_old_reph(&self) -> bool {
        self.get_bool(r"settings\FixedLayout", "OldReph", true)
    }

    pub fn get_fixed_old_kar_order(&self) -> bool {
        self.get_bool(r"settings\FixedLayout", "OldKarOrder", false)
    }

    // Candidate Window settings
    pub fn get_candidate_win_horizontal(&self) -> bool {
        self.get_bool(r"settings\CandidateWin", "Horizontal", true)
    }

    pub fn get_show_cw_phonetic(&self) -> bool {
        self.get_bool(r"settings\CandidateWin", "Phonetic", true)
    }

    // Preview Window settings
    pub fn get_suggestion_include_english(&self) -> bool {
        self.get_bool(r"settings\PreviewWin", "IncludeEnglish", true)
    }
}

pub fn set_riti_config(settings: &Settings) -> Config {
    let mut config = Config::default();
    config.set_layout_file_path(&settings.get_layout_path());
    config.set_database_dir("");
    config.set_phonetic_suggestion(settings.get_show_cw_phonetic());
    config.set_suggestion_include_english(settings.get_suggestion_include_english());

    config.set_fixed_suggestion(settings.get_show_prev_win_fixed());
    config.set_fixed_automatic_vowel(settings.get_auto_vowel_form_fixed());
    config.set_fixed_automatic_chandra(settings.get_auto_chandra_pos_fixed());
    config.set_fixed_traditional_kar(settings.get_traditional_kar_fixed());
    config.set_fixed_numpad(settings.get_number_pad_fixed());
    config.set_fixed_old_reph(settings.get_old_reph());
    config.set_fixed_old_kar_order(settings.get_fixed_old_kar_order());

    config.set_ansi_encoding(settings.get_ansi_encoding());
    config.set_smart_quote(settings.get_smart_quoting());

    log::info!("Loaded Riti Config {:?}", config);

    config
}
