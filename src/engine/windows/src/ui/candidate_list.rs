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
            Gdi::{
                BeginPaint, EndPaint, GetDC, GetDeviceCaps, HDC, InvalidateRect, LOGPIXELSY,
                PAINTSTRUCT, ReleaseDC,
            },
        },
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

// Vertical offset adjustment for English text to align with Bangla baseline
const ENGLISH_Y_OFFSET: f32 = -3.0;

/// Check if text is ASCII (English/Latin)
fn is_ascii_text(text: &str) -> bool {
    text.chars().all(|c| c.is_ascii())
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
    vertical: bool,
}

pub struct CandidateList {
    window: HWND,
    index_suffix: &'static str,
    font_size: f32,
    index_font_size: f32,
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
            let dc: HDC = GetDC(window);
            let pixel_per_inch = GetDeviceCaps(dc, LOGPIXELSY);
            let dpi_scale = pixel_per_inch as f32 / 96.0;

            // DirectWrite uses DIPs (device independent pixels), convert from points
            let font_size = FONT_SIZE as f32 * dpi_scale;
            let index_font_size = font_size * 0.7;

            let index_suffix = CANDI_INDEX_SUFFIX;
            ReleaseDC(window, dc);
            Ok(CandidateList {
                window,
                index_suffix,
                font_size,
                index_font_size,
                state: RwLock::new(HighlightState {
                    highlighted_index: 0,
                    candidate_count: 0,
                    candidates: Vec::new(),
                    vertical: false,
                }),
            })
        }
    }

    pub fn locate(&self, x: i32, y: i32) -> Result<()> {
        trace!("locate({x}, {y})");
        unsafe {
            SetWindowPos(
                self.window,
                HWND_TOPMOST,
                x + POS_OFFSETX,
                y + POS_OFFSETY,
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

    pub fn show(&self, suggs: &[String]) -> Result<()> {
        // Reset highlight to first candidate and store candidates
        {
            let mut state = self.state.write().unwrap();
            state.highlighted_index = 0;
            state.candidate_count = suggs.len().min(CANDI_NUM);
            state.candidates = suggs.iter().take(CANDI_NUM).cloned().collect();
        }

        self.repaint(true)
    }

    /// Internal method to rebuild PaintArg and trigger repaint
    fn repaint(&self, resize: bool) -> Result<()> {
        unsafe {
            // Copy data out of state and release lock early
            let (highlighted_index, suggs, vertical) = {
                let state = self.state.read().unwrap();
                if state.candidates.is_empty() {
                    return Ok(());
                }
                (state.highlighted_index, state.candidates.clone(), state.vertical)
            };

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
                        self.font_size,
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
                        self.index_font_size,
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

            let row_height = max_candi_height.max(index_height);
            let label_height = LABEL_PADDING_TOP as f32 + row_height + LABEL_PADDING_BOTTOM as f32;

            let mut wnd_height: f32 = 0.0;
            let mut wnd_width: f32 = 0.0;

            if vertical {
                let candi_num = suggs.len().min(CANDI_NUM) as f32;
                wnd_height += candi_num * label_height;
                let max_candi_width = candi_widths.iter().cloned().fold(0.0f32, f32::max);
                wnd_width += CLIP_WIDTH as f32
                    + LABEL_PADDING_LEFT as f32
                    + index_width
                    + INDEX_CANDI_GAP as f32
                    + max_candi_width
                    + LABEL_PADDING_RIGHT as f32;
            } else {
                wnd_height += label_height;
                wnd_width += CLIP_WIDTH as f32;
                for candi_width in candi_widths.iter() {
                    wnd_width += LABEL_PADDING_LEFT as f32 + LABEL_PADDING_RIGHT as f32;
                    wnd_width += index_width;
                    wnd_width += INDEX_CANDI_GAP as f32;
                    wnd_width += candi_width;
                }
            }
            wnd_height += (BORDER_WIDTH * 2) as f32;
            wnd_width += (BORDER_WIDTH * 2) as f32;

            // Calculate highlight width based on the highlighted candidate
            let highlight_width = if vertical {
                wnd_width - CLIP_WIDTH as f32 - (BORDER_WIDTH * 2) as f32
            } else {
                LABEL_PADDING_LEFT as f32
                    + index_width
                    + INDEX_CANDI_GAP as f32
                    + candi_widths[highlighted_index]
                    + LABEL_PADDING_RIGHT as f32
            };

            let arg = PaintArg {
                highlight_width,
                label_height,
                row_height,
                index_width,
                candi_widths,
                candis: candis_str,
                indice: indice_str,
                font_size: self.font_size,
                index_font_size: self.index_font_size,
                font_name: FONT_NAME.to_owned(),
                highlighted_index,
                vertical,
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
    font_size: f32,
    index_font_size: f32,
    font_name: String,
    highlighted_index: usize,
    vertical: bool,
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

    unsafe {
        rt.BeginDraw();

        // Clear with background color
        rt.Clear(Some(&BACKGROUND_COLOR));

        // Calculate highlight position based on highlighted_index
        let highlight_x: f32;
        let highlight_y: f32;

        if arg.vertical {
            highlight_x = (BORDER_WIDTH + CLIP_WIDTH) as f32;
            highlight_y = BORDER_WIDTH as f32 + (arg.highlighted_index as f32 * arg.label_height);
        } else {
            // Calculate x position by summing widths of previous candidates
            let mut x = (BORDER_WIDTH + CLIP_WIDTH) as f32;
            for i in 0..arg.highlighted_index {
                x += LABEL_PADDING_LEFT as f32
                    + arg.index_width
                    + INDEX_CANDI_GAP as f32
                    + arg.candi_widths[i]
                    + LABEL_PADDING_RIGHT as f32;
            }
            highlight_x = x;
            highlight_y = BORDER_WIDTH as f32;
        }

        // Draw clip (always at top-left, next to highlighted item in vertical mode)
        if let Ok(clip_brush) = rt.CreateSolidColorBrush(&CLIP_COLOR, None) {
            let clip_y = if arg.vertical {
                highlight_y
            } else {
                BORDER_WIDTH as f32
            };
            rt.FillRectangle(
                &D2D_RECT_F {
                    left: BORDER_WIDTH as f32,
                    top: clip_y,
                    right: (BORDER_WIDTH + CLIP_WIDTH) as f32,
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
        let mut index_x = (BORDER_WIDTH + CLIP_WIDTH + LABEL_PADDING_LEFT) as f32;
        let mut candi_x = index_x + arg.index_width + INDEX_CANDI_GAP as f32;
        let mut text_y = BORDER_WIDTH as f32 + LABEL_PADDING_TOP as f32;

        // Draw all items, using highlighted color for the selected one
        for i in 0..arg.candis.len() {
            if i > 0 {
                if arg.vertical {
                    text_y += arg.label_height;
                } else {
                    index_x += arg.index_width
                        + INDEX_CANDI_GAP as f32
                        + arg.candi_widths[i - 1]
                        + LABEL_PADDING_LEFT as f32
                        + LABEL_PADDING_RIGHT as f32;
                    candi_x = index_x + arg.index_width + INDEX_CANDI_GAP as f32;
                }
            }

            let candi_y_adjust = if is_ascii_text(&arg.candis[i]) {
                ENGLISH_Y_OFFSET
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
                arg.index_width + 10.0, // Add horizontal padding
                arg.row_height,
                &index_brush,
            );
            draw_text_with_color_emoji(
                &rt,
                &arg.candis[i],
                &candi_format,
                candi_x,
                text_y + candi_y_adjust,
                arg.candi_widths[i] + 10.0,
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
