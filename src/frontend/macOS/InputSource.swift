import Carbon
import Foundation

private let bundleID = "org.openbangla.inputmethod.keyboard"
private let imeName  = "OpenBangla"

// MARK: - IME location

/// Returns the URL of the installed IME bundle, checking system-wide and
/// per-user Input Methods directories in that order.
private func locateIME() -> URL? {
    let candidates = [
        URL(fileURLWithPath: "/Library/Input Methods/\(imeName).app"),
        FileManager.default.homeDirectoryForCurrentUser
            .appendingPathComponent("Library/Input Methods/\(imeName).app"),
    ]
    return candidates.first { FileManager.default.fileExists(atPath: $0.path) }
}

// MARK: - TIS helpers

/// Find the OpenBangla input source, **including disabled / not-yet-enabled
/// sources** (`includeAllInstalled: true`).
private func findInputSource() -> TISInputSource? {
    let props = [kTISPropertyBundleID: bundleID] as CFDictionary
    guard
        let list = TISCreateInputSourceList(props, true)?.takeRetainedValue() as? [TISInputSource]
    else { return nil }
    return list.first
}

private func getBool(_ source: TISInputSource, _ key: CFString) -> Bool {
    guard let ref = TISGetInputSourceProperty(source, key) else { return false }
    return CFBooleanGetValue(unsafeBitCast(ref, to: CFBoolean.self))
}

// MARK: - Public API

/// Returns `true` when the OpenBangla input source is installed and enabled.
public func getInputSourceEnabled() -> Bool {
    guard let source = findInputSource() else { return false }
    return getBool(source, kTISPropertyInputSourceIsEnabled)
}

/// Registers and enables the OpenBangla input source.
public func setupOpenBanglaInputSource() {
    if getInputSourceEnabled() {
        NSLog("OpenBangla input source is already enabled.")
        return
    }

    guard let imeURL = locateIME() else {
        NSLog("Error: OpenBangla.app not found in any Input Methods directory.")
        return
    }

    // Register the bundle so TIS can see it even before it is enabled.
    TISRegisterInputSource(imeURL as CFURL)

    guard let source = findInputSource() else {
        NSLog("Error: OpenBangla input source not found after registration (\(imeURL.path)).")
        return
    }

    let status = TISEnableInputSource(source)
    if status == noErr {
        NSLog("OpenBangla IME enabled.")
    } else {
        NSLog("Error: TISEnableInputSource failed with status \(status).")
    }
}
