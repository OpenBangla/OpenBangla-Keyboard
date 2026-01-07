//
//  OpenBanglaIMKInputController.swift
//  OpenBangla
//
//  Created by Brett Gutstein on 5/1/20.
//  Copyright © 2020 Brett Gutstein. All rights reserved.
//  Copyright © 2025 OpenBangla. All rights reserved.
//

import InputMethodKit

class OpenBanglaIMKInputController: IMKInputController {

    // fields and constructor

    private static let escapeCharacter: Int = 0x1b // int corresponding to Unicode value of escape
    private static let digitChars: Set<Character> = ["0", "1", "2" ,"3", "4", "5", "6", "7", "8", "9"]

    private var _originalString: String = "" // what the user typed
    private var _composedString: String = "" // currently selected transliteration candidate
    private var _candidates: [String] = [] // list of candidates to choose from
    private var _prevSelectedCandidateIndex: UInt = 0
    private var _selectedCandidateIndex: UInt = 0

    // handle system events

    // called once per client the first time it gets focus
    override init!(server: IMKServer!, delegate: Any!, client inputClient: Any!) {
        NSLog("OpenBanglaIMKInputController::init")
        openbanglaLog(logLevel: .VERBOSE, "client \(String(describing: inputClient))")
        super.init(server: server, delegate: delegate, client: inputClient)
    }

    // called when the client loses focus
    override func deactivateServer(_ sender: Any!) {
        NSLog("OpenBanglaIMKInputController::deactivateServer")
        openbanglaLog(logLevel: .VERBOSE, "client \(String(describing: sender))")
        commitComposition(sender)
    }

    // called when the client gains focus
    override func activateServer(_ sender: Any!) {
        NSLog("OpenBanglaIMKInputController::activateServer")
        openbanglaLog(logLevel: .VERBOSE, "client \(String(describing: sender))")
        // the user may have been changing keyboard layouts while we were deactivated
        // (but the controller survives so init() may not be called again).
        // openbangla will use the most recent ASCII capable keyboard layout to translate key
        // events (see TextInputSources.h:TISSetInputMethodKeyboardLayoutOverride()).
        // set the candidates window to use the same keyboard layout.
        let lastASCIIlayout = TISCopyCurrentASCIICapableKeyboardLayoutInputSource().takeRetainedValue()
        candidatesWindow.setSelectionKeysKeylayout(lastASCIIlayout)
    }

    // generate inputmethod menu and handle user clicks
    override func menu() -> NSMenu! {
        openbanglaLog(logLevel: .VERBOSE, "")
        let menu = NSMenu()
        menu.addItem(NSMenuItem(title: "About", action: #selector(showAboutPanel(_:)), keyEquivalent: ""))
        return menu
    }

    @objc private func showAboutPanel(_ sender: Any) {
        openbanglaLog(logLevel: .VERBOSE, "")

        let github = NSMutableAttributedString(string: "https://github.com/OpenBangla/OpenBangla-Keyboard")
        github.addAttribute(.link, value: "https://github.com/OpenBangla/OpenBangla-Keyboard",
                            range: NSRange(location: 0, length: github.length))

        let website = NSMutableAttributedString(string: "https://openbangla.org")
        website.addAttribute(.link, value: "https://openbangla.org",
                             range: NSRange(location: 0, length: website.length))

        let credits = NSMutableAttributedString(string:"")
        credits.append(github)
        credits.append(NSMutableAttributedString(string: "\n"))
        credits.append(website)

        let paragraphStyle = NSMutableParagraphStyle()
        paragraphStyle.alignment = .center
        credits.addAttribute(.paragraphStyle, value: paragraphStyle,
                             range: NSRange(location: 0, length: credits.length))

        // setting the activation policy and calling activate() doesn't
        // make our app visible on the dock or make a blank window appear,
        // but it allows our about panel to display in front of other apps
        NSApp.setActivationPolicy(.accessory)
        NSApp.activate(ignoringOtherApps: true)
        NSApp.orderFrontStandardAboutPanel(options: [ .credits : credits ])
    }

    // getters

    // called to obtain the composed string
    override func composedString(_ sender: Any!) -> Any! {
        NSLog("OpenBanglaIMKInputController::composedString")
        //openbanglaLog("")
        return self._composedString
    }

    // called to obtain the original string
    override func originalString(_ sender: Any!) -> NSAttributedString! {
        NSLog("OpenBanglaIMKInputController::originalString")
        //openbanglaLog("")
        return NSAttributedString(string: self._originalString)
    }

    // called to obtain the candidates array (used by IMKCandidates:update())
    override func candidates(_ sender: Any!) -> [Any]! {
        NSLog("OpenBanglaIMKInputController::candidates")
        //openbanglaLog("")
        return self._candidates
    }

    // helper functions

    // convert Int representing a Unicode character to a Character
    private func toChar(_ unicodeInt: Int) -> Character {
        NSLog("OpenBanglaIMKInputController::toChar")
        return Character(UnicodeScalar(unicodeInt) ?? UnicodeScalar(0))
    }

    // handle deficiencies in the swift API: untyped senders should cast successfully
    private func downcastSender(_ sender: Any!) -> (IMKTextInput & IMKUnicodeTextInput) {
        NSLog("OpenBanglaIMKInputController::downcastSender")
        guard let downcast = sender as? (IMKTextInput & IMKUnicodeTextInput) else {
            openbanglaLog("sender '\(String(describing: sender))' did not downcast, trying client()")
            return client() as! (IMKTextInput & IMKUnicodeTextInput)
        }
        return downcast
    }

    // insert marked text at the cursor
    private func writeMarkToClient(_ client: (IMKTextInput & IMKUnicodeTextInput),_ string: String) {
        NSLog("OpenBanglaIMKInputController::writeMarkToClient - '\(string)'")
        client.setMarkedText(string,
                             selectionRange: NSMakeRange(0, string.count),
                             replacementRange: NSMakeRange(NSNotFound, NSNotFound))
    }

    // insert text at the cursor, overwriting marked text that may be there
    private func writeTextToClient(_ client: (IMKTextInput & IMKUnicodeTextInput),_ string: String) {
        NSLog("OpenBanglaIMKInputController::writeTextToClient = '\(string)'")
        client.insertText(string, replacementRange: NSMakeRange(NSNotFound, NSNotFound))
    }

    // output to client

    // commit the current transliteration
    override func commitComposition(_ sender: Any!) {
        NSLog("OpenBanglaIMKInputController::commitComposition")
        openbanglaLog(logLevel: .VERBOSE, "--------- composed string '\(String(describing: self._composedString))'")

        writeTextToClient(downcastSender(sender), self._composedString)
        
        if candidatesWindow.isVisible() {
            riti.commitCandidate(at: self._selectedCandidateIndex)
            candidatesWindow.hide()
        } else {
            riti.finishInputSession()
        }

        self._originalString = ""
        self._composedString = ""
        self._candidates = []
        self._prevSelectedCandidateIndex = 0
        self._selectedCandidateIndex = 0
    }

    // update mark/candidates for the current transliteration
    override func updateComposition() {
        NSLog("OpenBanglaIMKInputController::updateComposition")
        openbanglaLog(logLevel: .VERBOSE, "original string '\(self._originalString))'")

        writeMarkToClient(downcastSender(self.client()), self._originalString)

        if self._candidates.count == 0 {
            candidatesWindow.hide()
        } else {
            candidatesWindow.update()
            if !candidatesWindow.isVisible() {
                candidatesWindow.show()
            }
            
            if self._prevSelectedCandidateIndex != 0 {
                for _ in 0..<self._prevSelectedCandidateIndex {
                    if settings.candidateWinHorizontal {
                        candidatesWindow.moveRight(self.client())
                    } else {
                        candidatesWindow.moveDown(self.client())
                    }
                }
            }
        }
    }

    // cancel the current transliteration
    override func cancelComposition() {
        NSLog("OpenBanglaIMKInputController::cancelComposition")
        openbanglaLog(logLevel: .VERBOSE, "")

        writeMarkToClient(downcastSender(self.client()), "")
        
        if riti.hasOngoingInputSession {
            riti.finishInputSession()
        }

        self._originalString = ""
        self._composedString = ""
        self._candidates = []
        self._prevSelectedCandidateIndex = 0
        self._selectedCandidateIndex = 0

        candidatesWindow.hide()
    }

    // input from candidates window

    // user highlighted a selection
    override func candidateSelectionChanged(_ candidateString: NSAttributedString!) {
        NSLog("OpenBanglaIMKInputController::candidateSelectionChanged")
        openbanglaLog(logLevel: .VERBOSE, "selection '\(String(describing: candidateString))'")
        self._composedString = candidateString.string
        
        for (index, candidate) in self._candidates.enumerated() {
            if candidate == self._composedString {
                self._selectedCandidateIndex = UInt(index)
            }
        }
    }

    // user made a selection
    override func candidateSelected(_ candidateString: NSAttributedString!) {
        NSLog("OpenBanglaIMKInputController::candidateSelected")
        openbanglaLog(logLevel: .VERBOSE, "selection '\(String(describing: candidateString))'")

        self._composedString = candidateString.string
        commitComposition(self.client())
        NSLog("OpenBanglaIMKInputController::candidateSelected - wrote a space")
        writeTextToClient(downcastSender(self.client()), " ")
    }

    // input from client

    // handle user actions
    override func handle(_ event: NSEvent!, client sender: Any!) -> Bool {
        openbanglaLog(logLevel: .VERBOSE, "handle() event '\(String(describing: event))'")
        switch event.type {
        case .keyDown: return handleKeyDown(event, downcastSender(sender))
        default: return false
        }
    }

    // handle user keypress
    private func handleKeyDown(_ event: NSEvent, _ sender: (IMKTextInput & IMKUnicodeTextInput)) -> Bool {
        // openbanglaLog("")
        NSLog("OpenBanglaIMKInputController::handleKeyDown - v05-01-1")
        let keyCode = event.keyCode
        NSLog("OpenBanglaIMKInputController::handleKeyDown - keycode: '\(keyCode)'")
        
        if !riti.hasOngoingInputSession {
            settings.update()
            
            if settings.candidateWinHorizontal {
                candidatesWindow.setPanelType(kIMKSingleRowSteppingCandidatePanel)
            } else {
                candidatesWindow.setPanelType(kIMKSingleColumnScrollingCandidatePanel)
            }
            
            riti.updateEngine()
        }
        
        // Space and Enter key
        if keyCode == 49 || keyCode == 36 {
            if riti.hasOngoingInputSession {
                commitComposition(sender)
            }
            
            return false
        }
        
        let charcount = event.characters?.count
        if charcount != 1 {
            openbanglaLog("unexpected charcount \(String(describing: charcount))")
            return false
        }
        
        let char = event.characters!.first!
        NSLog("OpenBanglaIMKInputController::handleKeyDown -> '\(char)'")
        
        let ctrl = event.modifierFlags.contains(.control)
        let alt = event.modifierFlags.contains(.option)
        let shift = event.modifierFlags.contains(.shift)

        // if a command sequence was not handled by the application, ignore it
        // We capture the Backspace (delete) key
        if event.modifierFlags.contains(.command) && keyCode != 51  {
            return false
        }
        

        if riti.hasOngoingInputSession {
            // send relevant keys to the candidates window
            if candidatesWindow.isVisible() && (char == toChar(NSCarriageReturnCharacter) ||
               char == toChar(NSUpArrowFunctionKey) ||
               char == toChar(NSDownArrowFunctionKey) ||
               char == toChar(NSRightArrowFunctionKey) ||
                char == toChar(NSLeftArrowFunctionKey)) {
                //candidatesWindow.interpretKeyEvents([event])
                // use this private function to workaround buggy candidates
                // window as of 10.15.3
                NSLog("------------ sending relevant keys to the candidates window - char: '\(char)'")
                candidatesWindow.perform(Selector(("handleKeyboardEvent:")), with: event)
                return true

            // backspace is straightforward
            } else if char == toChar(NSBackspaceCharacter) {
                let update = riti.backspace(ctrl: event.modifierFlags.contains(.command))

                if !update.isEmpty {
                    if !update.isLonely {
                        self._originalString = update.auxiliaryText
                        self._composedString = update.preEditText(at: 0)
                        self._candidates = update.suggestions
                    } else {
                        self._originalString = update.lonelySuggestion
                        self._composedString = update.lonelySuggestion
                        self._candidates = []
                    }
                    
                    updateComposition()
                } else {
                    cancelComposition()
                }
                
                return true

            // escape key cancels the composition
            } else if char == toChar(OpenBanglaIMKInputController.escapeCharacter) {
                cancelComposition()
                return true
            } else if char == " " {
                commitComposition(sender)
                return false
            }
        }
        
        guard let keycode = charToKeycode(char) else {
            NSLog("OpenBanglaIMKInputController::handleKeyDown -> unexpected char '\(char)'")
            return false
        }
        
        var modifiers: UInt8 = 0
        
        if ctrl && alt {
            modifiers |= 1 << 1; // MODIFIER_ALT_GR
        }
        
        if shift {
            modifiers |= 1 << 0 // MODIFIER_SHIFT
        }
        
        let suggestion = riti.getSuggestion(forKey: keycode, modifier: modifiers, selection: UInt8(self._selectedCandidateIndex))
        
        if !suggestion.isEmpty {
            if !suggestion.isLonely {
                self._originalString = suggestion.auxiliaryText
                self._composedString = suggestion.preEditText(at: 0)
                self._candidates = suggestion.suggestions
                self._prevSelectedCandidateIndex = suggestion.previouslySelectedIndex
            } else {
                self._originalString = suggestion.lonelySuggestion
                self._composedString = suggestion.lonelySuggestion
                self._candidates = []
            }
            
            updateComposition()
            return true
        } else {
            // Corner case: When old style kar typing is enabled, a lonely suggestion and an empty
            // suggestion is not distinguishable. So we accept the key event if a input session
            // is ongoing.
            return riti.hasOngoingInputSession
        }
    }
}
