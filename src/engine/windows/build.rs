use std::{env, io};

use winres::WindowsResource;

fn main() -> io::Result<()> {
    if env::var_os("CARGO_CFG_WINDOWS").is_none() {
        return Ok(());
    }
    WindowsResource::new()
        .set_icon_with_id("../../frontend/openbangla.ico", "0")
        .compile()
}
