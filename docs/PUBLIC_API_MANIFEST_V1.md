# Public API manifest — CASIO GUI v1.0.0

This manifest records the frozen names exposed by the canonical
`CASIO_GUI/casio.hpp` facade.

## Version contract

```cpp
casio::GUI_VERSION_MAJOR == 1
casio::GUI_VERSION_MINOR == 0
casio::GUI_VERSION_PATCH == 0
casio::GUI_VERSION       == "1.0.0"
casio::GUI_API_LEVEL     == 21
casio::GUI_API_FROZEN    == true
casio::GUI_API_STAGE     == "stable"
```

## Public configuration macros

```text
CASIO_GUI_VERSION_MAJOR=1
CASIO_GUI_VERSION_MINOR=0
CASIO_GUI_VERSION_PATCH=0
CASIO_GUI_API_LEVEL=21
CASIO_GUI_API_FROZEN=1

CASIO_GUI_THEME_LIGHT=0
CASIO_GUI_THEME_DARK=1
CASIO_GUI_THEME_HIGH_CONTRAST=2
CASIO_GUI_DEFAULT_THEME=CASIO_GUI_THEME_LIGHT

CASIO_GUI_STORAGE_ROOT="/personalised"
CASIO_GUI_SAFE_STORAGE_MAX_BYTES=131072
CASIO_GUI_SCREENSHOT_ROOT="/Capt"
```

## Facade type aliases

```cpp
using Color = ::Class_color;
using Image = ::bopti_image_t;
using NativeImage = ::image_t;
using ImageLinearMap = struct image_linear_map;
using Font = ::font_t;
using KeyEvent = ::key_event_t;
using Window = struct dwindow;
using Time = ::rtc_time_t;
using GUI = ::GUI;
using Cursor = ::Cursor;
using Theme = ::item_theme;
using ThemePreset = ::item_theme_preset;
using Item = ::item;
using Button = ::item_button;
using ToggleButton = ::item_toggle_button;
using TextBox = ::item_textbox;
using TextBoxInputMode = ::item_textbox_input_mode;
using AlphaMode = ::item_alpha_mode;
using TextInputState = ::item_text_input_state;
using Numeric = ::item_numeric;
using Orientation = ::item_orientation;
using Slider = ::item_slider;
using HorizontalSlider = ::item_horizontal_slider;
using VerticalSlider = ::item_vertical_slider;
using Checkbox = ::item_checkbox;
using CanvasCommandType = ::canvas_command_type;
using CanvasCommand = ::canvas_command;
using Canvas = ::item_canvas;
using DrawingCanvas = ::item_canvas;
using TextAlign = ::item_text_align;
using Label = ::item_text_label;
using Separator = ::item_separator;
using ToolbarAction = ::toolbar_action;
using Toolbar = ::item_toolbar;
using StatusBar = ::item_status_bar;
using LayoutMode = ::item_layout_mode;
using Margins = ::item_margins;
using ContainerChild = ::container_child;
using Container = ::item_container;
using Panel = ::item_panel;
using GroupBox = ::item_group_box;
using ScrollView = ::item_scroll_view;
using TabView = ::item_tab_view;
using TabPages = ::item_tab_pages;
using RadioButton = ::item_radio_button;
using RadioGroup = ::item_radio_group;
using ListBox = ::item_list_box;
using ComboBox = ::item_combo_box;
using DropDown = ::item_combo_box;
using ProgressBar = ::item_progress_bar;
using HorizontalProgressBar = ::item_horizontal_progress_bar;
using VerticalProgressBar = ::item_vertical_progress_bar;
using ScrollBar = ::item_scrollbar;
using HorizontalScrollBar = ::item_horizontal_scrollbar;
using VerticalScrollBar = ::item_vertical_scrollbar;
using LED = ::item_led;
using LEDState = ::item_led_state;
using LEDShape = ::item_led_shape;
using ColorWheel = ::item_color_wheel;
using HueSelector = ::item_hue_selector;
using GaugeZone = ::gauge_zone;
using Gauge = ::item_gauge;
using NeedleMeter = ::item_needle_meter;
using Meter = ::item_meter;
using Dial = ::item_dial;
using Tab = ::item_tab;
using Rectangle = ::item_rectangle;
using Square = ::item_square;
using Circle = ::item_circle;
using Ellipse = ::item_ellipse;
using Triangle = ::item_triangle;
using RightTriangle = ::item_right_triangle;
using Trapezoid = ::item_trapezoid;
using Polygon = ::item_polygon;
using RegularPolygon = ::item_regular_polygon;
using Line = ::item_line;
using Polyline = ::item_polyline;
using Arc = ::item_arc;
using Sector = ::item_sector;
using Pie = ::item_pie;
using RoundedRectangle = ::item_rounded_rectangle;
using Transform2D = ::Transform2D;
using RightTriangleCorner = ::item_right_triangle_corner;
using MenuBar = ::item_menu_bar;
using SoftKeyBar = ::item_softkey_bar;
using BottomMenu = ::item_bottom_menu;
using Graph = ::item_graph;
using GraphAxis = ::graph_axis;
using GraphPoint = ::graph_point;
using GraphSeries = ::graph_series;
using GraphScale = ::graph_scale;
using GraphCursor = ::graph_cursor;
using GraphCursorStyle = ::graph_cursor_style;
using TreeNode = ::tree_node;
using Tree = ::item_tree;
using Table = ::item_table;
using Spreadsheet = ::item_spreadsheet;
using ImageItem = ::item_image;
using ImageButton = ::item_image_button;
using IconButton = ::item_icon_button;
using ImageMode = ::item_image_mode;
using TimeItem = ::item_time;
using TimerItem = ::item_timer;
using Popup = ::item_popup;
using PromptPopup = ::item_prompt_popup;
using BoolPopup = ::item_bool_popup;
using ChoicePopup = ::item_choice_popup;
using NumericPopup = ::item_numeric_popup;
using OptionPopupType = ::option_popup_type;
using OptionPopupEntry = ::option_popup_entry;
using OptionsPopup = ::item_options_popup;
using CheckboxPopup = ::item_checkbox_popup;
using TogglePopup = ::item_toggle_popup;
using Page = ::gui_page;
using Position = ::STRUCT_pos;
using Point = ::STRUCT_point;
using DrawMode = ::item_draw_mode;
using ItemStatus = ::item_status;
using ItemLabel = ::item_label;
using ItemColor = ::itemColor;
using RawItemColor = ::RawitemColor;
using ItemEvent = ::itemEvent;
using EventTable = ::eventTable;
using EventList = ::listEventTable;
using ItemList = ::liste_item;
using IniFile = ::casio_storage::IniFile;
using ConfigFile = ::casio_storage::ConfigFile;
using SaveFile = ::casio_storage::SaveFile;
using DataFile = ::casio_storage::DataFile;
using GuiStateFile = ::casio_storage::GuiStateFile;
using NamedObjectFile = ::casio_storage::NamedObjectFile;
using ObjectFile = ::casio_storage::ObjectFile;
using ObjectSerializer = ::casio_storage::ObjectSerializer;
using NamedSaveFile = ::casio_storage::NamedSaveFile;
```

## Namespace aliases

```cpp
namespace storage = ::casio_storage;
namespace safeStorage = ::casio_storage::safe_file;
namespace Screenshot = ::casio_gui_screenshot;
```

## Selected public constants

```cpp
inline constexpr int GUI_VERSION_MAJOR = ::casio_gui_version::major;
inline constexpr int GUI_VERSION_MINOR = ::casio_gui_version::minor;
inline constexpr int GUI_VERSION_PATCH = ::casio_gui_version::patch;
inline constexpr const char* GUI_VERSION = ::casio_gui_version::string;
inline constexpr int GUI_API_LEVEL = ::casio_gui_version::api_level;
inline constexpr bool GUI_API_FROZEN = ::casio_gui_version::api_frozen;
inline constexpr const char* GUI_API_STAGE = ::casio_gui_version::stage;
inline constexpr int WIDTH  = DWIDTH;
inline constexpr int HEIGHT = DHEIGHT;
inline constexpr int WHITE = C_WHITE;
inline constexpr int LIGHT = C_LIGHT;
inline constexpr int DARK  = C_DARK;
inline constexpr int BLACK = C_BLACK;
inline constexpr int RED   = C_RED;
inline constexpr int GREEN = C_GREEN;
inline constexpr int BLUE  = C_BLUE;
inline constexpr int NONE  = C_NONE;
inline constexpr int INVERT = C_INVERT;
inline constexpr int LEFT = DTEXT_LEFT;
inline constexpr int CENTER = DTEXT_CENTER;
inline constexpr int RIGHT = DTEXT_RIGHT;
inline constexpr int TOP = DTEXT_TOP;
inline constexpr int MIDDLE = DTEXT_MIDDLE;
inline constexpr int BOTTOM = DTEXT_BOTTOM;
inline constexpr int RGB565 = IMAGE_RGB565;
inline constexpr int RGB565A = IMAGE_RGB565A;
inline constexpr int P8_RGB565 = IMAGE_P8_RGB565;
inline constexpr int P8_RGB565A = IMAGE_P8_RGB565A;
inline constexpr int P4_RGB565 = IMAGE_P4_RGB565;
inline constexpr int P4_RGB565A = IMAGE_P4_RGB565A;
inline constexpr int NONE = 0;
inline constexpr int HFLIP = IMAGE_HFLIP;
inline constexpr int VFLIP = IMAGE_VFLIP;
inline constexpr int CLEARBG = IMAGE_CLEARBG;
inline constexpr int SWAPCOLOR = IMAGE_SWAPCOLOR;
inline constexpr int ADDBG = IMAGE_ADDBG;
inline constexpr int DYE = IMAGE_DYE;
inline constexpr int F1 = KEY_F1;
inline constexpr int F2 = KEY_F2;
inline constexpr int F3 = KEY_F3;
inline constexpr int F4 = KEY_F4;
inline constexpr int F5 = KEY_F5;
inline constexpr int F6 = KEY_F6;
inline constexpr int SHIFT = KEY_SHIFT;
inline constexpr int OPTN = KEY_OPTN;
inline constexpr int VARS = KEY_VARS;
inline constexpr int MENU = KEY_MENU;
inline constexpr int LEFT = KEY_LEFT;
inline constexpr int RIGHT = KEY_RIGHT;
inline constexpr int UP = KEY_UP;
inline constexpr int DOWN = KEY_DOWN;
inline constexpr int ALPHA = KEY_ALPHA;
inline constexpr int SQUARE = KEY_SQUARE;
inline constexpr int POWER = KEY_POWER;
inline constexpr int EXIT = KEY_EXIT;
inline constexpr int XOT = KEY_XOT;
inline constexpr int LOG = KEY_LOG;
inline constexpr int LN = KEY_LN;
inline constexpr int SIN = KEY_SIN;
inline constexpr int COS = KEY_COS;
inline constexpr int TAN = KEY_TAN;
inline constexpr int FRAC = KEY_FRAC;
inline constexpr int FD = KEY_FD;
inline constexpr int LEFTP = KEY_LEFTP;
inline constexpr int RIGHTP = KEY_RIGHTP;
inline constexpr int COMMA = KEY_COMMA;
inline constexpr int ARROW = KEY_ARROW;
inline constexpr int K7 = KEY_7;
inline constexpr int K8 = KEY_8;
inline constexpr int K9 = KEY_9;
inline constexpr int DEL = KEY_DEL;
inline constexpr int K4 = KEY_4;
inline constexpr int K5 = KEY_5;
inline constexpr int K6 = KEY_6;
inline constexpr int MUL = KEY_MUL;
inline constexpr int DIV = KEY_DIV;
inline constexpr int K1 = KEY_1;
inline constexpr int K2 = KEY_2;
inline constexpr int K3 = KEY_3;
inline constexpr int ADD = KEY_ADD;
inline constexpr int SUB = KEY_SUB;
inline constexpr int K0 = KEY_0;
inline constexpr int DOT = KEY_DOT;
inline constexpr int EXP = KEY_EXP;
inline constexpr int NEG = KEY_NEG;
inline constexpr int EXE = KEY_EXE;
inline constexpr int ACON = KEY_ACON;
inline constexpr int HELP = KEY_HELP;
inline constexpr int LIGHT = KEY_LIGHT;
inline constexpr int X2 = KEY_X2;
inline constexpr int CARET = KEY_CARET;
inline constexpr int SWITCH = KEY_SWITCH;
inline constexpr int LEFTPAR = KEY_LEFTPAR;
inline constexpr int RIGHTPAR = KEY_RIGHTPAR;
inline constexpr int STORE = KEY_STORE;
inline constexpr int TIMES = KEY_TIMES;
inline constexpr int PLUS = KEY_PLUS;
inline constexpr int MINUS = KEY_MINUS;
inline constexpr int EXE = KEY_EXE;
inline constexpr int EXIT = KEY_EXIT;
inline constexpr int LEFT = KEY_LEFT;
inline constexpr int RIGHT = KEY_RIGHT;
inline constexpr int UP = KEY_UP;
inline constexpr int DOWN = KEY_DOWN;
inline constexpr int NONE = KEYEV_NONE;
inline constexpr int DOWN = KEYEV_DOWN;
inline constexpr int UP = KEYEV_UP;
inline constexpr int HOLD = KEYEV_HOLD;
inline constexpr int MOD_SHIFT = GETKEY_MOD_SHIFT;
inline constexpr int MOD_ALPHA = GETKEY_MOD_ALPHA;
inline constexpr int BACKLIGHT = GETKEY_BACKLIGHT;
inline constexpr int MENU = GETKEY_MENU;
inline constexpr int REP_ARROWS = GETKEY_REP_ARROWS;
inline constexpr int REP_ALL = GETKEY_REP_ALL;
inline constexpr int REP_PROFILE = GETKEY_REP_PROFILE;
inline constexpr int FEATURES = GETKEY_FEATURES;
inline constexpr int NONE = GETKEY_NONE;
inline constexpr int DEFAULT = GETKEY_DEFAULT;
inline constexpr int ANY = TIMER_ANY;
inline constexpr int TMU = TIMER_TMU;
inline constexpr int ETMU = TIMER_ETMU;
inline constexpr int CONTINUE = TIMER_CONTINUE;
inline constexpr int STOP = TIMER_STOP;
inline constexpr int PPHI_4 = TIMER_Pphi_4;
inline constexpr int PPHI_16 = TIMER_Pphi_16;
inline constexpr int PPHI_64 = TIMER_Pphi_64;
inline constexpr int PPHI_256 = TIMER_Pphi_256;
inline constexpr int HZ_500m = RTC_500mHz;
inline constexpr int HZ_1 = RTC_1Hz;
inline constexpr int HZ_2 = RTC_2Hz;
inline constexpr int HZ_4 = RTC_4Hz;
inline constexpr int HZ_16 = RTC_16Hz;
inline constexpr int HZ_64 = RTC_64Hz;
inline constexpr int HZ_256 = RTC_256Hz;
inline constexpr int NONE = RTC_NONE;
inline constexpr int ANCHOR_NONE   = ITEM_ANCHOR_NONE;
inline constexpr int ANCHOR_LEFT   = ITEM_ANCHOR_LEFT;
inline constexpr int ANCHOR_RIGHT  = ITEM_ANCHOR_RIGHT;
inline constexpr int ANCHOR_TOP    = ITEM_ANCHOR_TOP;
inline constexpr int ANCHOR_BOTTOM = ITEM_ANCHOR_BOTTOM;
inline constexpr int PAGE_GLOBAL = ITEM_PAGE_GLOBAL;
inline constexpr int PAGE_AUTO = ITEM_PAGE_AUTO;
```

## Re-exported gint functions/types

The v1 facade also intentionally exposes these `using ::...` declarations:

```cpp
using ::dclear;
using ::dupdate;
using ::drect;
using ::drect_border;
using ::dpixel;
using ::dgetpixel;
using ::dline;
using ::dhline;
using ::dvline;
using ::dtext;
using ::dtext_opt;
using ::dprint;
using ::dprint_opt;
using ::dsize;
using ::dnsize;
using ::drsize;
using ::dfont;
using ::dfont_default;
using ::dimage;
using ::dsubimage;
using ::dwindow_set;
using ::dupdate_set_hook;
using ::dupdate_get_hook;
using ::dsetvram;
using ::dgetvram;
using ::gint_vram;
using ::image_alloc;
using ::image_create;
using ::image_create_vram;
using ::image_set_palette;
using ::image_alloc_palette;
using ::image_copy_palette;
using ::image_free;
using ::image_valid;
using ::image_alpha;
using ::image_get_pixel;
using ::image_decode_pixel;
using ::image_data_size;
using ::image_set_pixel;
using ::image_copy;
using ::image_copy_alloc;
using ::image_fill;
using ::image_clear;
using ::image_sub;
using ::image_hflip;
using ::image_hflip_alloc;
using ::image_vflip;
using ::image_vflip_alloc;
using ::image_linear;
using ::image_linear_alloc;
using ::image_scale;
using ::image_rotate;
using ::image_rotate_around;
using ::image_rotate_around_scale;
using ::dsubimage_effect;
using ::dimage_rgb16;
using ::dsubimage_rgb16;
using ::dimage_p8;
using ::dsubimage_p8;
using ::dimage_p4;
using ::dsubimage_p4;
using ::dimage_rgb16_effect;
using ::dsubimage_rgb16_effect;
using ::dimage_p8_effect;
using ::dsubimage_p8_effect;
using ::dimage_p4_effect;
using ::dsubimage_p4_effect;
using ::dimage_rgb16_clearbg;
using ::dsubimage_rgb16_clearbg;
using ::dimage_p8_clearbg;
using ::dsubimage_p8_clearbg;
using ::dimage_p4_clearbg;
using ::dsubimage_p4_clearbg;
using ::dimage_rgb16_swapcolor;
using ::dsubimage_rgb16_swapcolor;
using ::dimage_p8_swapcolor;
using ::dsubimage_p8_swapcolor;
using ::dimage_p4_swapcolor;
using ::dsubimage_p4_swapcolor;
using ::dimage_rgb16_addbg;
using ::dsubimage_rgb16_addbg;
using ::dimage_p8_addbg;
using ::dsubimage_p8_addbg;
using ::dimage_p4_addbg;
using ::dsubimage_p4_addbg;
using ::dimage_rgb16_dye;
using ::dsubimage_rgb16_dye;
using ::dimage_p8_dye;
using ::dsubimage_p8_dye;
using ::dimage_p4_dye;
using ::dsubimage_p4_dye;
using ::dimage_p4_clearbg_alt;
using ::dsubimage_p4_clearbg_alt;
using ::getkey;
using ::getkey_opt;
using ::pollevent;
using ::waitevent;
using ::clearevents;
using ::keydown;
using ::keydown_all;
using ::keydown_any;
using ::keycode_function;
using ::keycode_digit;
using ::keysc_scan_frequency;
using ::keysc_scan_frequency_us;
using ::keysc_set_scan_frequency;
using ::getkey_feature_function;
using ::getkey_set_feature_function;
using ::timer_configure;
using ::timer_start;
using ::timer_pause;
using ::timer_stop;
using ::timer_wait;
using ::timer_spinwait;
using ::timer_delay;
using ::timer_reload;
using ::rtc_get_time;
using ::rtc_set_time;
using ::rtc_ticks;
using ::rtc_periodic_enable;
using ::rtc_periodic_disable;
using ::gint_world_switch;
using ::gint_world_sync;
using ::gint_osmenu;
using ::gint_osmenu_native;
using ::gint_setrestart;
using ::gint_set_quit_handler;
```

The public methods/constructors of the aliased classes are defined by the frozen
public headers listed in `API_BASELINE_V1.sha256`.
