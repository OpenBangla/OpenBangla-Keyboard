//
//  AppDelegate.swift
//  OpenBangla
//
//  Created by Brett Gutstein on 5/1/20.
//  Copyright © 2020 Brett Gutstein. All rights reserved.
//  Copyright © 2025 OpenBangla. All rights reserved.
//

import Cocoa
import InputMethodKit

// global scope

var supportDirectory: SupportDirectory = SupportDirectory()

// only one candidate window for the entire input method —
// only one should be visible at a time
var candidatePanel = CandidateWindow()

var settings = OpenBanglaSettings(plistPath: supportDirectory.path() + "/org.openbangla.keyboard.plist")

var riti = RitiContext()

// logging

enum OpenBanglaLogLevel: Int {
    case VERBOSE = 0
    case ALWAYS_PRINT
}

#if DEBUG
let currentLogLevel: OpenBanglaLogLevel = .VERBOSE
#else
let currentLogLevel: OpenBanglaLogLevel = .ALWAYS_PRINT
#endif

func openbanglaLog(logLevel: OpenBanglaLogLevel = .ALWAYS_PRINT, _ format: String,
               file: String = #file, caller: String = #function, args: CVarArg...) {
    if (logLevel.rawValue >= currentLogLevel.rawValue) {
        let fileName = file.components(separatedBy: "/").last ?? ""
        NSLog("\(fileName):\(caller) " + format, args)
    }
}

// app delegate

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

    // IMKServer must be kept alive for the duration of the app's life.
    private var server: IMKServer?

    func applicationDidFinishLaunching(_ aNotification: Notification) {
        let version: String =
            Bundle.main.object(forInfoDictionaryKey: "CFBundleShortVersionString") as? String ?? ""
        let buildNumber: String =
            Bundle.main.object(forInfoDictionaryKey: "CFBundleVersion") as? String ?? ""
        
        #if DEBUG
        let buildType = "Debug"
        #else
        let buildType = "Release"
        #endif
        
        openbanglaLog("\(buildType) build version \(version) (\(buildNumber))")

        // no matter what Info.plist and openbangla.entitlements say, the connection name
        // requested from the sandbox seems to be $(PRODUCT_BUNDLE_IDENTIFIER)_Connection,
        // so Info.plist and openbangla.entitlements have been set to comply with this choice
        server = IMKServer(name: Bundle.main.infoDictionary?["InputMethodConnectionName"] as? String,
                           bundleIdentifier: Bundle.main.bundleIdentifier)
        
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        openbanglaLog("")
    }

}

