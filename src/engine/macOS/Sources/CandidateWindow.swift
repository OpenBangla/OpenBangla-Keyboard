//
//  CandidateWindow.swift
//  OpenBangla Keyboard — macOS IMKit engine
//
//  A custom non-activating floating panel that shows:
//    • a preedit row  — raw Latin input (e.g. "ami"), rendered in a muted colour
//    • a full-width divider
//    • a horizontal candidate row — Bengali suggestions; selected one in accent colour
//
//  Design notes
//  ────────────────────────────────────────────────────────────────────────────
//  • CGShieldingWindowLevel() places the panel above the menu bar, which is
//    necessary for it to appear over full-screen apps (Squirrel technique).
//  • .nonactivatingPanel + .borderless  ─ the panel never steals key focus.
//  • NSVisualEffectView(.windowBackground) ─ native material that adapts to
//    both light and dark mode automatically.
//  • CAShapeLayer mask on the vibrancy view ─ correct rounded-corner clipping
//    for the blur backdrop at all four edges (Squirrel technique).
//  • Four-edge screen clamping + above/below cursor flip ─ the panel never
//    overflows any screen edge regardless of cursor position.
//  • acceptsMouseMovedEvents is enabled only while the cursor is inside the
//    panel, avoiding a global mouse-move overhead during normal typing.
//
//  Integration (in OpenBanglaIMKInputController)
//  ────────────────────────────────────────────────────────────────────────────
//  1. Replace the `candidatesWindow` (IMKCandidates) field with:
//       private let candidatePanel = CandidateWindow()
//     and set:
//       candidatePanel.delegate = self
//
//  2. In updateComposition(), instead of candidatesWindow.update()/show():
//       let cursorRect = (client() as! IMKTextInput)
//                           .firstRect(forCharacterRange: client().markedRange(),
//                                      actualRange: nil)
//       candidatePanel.update(preedit:          _originalString,
//                             candidates:       _candidates,
//                             highlightedIndex: Int(_selectedCandidateIndex),
//                             cursorRect:       cursorRect)
//
//  3. In commitComposition() / cancelComposition():
//       candidatePanel.hide()
//
//  4. Arrow-key navigation: handle Up/Down/Left/Right in handleKeyDown by
//     incrementing/decrementing _selectedCandidateIndex and calling
//     updateComposition(), instead of forwarding to candidatesWindow.
//
//  5. Implement CandidateWindowDelegate:
//       func candidateWindow(_ window: CandidateWindow, didSelectCandidateAt index: Int) {
//           _selectedCandidateIndex = UInt(index)
//           _composedString = _ritiSuggestion!.preEditText(at: _selectedCandidateIndex)
//           commitComposition(client())
//           writeTextToClient(downcastSender(client()), " ")
//       }

import AppKit

// MARK: - Delegate

protocol CandidateWindowDelegate: AnyObject {
    /// Called when the user clicks a candidate in the panel.
    func candidateWindow(_ window: CandidateWindow, didSelectCandidateAt index: Int)
}

// MARK: - CandidateWindow

final class CandidateWindow: NSPanel {

    // MARK: Subviews
    private let back    = NSVisualEffectView()
    private let content = CandidateContentView()

    // MARK: State
    weak var candidateDelegate: CandidateWindowDelegate?
    private var cursorRect: NSRect = .zero

    /// Currently hovered candidate index — drives the hover highlight.
    /// Setting to a new value automatically triggers a redraw.
    private var hoverIndex: Int? {
        didSet {
            guard hoverIndex != oldValue else { return }
            content.hoverIndex = hoverIndex
        }
    }

    // MARK: Layout constants
    private enum Style {
        /// Corner radius for the panel background.
        static let cornerRadius:      CGFloat = 10
        /// Horizontal space between the panel edge and the text content.
        static let horizontalPadding: CGFloat = 14
        /// Vertical space between the panel edge and the text content.
        static let verticalPadding:   CGFloat = 10
        /// Gap between the bottom of the cursor rect and the top of the panel.
        static let cursorGap:         CGFloat = 4
        /// Panel will never be narrower than this.
        static let minWidth:          CGFloat = 120
    }

    // MARK: Init

    init() {
        super.init(
            contentRect: .zero,
            styleMask:  [.nonactivatingPanel, .borderless],
            backing:    .buffered,
            defer:       true
        )

        // CGShieldingWindowLevel() is one level above kCGMainMenuWindowLevel,
        // ensuring the panel appears over full-screen apps and the Dock.
        level           = NSWindow.Level(Int(CGShieldingWindowLevel()))
        isOpaque        = false
        backgroundColor = .clear
        hasShadow       = true

        // Appear on all Spaces, stay put when Spaces animates, stay out of
        // Cmd+Tab, and appear on top in full-screen mode.
        collectionBehavior = [
            .canJoinAllSpaces,
            .stationary,
            .ignoresCycle,
            .fullScreenAuxiliary,
        ]

        back.blendingMode = .behindWindow
        back.material     = .windowBackground   // adapts to light / dark mode
        back.state        = .active
        back.wantsLayer   = true

        let root = NSView()
        root.wantsLayer = true
        root.addSubview(back)
        root.addSubview(content)
        self.contentView = root

        content.onSelect = { [weak self] idx in
            guard let self else { return }
            self.candidateDelegate?.candidateWindow(self, didSelectCandidateAt: idx)
        }
    }

    // MARK: - Public API

    /// Refresh the panel with new preedit, candidates, and cursor position,
    /// then show it.  Call on the main thread only.
    ///
    /// - Parameters:
    ///   - preedit:          Raw Latin input string shown above the candidates.
    ///   - candidates:       Ordered array of Bengali candidate strings.
    ///   - highlightedIndex: Index of the currently selected candidate.
    ///   - cursorRect:       Cursor rect in *screen* coordinates, as returned by
    ///                       `client.firstRect(forCharacterRange:actualRange:)`.
    func update(
        preedit:          String,
        candidates:       [String],
        highlightedIndex: Int,
        cursorRect:       NSRect
    ) {
        self.cursorRect = cursorRect
        guard !candidates.isEmpty || !preedit.isEmpty else { hide(); return }

        content.preedit          = preedit
        content.candidates       = candidates
        content.highlightedIndex = highlightedIndex
        content.hoverIndex       = hoverIndex
        content.needsDisplay     = true

        layoutAndShow()
    }

    /// Hide and reset the panel.
    func hide() {
        orderOut(nil)
        hoverIndex         = nil
        content.hoverIndex = nil
    }

    // MARK: - Mouse event routing
    //
    // Routing through sendEvent: (rather than NSTrackingArea callbacks on the
    // subview) is the Squirrel pattern — it centralises all panel-level mouse
    // logic and avoids responder-chain ambiguities in a non-activating panel.

    override func sendEvent(_ event: NSEvent) {
        switch event.type {
        case .mouseEntered:
            // Enable per-event mouse tracking only while the cursor is inside.
            acceptsMouseMovedEvents = true

        case .mouseExited:
            // Disable it again immediately to avoid global overhead.
            acceptsMouseMovedEvents = false
            hoverIndex = nil

        case .mouseMoved:
            let screenPt  = NSEvent.mouseLocation
            let windowPt  = convertPoint(fromScreen: screenPt)
            let contentPt = content.convert(windowPt, from: nil)
            hoverIndex = content.candidateIndex(at: contentPt)

        case .leftMouseUp:
            let screenPt  = NSEvent.mouseLocation
            let windowPt  = convertPoint(fromScreen: screenPt)
            let contentPt = content.convert(windowPt, from: nil)
            if let idx = content.candidateIndex(at: contentPt) {
                candidateDelegate?.candidateWindow(self, didSelectCandidateAt: idx)
            }

        default:
            break
        }
        super.sendEvent(event)
    }
}

// MARK: - Private layout & positioning

private extension CandidateWindow {

    func layoutAndShow() {
        // Sync appearance — picks up light/dark mode and accent colour changes.
        appearance = NSApp.effectiveAppearance

        // 1. Ask the content view for its tight bounding size (no padding).
        let cs     = content.measure()
        let panelW = max(cs.width  + Style.horizontalPadding * 2, Style.minWidth)
        let panelH =     cs.height + Style.verticalPadding   * 2

        // 2. Find which screen the cursor is on.
        let screen = NSScreen.screens.first(where: { $0.frame.contains(cursorRect.origin) })
                  ?? NSScreen.main
                  ?? NSScreen.screens[0]
        let sf = screen.visibleFrame   // excludes Dock and menu bar

        // 3. Preferred placement: just below the cursor line.
        //    In macOS screen coordinates Y grows upward.  The cursor rect
        //    returned by firstRect(forCharacterRange:) has its origin at the
        //    bottom-left of the text line.  Place the panel so its top edge
        //    sits just below that line.
        var ox = cursorRect.minX
        var oy = cursorRect.minY - Style.cursorGap - panelH

        // Not enough space below ─ flip the panel above the cursor line.
        if oy < sf.minY {
            oy = cursorRect.maxY + Style.cursorGap
        }

        // 4. Clamp all four edges so the panel never leaves the visible screen.
        //    Two-step clamp (max then min) handles the rare case where the panel
        //    is wider/taller than the screen itself.
        ox = min(ox, sf.maxX - panelW);  ox = max(ox, sf.minX)
        oy = min(oy, sf.maxY - panelH);  oy = max(oy, sf.minY)

        let panelRect = NSRect(x: ox, y: oy, width: panelW, height: panelH)
        setFrame(panelRect, display: false)

        let bounds = NSRect(origin: .zero, size: panelRect.size)

        // 5. Visual effect background fills the entire panel.
        back.frame = bounds

        // 6. Clip the vibrancy view to rounded corners via a CAShapeLayer mask.
        //    Setting `cornerRadius` on the layer would leave the blur unclipped
        //    at the edges in some macOS versions; the mask approach is reliable.
        let mask = CAShapeLayer()
        mask.path = CGPath(
            roundedRect:  bounds,
            cornerWidth:  Style.cornerRadius,
            cornerHeight: Style.cornerRadius,
            transform:    nil
        )
        back.layer?.mask = mask

        // 7. Content view fills the full panel bounds; it draws with internal padding.
        content.frame        = bounds
        content.needsDisplay = true

        // 8. Mouse tracking area covers the full panel.
        if let root = contentView {
            root.trackingAreas.forEach { root.removeTrackingArea($0) }
            root.addTrackingArea(NSTrackingArea(
                rect:     bounds,
                options:  [.mouseEnteredAndExited, .mouseMoved, .activeAlways],
                owner:    self,
                userInfo: nil
            ))
        }

        // 9. invalidateShadow() must be called after setFrame so the shadow
        //    bounding box is recomputed for the new panel size.
        invalidateShadow()
        orderFront(nil)
    }
}

// MARK: - CandidateContentView

/// Custom NSView that draws preedit + divider + horizontal candidate row.
///
/// This view is sized to fill the full panel bounds; all padding is applied
/// internally so the divider can span edge-to-edge.
private final class CandidateContentView: NSView {

    // MARK: State — every setter triggers a redraw via needsDisplay

    var preedit:          String   = ""  { didSet { needsDisplay = true } }
    var candidates:       [String] = []  { didSet { needsDisplay = true } }
    var highlightedIndex: Int      = 0   { didSet { needsDisplay = true } }
    var hoverIndex:       Int?           { didSet { needsDisplay = true } }

    /// Called when a candidate is clicked.
    var onSelect: ((Int) -> Void)?

    // Layout cache — populated during draw(_:), read in candidateIndex(at:).
    // Access is always on the main thread so no synchronisation is needed.
    private var candidateRects: [NSRect] = []

    // MARK: Design tokens

    private enum Style {
        // These must match CandidateWindow.Style.horizontalPadding / verticalPadding.
        static let hPad: CGFloat = 14
        static let vPad: CGFloat = 10

        // Preedit: monospaced so that raw Latin input is visually distinct.
        static let preeditFont   = NSFont.monospacedSystemFont(ofSize: 13, weight: .regular)
        // Candidates: system font — Core Text's font cascade list handles Bengali
        // (and other complex scripts) automatically through font fallback.
        static let candidateFont = NSFont.systemFont(ofSize: 16)

        // Horizontal gap between adjacent candidates.
        static let candidateGap: CGFloat  = 20
        // Retina-crisp half-point divider line.
        static let dividerH: CGFloat      = 0.5
        // Space above and below the divider.
        static let dividerVGap: CGFloat   = 7
    }

    // MARK: Appearance helpers (recomputed on every draw to catch mode changes)

    private var isDark: Bool {
        effectiveAppearance.bestMatch(from: [.aqua, .darkAqua]) == .darkAqua
    }
    private var preeditColor: NSColor {
        isDark ? NSColor(white: 1, alpha: 0.45) : NSColor(white: 0, alpha: 0.40)
    }
    private var defaultColor: NSColor {
        isDark ? .white : .black
    }
    private var highlightColor: NSColor { .controlAccentColor }
    private var hoverColor: NSColor {
        isDark ? NSColor(white: 1, alpha: 0.70) : NSColor(white: 0, alpha: 0.55)
    }
    private var dividerColor: NSColor {
        isDark ? NSColor(white: 1, alpha: 0.10) : NSColor(white: 0, alpha: 0.08)
    }

    // MARK: NSView overrides

    // isFlipped = true: y=0 is at the top, which is consistent with AppKit text
    // layout and avoids coordinate inversions throughout the drawing code.
    override var isFlipped: Bool { true }

    // MARK: - Sizing

    /// Returns the tight content size — width and height of the text, without
    /// the outer padding.  The panel adds Style.hPad*2 / Style.vPad*2.
    func measure() -> NSSize {
        var w: CGFloat = 0
        var h: CGFloat = 0

        if !preedit.isEmpty {
            // Use black for measurement; colour does not affect text metrics.
            let s = makeAttr(preedit, font: Style.preeditFont, color: .black).size()
            h += ceil(s.height) + Style.dividerVGap * 2 + Style.dividerH
            w  = max(w, ceil(s.width))
        }

        if !candidates.isEmpty {
            var rowW: CGFloat = 0
            var rowH: CGFloat = 0
            for (i, c) in candidates.enumerated() {
                let s = makeAttr(c, font: Style.candidateFont, color: .black).size()
                rowW += ceil(s.width)
                if i < candidates.count - 1 { rowW += Style.candidateGap }
                rowH = max(rowH, ceil(s.height))
            }
            h += rowH
            w  = max(w, rowW)
        }

        return NSSize(width: w, height: h)
    }

    // MARK: - Drawing

    override func draw(_ dirtyRect: NSRect) {
        candidateRects = []

        var y = Style.vPad   // current top-of-line y (isFlipped, so positive = downward)

        // ── Preedit row ───────────────────────────────────────────────────
        if !preedit.isEmpty {
            let a  = makeAttr(preedit, font: Style.preeditFont, color: preeditColor)
            let h  = ceil(a.size().height)
            a.draw(at: NSPoint(x: Style.hPad, y: y))
            y += h

            // Divider spans the full panel width (x: 0 to bounds.width) so it
            // appears edge-to-edge despite the content being horizontally inset.
            let divY = y + Style.dividerVGap
            dividerColor.setFill()
            NSRect(x: 0, y: divY, width: bounds.width, height: Style.dividerH).fill()
            y = divY + Style.dividerH + Style.dividerVGap
        }

        // ── Candidate row ─────────────────────────────────────────────────
        var x = Style.hPad
        for (i, candidate) in candidates.enumerated() {
            let color: NSColor
            if      i == highlightedIndex { color = highlightColor }
            else if i == hoverIndex       { color = hoverColor      }
            else                          { color = defaultColor    }

            let a = makeAttr(candidate, font: Style.candidateFont, color: color)
            let w = ceil(a.size().width)
            let h = ceil(a.size().height)

            a.draw(at: NSPoint(x: x, y: y))

            // Inflate hit rect slightly vertically so narrow glyphs are easier
            // to click; horizontal bounds are exact.
            candidateRects.append(NSRect(x: x, y: y - 3, width: w, height: h + 6))
            x += w + Style.candidateGap
        }
    }

    // MARK: - Hit testing

    /// Returns the index of the candidate that contains `point`
    /// (in view coordinates), or `nil` if the point is in a gap.
    ///
    /// The hit area is inflated ±6 pt horizontally to make narrow glyphs
    /// (e.g. a lone "i") easier to click.
    func candidateIndex(at point: NSPoint) -> Int? {
        for (i, rect) in candidateRects.enumerated() {
            if rect.insetBy(dx: -6, dy: 0).contains(point) {
                return i
            }
        }
        return nil
    }

    // MARK: - Mouse

    override func mouseUp(with event: NSEvent) {
        let pt = convert(event.locationInWindow, from: nil)
        if let idx = candidateIndex(at: pt) {
            onSelect?(idx)
        }
    }

    // MARK: - Attributed string factory

    private func makeAttr(_ text: String, font: NSFont, color: NSColor) -> NSAttributedString {
        NSAttributedString(string: text, attributes: [
            .font:            font,
            .foregroundColor: color,
        ])
    }
}
