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

// only need one candidates window for the entire input method
// because only one such window should be visible at a time
var candidatesWindow: IMKCandidates = IMKCandidates()

var settings = OpenBanglaSettings(plistPath: supportDirectory.path() + "/org.openbangla.keyboard.plist")

var riti = RitiContext()

// logging

enum OpenBanglaLogLevel: Int {
    case VERBOSE = 0
    case ALWAYS_PRINT
}
let currentLogLevel: OpenBanglaLogLevel = .VERBOSE
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

    func applicationDidFinishLaunching(_ aNotification: Notification) {
        let version: String =
            Bundle.main.object(forInfoDictionaryKey: "CFBundleShortVersionString") as? String ?? ""
        let buildNumber: String =
            Bundle.main.object(forInfoDictionaryKey: "CFBundleVersion") as? String ?? ""
        openbanglaLog("version \(version) (\(buildNumber))")

        // no matter what Info.plist and openbangla.entitlements say, the connection name
        // requested from the sandbox seems to be $(PRODUCT_BUNDLE_IDENTIFIER)_Connection,
        // so Info.plist and openbangla.entitlements have been set to comply with this choice
        let server = IMKServer(name: Bundle.main.infoDictionary?["InputMethodConnectionName"] as? String,
                               bundleIdentifier: Bundle.main.bundleIdentifier)
        
        var kind = 0
        
        if settings.candidateWinHorizontal {
            kind = kIMKSingleRowSteppingCandidatePanel
        } else {
            kind = kIMKSingleColumnScrollingCandidatePanel
        }

        // scrolling to the bottom of the scrolling panel puts selection numbers out of alignment
        candidatesWindow = IMKCandidates(server: server,
                                         panelType: kind)
//                                         panelType: kIMKSingleColumnScrollingCandidatePanel)
                                         //panelType: kIMKSingleRowSteppingCandidatePanel)

        // as of 10.15.3, default candidates window key event handling is buggy
        // (number selector keys don't work). workaround involves bypassing default window handling.
        candidatesWindow.setAttributes([IMKCandidatesSendServerKeyEventFirst : NSNumber(booleanLiteral: true)])
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        openbanglaLog("")
    }

}

