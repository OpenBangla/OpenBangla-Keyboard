use windows::{
    Win32::UI::TextServices::{ITfContext, ITfDocumentMgr, ITfThreadMgrEventSink_Impl},
    core::Result,
};

use super::TextService;

#[allow(non_snake_case, unused)]
impl ITfThreadMgrEventSink_Impl for TextService {
    fn OnInitDocumentMgr(&self, pdim: Option<&ITfDocumentMgr>) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        Ok(())
    }
    fn OnUninitDocumentMgr(&self, pdim: Option<&ITfDocumentMgr>) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        Ok(())
    }
    fn OnSetFocus(
        &self,
        focus: Option<&ITfDocumentMgr>,
        prevfocus: Option<&ITfDocumentMgr>,
    ) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        self.write()?.abort()
    }
    fn OnPushContext(&self, pic: Option<&ITfContext>) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        Ok(())
    }
    fn OnPopContext(&self, pic: Option<&ITfContext>) -> Result<()> {
        //log::info!("[{}:{};{}] {}()", file!(), line!(), column!(), crate::function!());

        Ok(())
    }
}
