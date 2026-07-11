use std::mem::{ManuallyDrop, size_of};
use std::sync::RwLock;

use log::{debug, error, trace};
use windows::Win32::Foundation::COLORREF;
use windows::Win32::Graphics::Direct2D::Common::D2D1_COLOR_F;
use windows::Win32::Graphics::Gdi::CreateSolidBrush;
use windows::{
    Win32::{
        Foundation::{BOOL, GetLastError, HWND, LPARAM, LRESULT, RECT, WPARAM},
        Graphics::{
            Direct2D::{
                Common::{D2D_RECT_F, D2D1_ALPHA_MODE_PREMULTIPLIED, D2D1_PIXEL_FORMAT},
                D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT, D2D1_FACTORY_TYPE_SINGLE_THREADED,
                D2D1_HWND_RENDER_TARGET_PROPERTIES, D2D1_PRESENT_OPTIONS_NONE,
                D2D1_RENDER_TARGET_PROPERTIES, D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1CreateFactory,
                ID2D1Factory, ID2D1HwndRenderTarget, ID2D1SolidColorBrush,
            },
            DirectWrite::{
                DWRITE_FACTORY_TYPE_SHARED, DWRITE_FONT_STRETCH_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_WEIGHT_NORMAL, DWRITE_MEASURING_MODE_NATURAL,
                DWRITE_PARAGRAPH_ALIGNMENT_CENTER, DWRITE_TEXT_ALIGNMENT_LEADING,
                DWRITE_TEXT_METRICS, DWriteCreateFactory, IDWriteFactory, IDWriteTextFormat,
                IDWriteTextLayout,
            },
            Dxgi::Common::DXGI_FORMAT_B8G8R8A8_UNORM,
            Gdi::{BeginPaint, EndPaint, HDC, InvalidateRect, PAINTSTRUCT},
        },
        UI::HiDpi::GetDpiForWindow,
        UI::WindowsAndMessaging::{
            CS_DROPSHADOW, CS_HREDRAW, CS_IME, CS_VREDRAW, CreateWindowExA, DefWindowProcA,
            DestroyWindow, GetClientRect, GetWindowLongPtrA, HICON, HWND_TOPMOST, IDC_ARROW,
            LoadCursorW, RegisterClassExA, SW_HIDE, SW_SHOWNOACTIVATE, SWP_NOACTIVATE, SWP_NOMOVE,
            SWP_NOSIZE, SetWindowLongPtrA, SetWindowPos, ShowWindow, WINDOW_LONG_PTR_INDEX,
            WM_ERASEBKGND, WM_PAINT, WNDCLASSEXA, WS_EX_NOACTIVATE, WS_EX_TOOLWINDOW,
            WS_EX_TOPMOST, WS_POPUP,
        },
    },
    core::{PCSTR, Result, s, w},
};

use crate::{
    CANDI_INDEX_SUFFIX, CANDI_INDEXES,
    global::{self, CANDI_NUM},
};

const WINDOW_CLASS: PCSTR = s!("CANDIDATE_LIST");
// Layout
const CLIP_WIDTH: i32 = 3;
const LABEL_PADDING_TOP: i32 = 4;
const LABEL_PADDING_BOTTOM: i32 = 4;
const LABEL_PADDING_LEFT: i32 = 5;
const LABEL_PADDING_RIGHT: i32 = 6;
const INDEX_CANDI_GAP: i32 = 6;
const BORDER_WIDTH: i32 = 0;

// Auxiliary (preedit) text row shown above the candidates.
const AUX_PADDING_TOP: i32 = 3;
const AUX_PADDING_BOTTOM: i32 = 3;
const SEPARATOR_HEIGHT: i32 = 1;

const POS_OFFSETX: i32 = 2;
const POS_OFFSETY: i32 = 2;

const FONT_NAME: &str = "Kalpurush";
const FONT_SIZE: i32 = 20;

const CLIP_COLOR: D2D1_COLOR_F = D2D1_COLOR_F {
    r: 0.0,
    g: 0.47058824,
    b: 0.84313726,
    a: 1.0,
}; // #0078D7
const BACKGROUND_COLOR: D2D1_COLOR_F = D2D1_COLOR_F {
    r: 0.98039216,
    g: 0.98039216,
    b: 0.98039216,
    a: 1.0,
}; // #FAFAFA
const HIGHLIGHT_COLOR: D2D1_COLOR_F = D2D1_COLOR_F {
    r: 0.9098039,
    g: 0.9098039,
    b: 1.0,
    a: 1.0,
}; // #E8E8FF
const INDEX_COLOR: D2D1_COLOR_F = D2D1_COLOR_F {
    r: 0.627451,
    g: 0.627451,
    b: 0.627451,
    a: 1.0,
}; // #A0A0A0
const CANDIDATE_COLOR: D2D1_COLOR_F = D2D1_COLOR_F {
    r: 0.0,
    g: 0.0,
    b: 0.0,
    a: 1.0,
}; // black
const HIGHLIGHTED_COLOR: D2D1_COLOR_F = D2D1_COLOR_F {
    r: 0.0,
    g: 0.0,
    b: 0.0,
    a: 1.0,
}; // black
const SEPARATOR_COLOR: D2D1_COLOR_F = D2D1_COLOR_F {
    r: 0.8784314,
    g: 0.8784314,
    b: 0.8784314,
    a: 1.0,
}; // #E0E0E0

// Vertical offset adjustment for English text to align with Bangla baseline
const ENGLISH_Y_OFFSET: f32 = -3.0;

/// Check if text is ASCII (English/Latin)
fn is_ascii_text(text: &str) -> bool {
    text.chars().all(|c| c.is_ascii())
}

/// Query the DPI scale (1.0 == 96 DPI) for the monitor the window is on.
/// Uses per-monitor DPI so the popup scales correctly on high-DPI displays and
/// adapts when the caret moves across monitors. Falls back to 1.0 if the query
/// fails (`GetDpiForWindow` returns 0 for an invalid window).
fn dpi_scale_for(window: HWND) -> f32 {
    let dpi = unsafe { GetDpiForWindow(window) };
    if dpi == 0 { 1.0 } else { dpi as f32 / 96.0 }
}

/// Layout constants scaled to physical pixels for the current DPI. The render
/// target is pinned to 96 DPI (1 DIP == 1 px), so every layout value must be
/// pre-scaled here; both the sizing path (`repaint`) and the drawing path
/// (`paint`) build this from the same `dpi_scale` to stay consistent.
struct ScaledLayout {
    clip_width: f32,
    label_padding_top: f32,
    label_padding_bottom: f32,
    label_padding_left: f32,
    label_padding_right: f32,
    index_candi_gap: f32,
    border_width: f32,
    english_y_offset: f32,
    aux_padding_top: f32,
    aux_padding_bottom: f32,
    separator_height: f32,
}

impl ScaledLayout {
    fn new(dpi_scale: f32) -> Self {
        Self {
            clip_width: CLIP_WIDTH as f32 * dpi_scale,
            label_padding_top: LABEL_PADDING_TOP as f32 * dpi_scale,
            label_padding_bottom: LABEL_PADDING_BOTTOM as f32 * dpi_scale,
            label_padding_left: LABEL_PADDING_LEFT as f32 * dpi_scale,
            label_padding_right: LABEL_PADDING_RIGHT as f32 * dpi_scale,
            index_candi_gap: INDEX_CANDI_GAP as f32 * dpi_scale,
            border_width: BORDER_WIDTH as f32 * dpi_scale,
            english_y_offset: ENGLISH_Y_OFFSET * dpi_scale,
            aux_padding_top: AUX_PADDING_TOP as f32 * dpi_scale,
            aux_padding_bottom: AUX_PADDING_BOTTOM as f32 * dpi_scale,
            separator_height: SEPARATOR_HEIGHT as f32 * dpi_scale,
        }
    }
}

/// Total height of the auxiliary-text section (aux row padding + text + separator)
/// that sits above the candidates. Returns 0.0 when there is no aux text, so the
/// candidate area falls back to its original position. This is the single source
/// of truth used by both the sizing path (`repaint`) and the drawing path
/// (`paint`) for the candidates' top offset.
fn aux_section_height(layout: &ScaledLayout, aux_row_height: f32) -> f32 {
    if aux_row_height <= 0.0 {
        0.0
    } else {
        layout.aux_padding_top + aux_row_height + layout.aux_padding_bottom + layout.separator_height
    }
}

#[cfg(target_pointer_width = "64")]
type LongPointer = isize;
#[cfg(target_pointer_width = "32")]
type LongPointer = i32;

// Thread-local storage for Direct2D/DirectWrite factories
thread_local! {
    static D2D_FACTORY: ID2D1Factory = unsafe {
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, None).unwrap()
    };
    static DW_FACTORY: IDWriteFactory = unsafe {
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED).unwrap()
    };
}

/// To create a window you need to register the window class beforehand.
pub fn setup() -> Result<()> {
    let wcex = WNDCLASSEXA {
        cbSize: size_of::<WNDCLASSEXA>() as u32,
        style: CS_IME | CS_HREDRAW | CS_VREDRAW | CS_DROPSHADOW,
        lpfnWndProc: Some(wind_proc),
        cbClsExtra: 0,
        cbWndExtra: size_of::<Box<PaintArg>>().try_into().unwrap(),
        hInstance: global::dll_module(),
        hIcon: HICON::default(),
        hCursor: unsafe { LoadCursorW(None, IDC_ARROW)? },
        hbrBackground: unsafe { CreateSolidBrush(COLORREF(0)) },
        lpszMenuName: PCSTR::null(),
        lpszClassName: WINDOW_CLASS,
        hIconSm: HICON::default(),
    };
    unsafe {
        if RegisterClassExA(&wcex) == 0 {
            error!("Failed to register window class for candidate list");
            return Err(GetLastError().into());
        }
        debug!("Registered window class for candidate list.");
    }
    Ok(())
}

/// use default handlers for everything but repaint
unsafe extern "system" fn wind_proc(
    window: HWND,
    msg: u32,
    wparam: WPARAM,
    lparam: LPARAM,
) -> LRESULT {
    match msg {
        WM_ERASEBKGND => LRESULT(1), // Prevent background erase to avoid flickering
        WM_PAINT => paint(window),
        _ => unsafe { DefWindowProcA(window, msg, wparam, lparam) },
    }
}

//----------------------------------------------------------------------------
//
//  Helper function to measure text with DirectWrite
//
//----------------------------------------------------------------------------

fn measure_text_dwrite(
    factory: &IDWriteFactory,
    text: &str,
    format: &IDWriteTextFormat,
) -> (f32, f32) {
    unsafe {
        let text_wide: Vec<u16> = text.encode_utf16().collect();
        let layout: std::result::Result<IDWriteTextLayout, _> = factory.CreateTextLayout(
            &text_wide, format, 10000.0, // max width
            10000.0, // max height
        );

        if let Ok(layout) = layout {
            let mut metrics = DWRITE_TEXT_METRICS::default();
            if layout.GetMetrics(&mut metrics).is_ok() {
                return (metrics.width, metrics.height);
            }
        }
        (0.0, 0.0)
    }
}

//----------------------------------------------------------------------------
//
//  The implementation
//
//----------------------------------------------------------------------------

/// Interior mutable state for highlight tracking
struct HighlightState {
    highlighted_index: usize,
    candidate_count: usize,
    candidates: Vec<String>,
    aux_text: String,
    vertical: bool,
}

pub struct CandidateList {
    window: HWND,
    index_suffix: &'static str,
    state: RwLock<HighlightState>,
}

impl CandidateList {
    pub fn create(_parent_window: HWND) -> Result<CandidateList> {
        // WS_EX_TOOLWINDOW:  A floating toolbar that won't appear in taskbar and ALT+TAB.
        // WS_EX_NOACTIVATE:  A window that doesn't take the foreground thus not making parent window lose focus.
        // WS_EX_TOPMOST:     A window that is topmost.
        // WS_POPUP:          A window having no top bar or border.
        // see: https://learn.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
        unsafe {
            let window = CreateWindowExA(
                WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST,
                WINDOW_CLASS,
                PCSTR::null(),
                WS_POPUP,
                0,
                0,
                0,
                0,
                None,
                None,
                global::dll_module(),
                None,
            );
            if window.0 == 0 {
                error!("CreateWindowExA returned null.");
                return Err(GetLastError().into());
            }
            // DPI is queried per-repaint (see `repaint`) so the popup adapts to
            // the monitor it is currently shown on.
            let index_suffix = CANDI_INDEX_SUFFIX;
            Ok(CandidateList {
                window,
                index_suffix,
                state: RwLock::new(HighlightState {
                    highlighted_index: 0,
                    candidate_count: 0,
                    candidates: Vec::new(),
                    aux_text: String::new(),
                    vertical: false,
                }),
            })
        }
    }

    pub fn locate(&self, x: i32, y: i32) -> Result<()> {
        trace!("locate({x}, {y})");
        let dpi_scale = dpi_scale_for(self.window);
        let offset_x = (POS_OFFSETX as f32 * dpi_scale).round() as i32;
        let offset_y = (POS_OFFSETY as f32 * dpi_scale).round() as i32;
        unsafe {
            SetWindowPos(
                self.window,
                HWND_TOPMOST,
                x + offset_x,
                y + offset_y,
                0,
                0,
                SWP_NOACTIVATE | SWP_NOSIZE,
            )?
        };
        Ok(())
    }

    /// Move the highlight to the next candidate (right/down), wrapping around to the first.
    pub fn move_highlight_next(&self) {
        let mut state = self.state.write().unwrap();
        if state.candidate_count == 0 {
            return;
        }
        state.highlighted_index = (state.highlighted_index + 1) % state.candidate_count;
        drop(state);
        self.invalidate();
    }

    /// Move the highlight to the previous candidate (left/up), wrapping around to the last.
    pub fn move_highlight_prev(&self) {
        let mut state = self.state.write().unwrap();
        if state.candidate_count == 0 {
            return;
        }
        if state.highlighted_index == 0 {
            state.highlighted_index = state.candidate_count - 1;
        } else {
            state.highlighted_index -= 1;
        }
        drop(state);
        self.invalidate();
    }

    /// Set the highlight to a specific index. Returns false if index is out of bounds.
    pub fn set_highlight(&self, index: usize) -> bool {
        let mut state = self.state.write().unwrap();
        if index >= state.candidate_count {
            return false;
        }
        state.highlighted_index = index;
        drop(state);
        self.invalidate();
        true
    }

    /// Get the currently highlighted index.
    pub fn get_highlighted_index(&self) -> usize {
        self.state.read().unwrap().highlighted_index
    }

    /// Get the total number of candidates currently displayed.
    pub fn get_candidate_count(&self) -> usize {
        self.state.read().unwrap().candidate_count
    }

    /// Reset highlight to the first candidate.
    pub fn reset_highlight(&self) {
        self.state.write().unwrap().highlighted_index = 0;
    }

    /// Set the candidate window orientation.
    /// `true` for vertical (candidates stacked top-to-bottom),
    /// `false` for horizontal (candidates laid out left-to-right).
    pub fn set_vertical(&self, vertical: bool) {
        self.state.write().unwrap().vertical = vertical;
    }

    /// Trigger a repaint of the window with updated highlight.
    fn invalidate(&self) {
        let _ = self.repaint(false);
    }

    pub fn show(&self, aux: &str, suggs: &[String]) -> Result<()> {
        // Reset highlight to first candidate and store candidates + aux text
        {
            let mut state = self.state.write().unwrap();
            state.highlighted_index = 0;
            state.candidate_count = suggs.len().min(CANDI_NUM);
            state.candidates = suggs.iter().take(CANDI_NUM).cloned().collect();
            state.aux_text = aux.to_string();
        }

        self.repaint(true)
    }

    /// Internal method to rebuild PaintArg and trigger repaint
    fn repaint(&self, resize: bool) -> Result<()> {
        unsafe {
            // Copy data out of state and release lock early
            let (highlighted_index, suggs, aux_text, vertical) = {
                let state = self.state.read().unwrap();
                if state.candidates.is_empty() {
                    return Ok(());
                }
                (
                    state.highlighted_index,
                    state.candidates.clone(),
                    state.aux_text.clone(),
                    state.vertical,
                )
            };

            // Query the current monitor's DPI so the popup scales correctly and
            // adapts when the caret moves across monitors. The render target is
            // pinned to 96 DPI (1 DIP == 1 px), so fonts and layout constants are
            // scaled here in physical pixels.
            let dpi_scale = dpi_scale_for(self.window);
            let font_size = FONT_SIZE as f32 * dpi_scale;
            let index_font_size = font_size * 0.7;
            let layout = ScaledLayout::new(dpi_scale);

            // Create DirectWrite text formats for measurement
            let (candi_format, index_format) = DW_FACTORY.with(|factory| {
                let font_name_wide: Vec<u16> =
                    FONT_NAME.encode_utf16().chain(std::iter::once(0)).collect();

                let candi_format = factory
                    .CreateTextFormat(
                        windows::core::PCWSTR(font_name_wide.as_ptr()),
                        None,
                        DWRITE_FONT_WEIGHT_NORMAL,
                        DWRITE_FONT_STYLE_NORMAL,
                        DWRITE_FONT_STRETCH_NORMAL,
                        font_size,
                        w!("en-us"),
                    )
                    .ok();

                let index_format = factory
                    .CreateTextFormat(
                        windows::core::PCWSTR(font_name_wide.as_ptr()),
                        None,
                        DWRITE_FONT_WEIGHT_NORMAL,
                        DWRITE_FONT_STYLE_NORMAL,
                        DWRITE_FONT_STRETCH_NORMAL,
                        index_font_size,
                        w!("en-us"),
                    )
                    .ok();

                (candi_format, index_format)
            });

            let Some(candi_format) = candi_format else {
                error!("Failed to create candidate text format");
                return Ok(());
            };
            let Some(index_format) = index_format else {
                error!("Failed to create index text format");
                return Ok(());
            };

            let mut indice_str = Vec::with_capacity(suggs.len());
            let mut candis_str = Vec::with_capacity(suggs.len());

            let mut max_candi_height: f32 = 0.0;
            let mut index_height: f32 = 0.0;
            let mut index_width: f32 = 0.0;
            let mut candi_widths: Vec<f32> = Vec::with_capacity(suggs.len());

            // Measure text using DirectWrite
            DW_FACTORY.with(|factory| {
                for (index, sugg) in suggs.iter().take(CANDI_NUM).enumerate() {
                    let index_str = format!("{}{}", CANDI_INDEXES[index], self.index_suffix);
                    let (w, h) = measure_text_dwrite(factory, &index_str, &index_format);
                    index_height = index_height.max(h);
                    index_width = index_width.max(w);
                    indice_str.push(index_str);

                    let (w, h) = measure_text_dwrite(factory, sugg, &candi_format);
                    max_candi_height = max_candi_height.max(h);
                    candi_widths.push(w);
                    candis_str.push(sugg.clone());
                }
            });

            // Measure the auxiliary (preedit) text shown above the candidates.
            // A 0.0 height means "no aux row" (empty aux), keeping the original layout.
            let (aux_width, aux_row_height) = if aux_text.is_empty() {
                (0.0, 0.0)
            } else {
                DW_FACTORY.with(|factory| measure_text_dwrite(factory, &aux_text, &candi_format))
            };

            let row_height = max_candi_height.max(index_height);
            let label_height = layout.label_padding_top + row_height + layout.label_padding_bottom;

            let mut wnd_height: f32 = 0.0;
            let mut wnd_width: f32 = 0.0;

            // Reserve room for the aux-text section at the top (0 when no aux).
            wnd_height += aux_section_height(&layout, aux_row_height);

            if vertical {
                let candi_num = suggs.len().min(CANDI_NUM) as f32;
                wnd_height += candi_num * label_height;
                let max_candi_width = candi_widths.iter().cloned().fold(0.0f32, f32::max);
                wnd_width += layout.clip_width
                    + layout.label_padding_left
                    + index_width
                    + layout.index_candi_gap
                    + max_candi_width
                    + layout.label_padding_right;
            } else {
                wnd_height += label_height;
                wnd_width += layout.clip_width;
                for candi_width in candi_widths.iter() {
                    wnd_width += layout.label_padding_left + layout.label_padding_right;
                    wnd_width += index_width;
                    wnd_width += layout.index_candi_gap;
                    wnd_width += candi_width;
                }
            }
            wnd_height += layout.border_width * 2.0;
            wnd_width += layout.border_width * 2.0;

            // Ensure the window is wide enough for the aux text (it aligns under
            // the first index: clip + left padding, with right padding to spare).
            if aux_row_height > 0.0 {
                let aux_total_width = layout.border_width * 2.0
                    + layout.clip_width
                    + layout.label_padding_left
                    + aux_width
                    + layout.label_padding_right;
                wnd_width = wnd_width.max(aux_total_width);
            }

            // Calculate highlight width based on the highlighted candidate
            let highlight_width = if vertical {
                wnd_width - layout.clip_width - layout.border_width * 2.0
            } else {
                layout.label_padding_left
                    + index_width
                    + layout.index_candi_gap
                    + candi_widths[highlighted_index]
                    + layout.label_padding_right
            };

            let arg = PaintArg {
                highlight_width,
                label_height,
                row_height,
                index_width,
                candi_widths,
                candis: candis_str,
                indice: indice_str,
                aux_text,
                aux_width,
                aux_row_height,
                font_size,
                index_font_size,
                font_name: FONT_NAME.to_owned(),
                highlighted_index,
                vertical,
                dpi_scale,
            };
            let long_ptr = arg.into_long_ptr();
            SetWindowLongPtrA(self.window, WINDOW_LONG_PTR_INDEX::default(), long_ptr);

            if resize {
                SetWindowPos(
                    self.window,
                    HWND_TOPMOST,
                    0,
                    0,
                    wnd_width.ceil() as i32,
                    wnd_height.ceil() as i32,
                    SWP_NOACTIVATE | SWP_NOMOVE,
                )?;
                ShowWindow(self.window, SW_SHOWNOACTIVATE);
            }
            InvalidateRect(self.window, None, BOOL::from(false));
        };
        Ok(())
    }

    pub fn hide(&self) {
        unsafe {
            ShowWindow(self.window, SW_HIDE);
        }
    }

    pub fn destroy(&self) -> Result<()> {
        unsafe { DestroyWindow(self.window) }
    }
}

struct PaintArg {
    highlight_width: f32,
    label_height: f32,
    row_height: f32,
    index_width: f32,
    candi_widths: Vec<f32>,
    indice: Vec<String>,
    candis: Vec<String>,
    aux_text: String,
    aux_width: f32,
    aux_row_height: f32,
    font_size: f32,
    index_font_size: f32,
    font_name: String,
    highlighted_index: usize,
    vertical: bool,
    dpi_scale: f32,
}

impl PaintArg {
    fn into_long_ptr(self) -> LongPointer {
        ManuallyDrop::new(Box::new(self)).as_ref() as *const PaintArg as LongPointer
    }

    unsafe fn from_long_ptr(long_ptr: LongPointer) -> Option<Box<PaintArg>> {
        if long_ptr == 0 {
            None
        } else {
            Some(unsafe { Box::from_raw(long_ptr as *mut PaintArg) })
        }
    }
}

fn paint(window: HWND) -> LRESULT {
    let arg = unsafe {
        PaintArg::from_long_ptr(GetWindowLongPtrA(window, WINDOW_LONG_PTR_INDEX::default()))
    };
    let Some(arg) = arg else {
        error!("Args for repaint is not found.");
        return LRESULT::default();
    };
    unsafe { SetWindowLongPtrA(window, WINDOW_LONG_PTR_INDEX::default(), 0) };

    let mut ps = PAINTSTRUCT::default();
    let _dc: HDC = unsafe { BeginPaint(window, &mut ps) };

    // Create Direct2D render target
    let render_target = D2D_FACTORY.with(|factory| unsafe {
        let mut rect = RECT::default();
        let _ = GetClientRect(window, &mut rect);

        let render_props = D2D1_RENDER_TARGET_PROPERTIES {
            r#type: D2D1_RENDER_TARGET_TYPE_DEFAULT,
            pixelFormat: D2D1_PIXEL_FORMAT {
                format: DXGI_FORMAT_B8G8R8A8_UNORM,
                alphaMode: D2D1_ALPHA_MODE_PREMULTIPLIED,
            },
            // Pin to 96 DPI so 1 DIP == 1 physical pixel. Without this Direct2D
            // uses the desktop DPI and scales all drawing up, overflowing the
            // window (which is sized treating DirectWrite DIP metrics as pixels).
            // DPI scaling is applied manually to fonts and layout instead.
            dpiX: 96.0,
            dpiY: 96.0,
            ..Default::default()
        };

        let hwnd_props = D2D1_HWND_RENDER_TARGET_PROPERTIES {
            hwnd: window,
            pixelSize: windows::Win32::Graphics::Direct2D::Common::D2D_SIZE_U {
                width: (rect.right - rect.left) as u32,
                height: (rect.bottom - rect.top) as u32,
            },
            presentOptions: D2D1_PRESENT_OPTIONS_NONE,
        };

        factory.CreateHwndRenderTarget(&render_props, &hwnd_props)
    });

    let Ok(rt) = render_target else {
        error!("Failed to create render target");
        unsafe { EndPaint(window, &ps) };
        return LRESULT::default();
    };

    // Create text formats
    let text_formats = DW_FACTORY.with(|factory| unsafe {
        let font_name_wide: Vec<u16> = arg
            .font_name
            .encode_utf16()
            .chain(std::iter::once(0))
            .collect();

        let candi_format = factory.CreateTextFormat(
            windows::core::PCWSTR(font_name_wide.as_ptr()),
            None,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            arg.font_size,
            w!("en-us"),
        );

        let index_format = factory.CreateTextFormat(
            windows::core::PCWSTR(font_name_wide.as_ptr()),
            None,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            arg.index_font_size,
            w!("en-us"),
        );

        match (candi_format, index_format) {
            (Ok(cf), Ok(inf)) => {
                let _ = cf.SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                let _ = cf.SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                let _ = inf.SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                let _ = inf.SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                Some((cf, inf))
            }
            _ => None,
        }
    });

    let Some((candi_format, index_format)) = text_formats else {
        error!("Failed to create text formats");
        unsafe { EndPaint(window, &ps) };
        return LRESULT::default();
    };

    // Layout constants scaled to physical pixels, matching how the window was
    // sized in `repaint`. Must use the same `dpi_scale` used there.
    let layout = ScaledLayout::new(arg.dpi_scale);

    unsafe {
        rt.BeginDraw();

        // Clear with background color
        rt.Clear(Some(&BACKGROUND_COLOR));

        // Draw the auxiliary (preedit) text section at the top, above the
        // candidates. `content_top` is the y-offset the candidate area shifts
        // down by; it is 0.0 when there is no aux text (original layout).
        let content_top = aux_section_height(&layout, arg.aux_row_height);
        if content_top > 0.0 {
            let aux_pad = 10.0 * arg.dpi_scale;
            if let Ok(aux_brush) = rt.CreateSolidColorBrush(&INDEX_COLOR, None) {
                draw_text_with_color_emoji(
                    &rt,
                    &arg.aux_text,
                    &candi_format,
                    layout.border_width + layout.clip_width + layout.label_padding_left,
                    layout.border_width + layout.aux_padding_top,
                    arg.aux_width + aux_pad,
                    arg.aux_row_height,
                    &aux_brush,
                );
            }
            // Thin separator line at the bottom of the aux section.
            if let Ok(sep_brush) = rt.CreateSolidColorBrush(&SEPARATOR_COLOR, None) {
                let sep_top = content_top - layout.separator_height;
                let mut rect = RECT::default();
                let _ = GetClientRect(window, &mut rect);
                rt.FillRectangle(
                    &D2D_RECT_F {
                        left: layout.border_width,
                        top: sep_top,
                        right: (rect.right - rect.left) as f32 - layout.border_width,
                        bottom: content_top,
                    },
                    &sep_brush,
                );
            }
        }

        // Calculate highlight position based on highlighted_index
        let highlight_x: f32;
        let highlight_y: f32;

        if arg.vertical {
            highlight_x = layout.border_width + layout.clip_width;
            highlight_y = layout.border_width
                + content_top
                + (arg.highlighted_index as f32 * arg.label_height);
        } else {
            // Calculate x position by summing widths of previous candidates
            let mut x = layout.border_width + layout.clip_width;
            for i in 0..arg.highlighted_index {
                x += layout.label_padding_left
                    + arg.index_width
                    + layout.index_candi_gap
                    + arg.candi_widths[i]
                    + layout.label_padding_right;
            }
            highlight_x = x;
            highlight_y = layout.border_width + content_top;
        }

        // Draw clip (always at top-left, next to highlighted item in vertical mode)
        if let Ok(clip_brush) = rt.CreateSolidColorBrush(&CLIP_COLOR, None) {
            let clip_y = if arg.vertical {
                highlight_y
            } else {
                layout.border_width + content_top
            };
            rt.FillRectangle(
                &D2D_RECT_F {
                    left: layout.border_width,
                    top: clip_y,
                    right: layout.border_width + layout.clip_width,
                    bottom: clip_y + arg.label_height,
                },
                &clip_brush,
            );
        }

        // Draw highlight
        if let Ok(highlight_brush) = rt.CreateSolidColorBrush(&HIGHLIGHT_COLOR, None) {
            rt.FillRectangle(
                &D2D_RECT_F {
                    left: highlight_x,
                    top: highlight_y,
                    right: highlight_x + arg.highlight_width,
                    bottom: highlight_y + arg.label_height,
                },
                &highlight_brush,
            );
        }

        // Create text brushes
        let index_brush = rt.CreateSolidColorBrush(&INDEX_COLOR, None).ok();
        let highlighted_brush = rt.CreateSolidColorBrush(&HIGHLIGHTED_COLOR, None).ok();
        let candidate_brush = rt.CreateSolidColorBrush(&CANDIDATE_COLOR, None).ok();

        if index_brush.is_none() || highlighted_brush.is_none() || candidate_brush.is_none() {
            error!("Failed to create text brushes");
            let _ = rt.EndDraw(None, None);
            EndPaint(window, &ps);
            return LRESULT::default();
        }

        let index_brush = index_brush.unwrap();
        let highlighted_brush = highlighted_brush.unwrap();
        let candidate_brush = candidate_brush.unwrap();

        // Draw text - use row_height for all items and let DirectWrite paragraph alignment handle centering
        let text_pad = 10.0 * arg.dpi_scale;
        let mut index_x = layout.border_width + layout.clip_width + layout.label_padding_left;
        let mut candi_x = index_x + arg.index_width + layout.index_candi_gap;
        let mut text_y = layout.border_width + content_top + layout.label_padding_top;

        // Draw all items, using highlighted color for the selected one
        for i in 0..arg.candis.len() {
            if i > 0 {
                if arg.vertical {
                    text_y += arg.label_height;
                } else {
                    index_x += arg.index_width
                        + layout.index_candi_gap
                        + arg.candi_widths[i - 1]
                        + layout.label_padding_left
                        + layout.label_padding_right;
                    candi_x = index_x + arg.index_width + layout.index_candi_gap;
                }
            }

            let candi_y_adjust = if is_ascii_text(&arg.candis[i]) {
                layout.english_y_offset
            } else {
                0.0
            };

            // Use highlighted brush for the selected candidate, candidate brush for others
            let text_brush = if i == arg.highlighted_index {
                &highlighted_brush
            } else {
                &candidate_brush
            };

            draw_text_with_color_emoji(
                &rt,
                &arg.indice[i],
                &index_format,
                index_x,
                text_y,
                arg.index_width + text_pad, // Add horizontal padding
                arg.row_height,
                &index_brush,
            );
            draw_text_with_color_emoji(
                &rt,
                &arg.candis[i],
                &candi_format,
                candi_x,
                text_y + candi_y_adjust,
                arg.candi_widths[i] + text_pad,
                arg.row_height,
                text_brush,
            );
        }

        let _ = rt.EndDraw(None, None);
    }

    unsafe { EndPaint(window, &ps) };
    LRESULT::default()
}

unsafe fn draw_text_with_color_emoji(
    rt: &ID2D1HwndRenderTarget,
    text: &str,
    format: &IDWriteTextFormat,
    x: f32,
    y: f32,
    width: f32,
    height: f32,
    brush: &ID2D1SolidColorBrush,
) {
    let text_wide: Vec<u16> = text.encode_utf16().collect();
    let rect = D2D_RECT_F {
        left: x,
        top: y,
        right: x + width,
        bottom: y + height,
    };

    // D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT enables color emoji rendering
    unsafe {
        rt.DrawText(
            &text_wide,
            format,
            &rect,
            brush,
            D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT,
            DWRITE_MEASURING_MODE_NATURAL,
        );
    }
}
