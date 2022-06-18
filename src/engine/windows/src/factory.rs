use windows::Win32::Foundation::BOOL;
use windows::core::{implement, IUnknown, GUID};
use windows::Win32::System::Com::{IClassFactory, IClassFactory_Impl};


#[implement(IClassFactory)]
struct Factory {
    //
}

impl IClassFactory_Impl for Factory {
    fn CreateInstance(&self,punkouter: &Option<IUnknown>,riid: *const GUID, ppvobject: *mut *mut core::ffi::c_void) ->  windows::core::Result<()> {
        todo!()
    }

    fn LockServer(&self,flock:BOOL) ->  windows::core::Result<()> {
        todo!()
    }
}
