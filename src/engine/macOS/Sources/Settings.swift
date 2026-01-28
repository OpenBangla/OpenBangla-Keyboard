//
//  Settings.swift
//  OpenBangla
//
//  Created by Muhammad Mominul Huque on 3/1/26.
//  Copyright © 2026 OpenBangla. All rights reserved.
//


import Foundation
import CoreGraphics

// MARK: - Main Plist Loader
class QtPlistLoader {
    private var data: [String: Any]
    private let fileURL: URL
    
    init(fileURL: URL) {
        self.fileURL = fileURL
        
        guard FileManager.default.fileExists(atPath: fileURL.path) else {
            openbanglaLog("Settings file doesn't exist at: \(fileURL.path)")
            self.data = [:]
            return
        }
        
        guard let plistData = try? Data(contentsOf: fileURL),
              let plist = try? PropertyListSerialization.propertyList(
                from: plistData,
                options: [],
                format: nil
              ) as? [String: Any] else {
            self.data = [:]
            openbanglaLog("Failed to parse plist")
            return
        }
        
        self.data = plist
    }
    
    convenience init(path: String) {
        self.init(fileURL: URL(fileURLWithPath: path))
    }
    
    // MARK: - Getters
    
    func getString(_ key: String, defaultValue: String = "") -> String {
        return data[key] as? String ?? defaultValue
    }
    
    func getBool(_ key: String, defaultValue: Bool = false) -> Bool {
        return data[key] as? Bool ?? defaultValue
    }
    
    func getInt(_ key: String, defaultValue: Int = 0) -> Int {
        return data[key] as? Int ?? defaultValue
    }
    
    func getValue(_ key: String) -> Any? {
        return data[key]
    }
    
    // MARK: - Setters
    
    func setString(_ key: String, value: String) {
        data[key] = value
    }
    
    func setBool(_ key: String, value: Bool) {
        data[key] = value
    }
    
    func setInt(_ key: String, value: Int) {
        data[key] = value
    }
    
    func setValue(_ key: String, value: Any) {
        data[key] = value
    }
    
    // MARK: - Debugging
    
    func printAll() {
        openbanglaLog("=== Plist Contents ===")
        for (key, value) in data.sorted(by: { $0.key < $1.key }) {
            openbanglaLog("\(key): \(value)")
        }
    }
}

// MARK: - OpenBangla Settings Wrapper
class OpenBanglaSettings {
    private var loader: QtPlistLoader
    private let path: String
    
    init(plistPath: String) {
        let loader = QtPlistLoader(path: plistPath)
        self.loader = loader
        self.path = plistPath
    }
    
    public func update() {
        let loader = QtPlistLoader(path: path)
        self.loader = loader
    }
    
    // MARK: - Layout Settings
    
    var layoutName: String {
        get { loader.getString("layout.name", defaultValue: "Avro Phonetic") }
    }
    
    var layoutPath: String {
        get { loader.getString("layout.path", defaultValue: "avro_phonetic") }
    }
    
    // MARK: - General Settings
    
    var enterKeyClosesPrevWin: Bool {
        get { loader.getBool("settings.EnterKeyClosesPrevWin", defaultValue: false) }
    }
    
    var ansiEncoding: Bool {
        get { loader.getBool("settings.ANSI", defaultValue: false) }
    }
    
    var smartQuoting: Bool {
        get { loader.getBool("settings.SmartQuoting", defaultValue: true) }
    }
    
    var previousUserDataRemains: Bool {
        get { loader.getBool("settings.PreviousUserDataRemains", defaultValue: true) }
    }
    
    // MARK: - Fixed Layout Settings
    
    var showPrevWinFixed: Bool {
        get { loader.getBool("settings.FixedLayout.ShowPrevWin", defaultValue: true) }
    }
    
    var autoVowelFormFixed: Bool {
        get { loader.getBool("settings.FixedLayout.AutoVowelForm", defaultValue: true) }
    }
    
    var autoChandraPosFixed: Bool {
        get { loader.getBool("settings.FixedLayout.AutoChandraPos", defaultValue: true) }
    }
    
    var traditionalKarFixed: Bool {
        get { loader.getBool("settings.FixedLayout.TraditionalKar", defaultValue: false) }
    }
    
    var numberPadFixed: Bool {
        get { loader.getBool("settings.FixedLayout.NumberPad", defaultValue: true) }
    }
    
    var oldReph: Bool {
        get { loader.getBool("settings.FixedLayout.OldReph", defaultValue: true) }
    }
    
    var fixedOldKarOrder: Bool {
        get { loader.getBool("settings.FixedLayout.OldKarOrder", defaultValue: false) }
    }
    
    // MARK: - Candidate Window Settings
    
    var candidateWinHorizontal: Bool {
        get { loader.getBool("settings.CandidateWin.Horizontal", defaultValue: true) }
    }
    
    var showCWPhonetic: Bool {
        get { loader.getBool("settings.CandidateWin.Phonetic", defaultValue: true) }
    }
    
    // MARK: - Preview Window Settings
    
    var suggestionIncludeEnglish: Bool {
        get { loader.getBool("settings.PreviewWin.IncludeEnglish", defaultValue: true) }
    }
    
    // MARK: - Debug
    
    func printAllSettings() {
        loader.printAll()
    }
}
