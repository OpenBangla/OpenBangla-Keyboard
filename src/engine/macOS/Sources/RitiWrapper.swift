//
//  RitiWrapper.swift
//  OpenBangla
//
//  Created by Muhammad Mominul Huque on 23/12/24.
//  Copyright © 2024 OpenBangla. All rights reserved.
//

import Foundation

typealias CRitiContext = OpaquePointer
typealias CRitiSuggestion = OpaquePointer
typealias CRitiConfig = OpaquePointer

/// Configuration for Riti context
public final class RitiConfig {
    private let ptr: CRitiConfig
    
    public init() {
        ptr = riti_config_new()
        
        openbanglaLog("Setting user directory: \(supportDirectory.path())")
        if !self.setUserDirectory(supportDirectory.path()) {
            NSLog("Error: Unable to set user directory!")
        }
    }
    
    deinit {
        riti_config_free(ptr)
    }
    
    /// Sets the layout file path. For Avro Phonetic, use "avro_phonetic"
    /// - Parameter path: Path to layout file or "avro_phonetic"
    /// - Returns: true if the layout file path is valid
    @discardableResult
    public func setLayoutFile(_ path: String) -> Bool {
        path.withCString { cPath in
            riti_config_set_layout_file(ptr, cPath)
        }
    }
    
    /// Sets the database directory path
    /// - Parameter path: Path to database directory
    /// - Returns: true if the path exists
    @discardableResult
    public func setDatabaseDirectory(_ path: String) -> Bool {
        path.withCString { cPath in
            riti_config_set_database_dir(ptr, cPath)
        }
    }
    
    /// Sets the user specific writable directory path
    /// - Parameter path: Path to the directory
    /// - Returns: true if the path exists
    @discardableResult
    public func setUserDirectory(_ path: String) -> Bool {
        path.withCString { cPath in
            riti_config_set_user_dir(ptr, cPath)
        }
    }
    
    public func setSuggestionIncludeEnglish(_ enabled: Bool) {
        riti_config_set_suggestion_include_english(ptr, enabled)
    }
    
    public func setPhoneticSuggestion(_ enabled: Bool) {
        riti_config_set_phonetic_suggestion(ptr, enabled)
    }
    
    public func setFixedSuggestion(_ enabled: Bool) {
        riti_config_set_fixed_suggestion(ptr, enabled)
    }
    
    public func setFixedAutoVowel(_ enabled: Bool) {
        riti_config_set_fixed_auto_vowel(ptr, enabled)
    }
    
    public func setFixedAutoChandra(_ enabled: Bool) {
        riti_config_set_fixed_auto_chandra(ptr, enabled)
    }
    
    public func setFixedTraditionalKar(_ enabled: Bool) {
        riti_config_set_fixed_traditional_kar(ptr, enabled)
    }
    
    public func setFixedOldReph(_ enabled: Bool) {
        riti_config_set_fixed_old_reph(ptr, enabled)
    }
    
    public func setFixedNumpad(_ enabled: Bool) {
        riti_config_set_fixed_numpad(ptr, enabled)
    }
    
    public func setFixedOldKarOrder(_ enabled: Bool) {
        riti_config_set_fixed_old_kar_order(ptr, enabled)
    }
    
    public func setAnsiEncoding(_ enabled: Bool) {
        riti_config_set_ansi_encoding(ptr, enabled)
    }
    
    public func setSmartQuote(_ enabled: Bool) {
        riti_config_set_smart_quote(ptr, enabled)
    }
    
    internal var rawPointer: CRitiConfig {
        ptr
    }
}

/// Suggestion result from Riti
public final class RitiSuggestion {
    private let ptr: CRitiSuggestion
    
    internal init(ptr: CRitiSuggestion) {
        self.ptr = ptr
    }
    
    deinit {
        riti_suggestion_free(ptr)
    }
    
    /// Number of suggestions
    public var count: UInt {
        riti_suggestion_get_length(ptr)
    }
    
    /// Whether this is a lonely suggestion (only one option)
    public var isLonely: Bool {
        riti_suggestion_is_lonely(ptr)
    }
    
    /// Whether the suggestion list is empty
    public var isEmpty: Bool {
        riti_suggestion_is_empty(ptr)
    }
    
    /// Index of previously selected suggestion
    public var previouslySelectedIndex: UInt {
        UInt(riti_suggestion_previously_selected_index(ptr))
    }
    
    /// Get suggestion at specific index
    /// - Parameter index: Index of the suggestion
    /// - Returns: Suggestion string
    public func suggestion(at index: UInt) -> String {
        let cStr = riti_suggestion_get_suggestion(ptr, index)
        defer { riti_string_free(cStr) }
        return String(cString: cStr!)
    }
    
    /// Get all suggestions as an array
    public var suggestions: [String] {
        (0..<count).map { suggestion(at: $0) }
    }
    
    /// Get the lonely suggestion (when there's only one option)
    public var lonelySuggestion: String {
        let cStr = riti_suggestion_get_lonely_suggestion(ptr)
        defer { riti_string_free(cStr) }
        return String(cString: cStr!)
    }
    
    /// Get auxiliary text
    public var auxiliaryText: String {
        let cStr = riti_suggestion_get_auxiliary_text(ptr)
        defer { riti_string_free(cStr) }
        return String(cString: cStr!)
    }
    
    /// Get pre-edit text for specific index
    /// - Parameter index: Index of the suggestion
    /// - Returns: Pre-edit text
    public func preEditText(at index: UInt) -> String {
        let cStr = riti_suggestion_get_pre_edit_text(ptr, index)
        defer { riti_string_free(cStr) }
        return String(cString: cStr!)
    }
}

/// Main Riti context for input method
public final class RitiContext {
    private let ptr: CRitiContext
    
    /// Create a new Riti context with configuration
    /// - Parameter config: Configuration object
    public init() {
        let config = loadConfig()
        ptr = riti_context_new_with_config(config.rawPointer)
    }
    
    deinit {
        riti_context_free(ptr)
    }
    
    /// Get suggestion for a key press
    /// - Parameters:
    ///   - key: Key code
    ///   - modifier: State of modifier keys
    ///   - selection: Previously selected user selection index (0 if not available)
    /// - Returns: Suggestion object
    public func getSuggestion(forKey key: UInt16, modifier: UInt8 = 0, selection: UInt8 = 0) -> RitiSuggestion {
        let suggestionPtr = riti_get_suggestion_for_key(ptr, key, modifier, selection)
        return RitiSuggestion(ptr: suggestionPtr!)
    }
    
    /// Commit a candidate from the suggestion list
    /// - Parameter index: Index of the candidate to commit
    /// This ends the ongoing input session
    public func commitCandidate(at index: UInt) {
        riti_context_candidate_committed(ptr, index)
    }
    
    /// Update the suggestion engine
    /// - Parameter config: Updated configuration
    public func updateEngine() {
        let config = loadConfig()
        riti_context_update_engine(ptr, config.rawPointer)
    }
    
    /// Check if there's an ongoing input session
    public var hasOngoingInputSession: Bool {
        riti_context_ongoing_input_session(ptr)
    }
    
    /// Finish the ongoing input session
    public func finishInputSession() {
        riti_context_finish_input_session(ptr)
    }
    
    /// Handle backspace event
    /// - Parameter ctrl: If true, deletes the whole word
    /// - Returns: New suggestion after applying backspace
    public func backspace(ctrl: Bool = false) -> RitiSuggestion {
        let suggestionPtr = riti_context_backspace_event(ptr, ctrl)
        return RitiSuggestion(ptr: suggestionPtr!)
    }
}

// MARK: - Convenience Extensions

func loadConfig() -> RitiConfig {
    settings.printAllSettings()
    
    let config = RitiConfig()
    
    guard config.setLayoutFile(settings.layoutPath) else {
        NSLog("!!!!!!!!!! Layout path invalid! !!!!!!!!!!!!")
        return RitiConfig()
    }
    
    config.setSuggestionIncludeEnglish(settings.suggestionIncludeEnglish)
    config.setPhoneticSuggestion(settings.showCWPhonetic)
    
    config.setFixedSuggestion(settings.showPrevWinFixed)
    config.setFixedAutoChandra(settings.autoChandraPosFixed)
    config.setFixedAutoVowel(settings.autoVowelFormFixed)
    config.setFixedNumpad(settings.numberPadFixed)
    config.setFixedOldKarOrder(settings.fixedOldKarOrder)
    config.setFixedOldReph(settings.oldReph)
    
    config.setSmartQuote(settings.smartQuoting)
    config.setAnsiEncoding(settings.ansiEncoding)
    
    return config
}

extension RitiSuggestion: Sequence {
    public func makeIterator() -> SuggestionIterator {
        SuggestionIterator(suggestion: self)
    }
    
    public struct SuggestionIterator: IteratorProtocol {
        private let suggestion: RitiSuggestion
        private var currentIndex: UInt = 0
        
        init(suggestion: RitiSuggestion) {
            self.suggestion = suggestion
        }
        
        public mutating func next() -> String? {
            guard currentIndex < suggestion.count else { return nil }
            let value = suggestion.suggestion(at: currentIndex)
            currentIndex += 1
            return value
        }
    }
}

extension RitiSuggestion: Collection {
    public var startIndex: UInt { 0 }
    public var endIndex: UInt { count }
    
    public func index(after i: UInt) -> UInt {
        i + 1
    }
    
    public subscript(position: UInt) -> String {
        suggestion(at: position)
    }
}
