//
//  SupportDirectory.swift
//  OpenBangla
//
//  Created by Muhammad Mominul Huque on 3/1/26.
//  Copyright © 2026 OpenBangla. All rights reserved.
//

import Foundation

class SupportDirectory {
    public let applicationSupportURL: URL
    
    public init() {
        applicationSupportURL =
            FileManager.default.urls(for: .applicationSupportDirectory, in: .userDomainMask).first!
                .appendingPathComponent(Bundle.main.bundleIdentifier!, isDirectory: true)

        if !((try? applicationSupportURL.checkResourceIsReachable()) ?? false) {
            openbanglaLog("creating application support directory")

            do {
                try FileManager.default.createDirectory(at: applicationSupportURL,
                                                        withIntermediateDirectories: true, attributes: nil)
            } catch {
                openbanglaLog("couldn't create application support directory: \(error)")
            }
        }
    }
    
    public func path() -> String {
        return applicationSupportURL.path
    }
}
