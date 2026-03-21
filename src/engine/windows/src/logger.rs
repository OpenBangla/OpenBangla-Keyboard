use std::{env, fs, panic, path::PathBuf};

use chrono::Local;
use log::{Level, LevelFilter::*};

#[cfg(debug_assertions)]
const RELEASE: bool = false;
#[cfg(not(debug_assertions))]
const RELEASE: bool = true;

#[cfg(target_pointer_width = "64")]
const ARCHITECHTURE: &str = "x64";
#[cfg(target_pointer_width = "32")]
const ARCHITECHTURE: &str = "x86";

pub fn setup() {
    let _ = _setup();
}

fn _setup() -> Result<(), fern::InitError> {
    let path = if let Ok(appdata) = env::var("LOCALAPPDATA") {
        PathBuf::from(appdata).join("OpenBangla")
    } else {
        return Ok(());
    };
    fs::create_dir_all(&path)?;
    let path = path.join("log.txt");
    // if let Ok(meta) = fs::metadata(&path)
    //     && meta.file_size() >= 5 * 1024 * 1024
    // {
    //     let _ = fs::remove_file(&path);
    // }
    fern::Dispatch::new()
        .format(|out, message, record| {
            out.finish(format_args!(
                "{} [{:<3}({})] {}",
                Local::now().format("%Y-%m-%d %H:%M:%S"),
                record.level().abbr(),
                ARCHITECHTURE,
                message
            ))
        })
        .level(if RELEASE { Info } else { Trace })
        .chain(fern::log_file(path)?)
        .apply()?;
    panic::set_hook(Box::new(|panic_info| {
        log::error!("Fatal error happend. {panic_info}")
    }));
    Ok(())
}

trait LevelExt {
    fn abbr(self) -> &'static str;
}

impl LevelExt for Level {
    fn abbr(self) -> &'static str {
        match self {
            Level::Error => "ERR",
            Level::Warn => "WRN",
            Level::Info => "INF",
            Level::Debug => "DBG",
            Level::Trace => "TRC",
        }
    }
}
