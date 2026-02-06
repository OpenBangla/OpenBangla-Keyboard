
import Carbon
import Foundation

let bundleID = "org.openbangla.inputmethod.keyboard"

func getInputSource() -> TISInputSource? {
    // Create properties to find your specific input source
    let properties: [CFString: Any] = [
        kTISPropertyBundleID: bundleID
    ]
    
    guard let sourceList = TISCreateInputSourceList(properties as CFDictionary, false)?.takeRetainedValue() as? [TISInputSource],
          let inputSource = sourceList.first else {
        NSLog("Error: Could not find OpenBangla input source")
        return nil
    }
    
    return inputSource
}

public func getInputSourceEnabled() -> Bool { 
    guard let inputSource = getInputSource() else {
        return false
    }

    let isEnabled = TISGetInputSourceProperty(inputSource, kTISPropertyInputSourceIsEnabled)

    if let enabled = isEnabled, Unmanaged<CFBoolean>.fromOpaque(enabled).takeUnretainedValue() == kCFBooleanTrue {
        return true
    }

    return false
}

public func setupOpenBanglaInputSource() {
    guard let inputSource = getInputSource() else {
        return
    }

    let enableStatus = TISEnableInputSource(inputSource)
    if enableStatus != noErr {
        NSLog("Error: Failed to enable input source: \(enableStatus)")
        return
    }
    
    // let selectStatus = TISSelectInputSource(inputSource)
    // if selectStatus != noErr {
    //     print("Failed to select input source: \(selectStatus)")
    //     return
    // }
    
    NSLog("OpenBangla IME enabled!")
}
