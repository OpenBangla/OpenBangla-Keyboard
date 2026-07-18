use system_theme::{SystemTheme, ThemeScheme};

/// System color scheme, exposed across the C ABI.
#[repr(C)]
pub enum Theme {
    Light,
    Dark,
}

/// Returns the current OS color scheme. Falls back to `Light` if the
/// system theme cannot be determined (e.g. headless session / unsupported env).
#[no_mangle]
pub extern "C" fn get_system_theme() -> Theme {
    match SystemTheme::new().and_then(|t| t.get_scheme()) {
        Ok(ThemeScheme::Dark) => Theme::Dark,
        Ok(ThemeScheme::Light) => Theme::Light,
        Err(_) => Theme::Light,
    }
}
