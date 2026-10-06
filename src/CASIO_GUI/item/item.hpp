#ifndef __ITEM_H__
#define __ITEM_H__

#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/rtc.h>
#include <gint/image.h>
#include <string>
#include <functional>
#include <vector>
#include <map>
#include <climits>
#include <cstdint>
#include "../item/color.hpp"

// Compile-time default theme. Override from CMake, for example:
// add_compile_definitions(CASIO_GUI_DEFAULT_THEME=CASIO_GUI_THEME_DARK)
#ifndef CASIO_GUI_THEME_LIGHT
#define CASIO_GUI_THEME_LIGHT 0
#endif

#ifndef CASIO_GUI_THEME_DARK
#define CASIO_GUI_THEME_DARK 1
#endif

#ifndef CASIO_GUI_THEME_HIGH_CONTRAST
#define CASIO_GUI_THEME_HIGH_CONTRAST 2
#endif

#ifndef CASIO_GUI_DEFAULT_THEME
#define CASIO_GUI_DEFAULT_THEME CASIO_GUI_THEME_LIGHT
#endif


// Page assignment used by the GUI page manager.
// ITEM_PAGE_AUTO means "use the currently active page when the item is added".
// ITEM_PAGE_GLOBAL means "always active, whatever the current page".
inline constexpr int ITEM_PAGE_GLOBAL = -1;
inline constexpr int ITEM_PAGE_AUTO = -2;

//********************************* Structure **********************************

// Position and size of the item
typedef struct STRUCT_pos{
    int x;
    int y;
    int w;
    int h;

    STRUCT_pos() = default;
    STRUCT_pos(int _x, int _y, int _w, int _h)
        : x(_x), y(_y), w(_w), h(_h)
    {
    }
}STRUCT_pos;

// Simple 2D point used by geometric items
typedef struct STRUCT_point{
    int x;
    int y;

    STRUCT_point() = default;
    STRUCT_point(int _x, int _y) : x(_x), y(_y) {}
}STRUCT_point;


// Generic 2D affine transform shared by geometric items.
// Operations are accumulated in the order in which they are called.
class Transform2D{
    public:
    Transform2D() = default;

    Transform2D& reset();

    Transform2D& translate(double dx, double dy);
    Transform2D& rotate(double degrees);
    Transform2D& rotate(double degrees, double centerX, double centerY);

    Transform2D& scale(double sx, double sy);
    Transform2D& scale(double sx, double sy, double centerX, double centerY);

    // mirrorX() mirrors around a vertical X axis.
    Transform2D& mirrorX(double axisX = 0.0);

    // mirrorY() mirrors around a horizontal Y axis.
    Transform2D& mirrorY(double axisY = 0.0);

    STRUCT_point apply(STRUCT_point point) const;

    private:
    void prepend(
        double a,
        double b,
        double c,
        double d,
        double tx,
        double ty
    );

    double m00 = 1.0;
    double m01 = 0.0;
    double m02 = 0.0;
    double m10 = 0.0;
    double m11 = 1.0;
    double m12 = 0.0;
};

// Drawing mode used by shapes
enum class item_draw_mode{
    FILLED,
    OUTLINE
};

// Orientation shared by sliders, progress bars and scrollbars.
enum class item_orientation{
    HORIZONTAL,
    VERTICAL
};

// Layout policy used by container controls.
enum class item_layout_mode{
    ABSOLUTE,
    VERTICAL,
    HORIZONTAL,
    GRID
};

// Anchors used by absolute-layout children when the container is resized.
enum item_anchor{
    ITEM_ANCHOR_NONE   = 0,
    ITEM_ANCHOR_LEFT   = 1 << 0,
    ITEM_ANCHOR_RIGHT  = 1 << 1,
    ITEM_ANCHOR_TOP    = 1 << 2,
    ITEM_ANCHOR_BOTTOM = 1 << 3
};

typedef struct item_margins{
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;

    item_margins() = default;

    item_margins(
        int _left,
        int _top,
        int _right,
        int _bottom)
        : left(_left),
          top(_top),
          right(_right),
          bottom(_bottom)
    {
    }
} item_margins;


//******************************** Theme *********************************

enum class item_theme_preset{
    LIGHT,
    DARK,
    HIGH_CONTRAST
};

typedef struct item_theme{
    Class_color background = Class_color(245, 245, 245);
    Class_color surface = Class_color(255, 255, 255);
    Class_color surfaceAlt = Class_color(232, 232, 232);

    Class_color border = Class_color(30, 30, 30);
    Class_color text = Class_color(0, 0, 0);
    Class_color textMuted = Class_color(100, 100, 100);

    Class_color accent = Class_color(35, 95, 200);
    Class_color accentText = Class_color(255, 255, 255);
    Class_color selection = Class_color(205, 220, 255);

    Class_color disabled = Class_color(155, 155, 155);

    Class_color success = Class_color(20, 160, 70);
    Class_color warning = Class_color(235, 155, 20);
    Class_color error = Class_color(210, 40, 40);

    int borderSize = 1;
    int focusBorderSize = 2;
} item_theme;

item_theme make_item_theme(
    item_theme_preset preset
);

item_theme_preset default_item_theme_preset();

item_theme& current_item_theme();

void set_item_theme(
    const item_theme& theme
);

void set_item_theme_preset(
    item_theme_preset preset
);


//******************************** Text input modifiers *********************************

enum class item_alpha_mode{
    OFF,
    ONCE,
    LOCKED
};

enum class item_textbox_input_mode{
    MIXED,
    TEXT_ONLY,
    NUMERIC_ONLY
};

typedef struct item_text_input_state{
    item_alpha_mode alpha = item_alpha_mode::OFF;
    bool shift = false;

    void reset()
    {
        alpha = item_alpha_mode::OFF;
        shift = false;
    }

    bool isAlpha() const
    {
        return alpha != item_alpha_mode::OFF;
    }

    bool isAlphaLocked() const
    {
        return alpha == item_alpha_mode::LOCKED;
    }
} item_text_input_state;

// GUI-oriented ASCII mapping for GRAPH 90+E keys.
// ALPHA follows the letters printed on the keyboard.
// SHIFT provides an additional ASCII punctuation layer suitable for text fields.
char item_text_key_to_char(
    int key,
    const item_text_input_state& state
);

bool item_text_handle_modifier(
    int key,
    item_text_input_state& state
);

void item_text_consume_modifier(
    item_text_input_state& state
);

const char* item_text_modifier_label(
    const item_text_input_state& state
);


// LED indicator states.
enum class item_led_state{
    OFF,
    ON,
    WARNING,
    ERROR
};

// LED geometry.
enum class item_led_shape{
    CIRCLE,
    RECTANGLE
};

// Display policy used by image controls.
enum class item_image_mode{
    ORIGINAL,   // Native size, aligned to top-left
    CENTER,     // Native size, centered in the item
    STRETCH,    // Independent X/Y scaling to exactly match the item
    FIT,        // Preserve ratio, whole image visible
    FILL        // Preserve ratio, cover the item and crop overflow
};

// Right-angle position for item_right_triangle.
enum class item_right_triangle_corner{
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
};

// Status of the item
typedef struct item_status{
    bool hover;
    bool ON;
    bool draged;
    bool dimmed;
    bool toggle;
    bool visible;
    bool clicked;

    item_status() = default;
    item_status(
        bool _hover,
        bool _ON,
        bool _draged,
        bool _dimmed,
        bool _toggle,
        bool _visible,
        bool _clicked)
        : hover(_hover),
          ON(_ON),
          draged(_draged),
          dimmed(_dimmed),
          toggle(_toggle),
          visible(_visible),
          clicked(_clicked)
    {
    }
} item_status;



// Item label with different states and colors
typedef struct item_label{
    std::string label_ext_on;
    std::string label_ext_off;
    std::string label_on;
    std::string label_off;
    std::string label_on_hover;
    std::string label_off_hover;

    item_label() = default;
    item_label(
        std::string _label_ext_on,
        std::string _label_ext_off,
        std::string _label_on,
        std::string _label_off,
        std::string _label_on_hover,
        std::string _label_off_hover
    )
        : label_ext_on(_label_ext_on),
          label_ext_off(_label_ext_off),
          label_on(_label_on),
          label_off(_label_off),
          label_on_hover(_label_on_hover),
          label_off_hover(_label_off_hover)
    {
    }
}item_label;

//==============================================================================

struct itemEvent
{
    enum class eventType
    {
        HOVER,
        KEY_UP,
        KEY_DOWN,
        KEY_PRESS,
        DRAG
    };

    // Bit mask used when a caller wants to register the same callback for
    // several event types at once (eg. KEY_DOWN | KEY_PRESS).
    //
    // eventType itself intentionally remains a simple enum because an actual
    // dispatched event always has one and only one type. eventMask is only a
    // registration convenience.
    struct eventMask
    {
        unsigned int bits = 0;

        constexpr eventMask() = default;
        constexpr explicit eventMask(unsigned int _bits) : bits(_bits) {}

        constexpr bool contains(eventType type) const
        {
            return (bits & (1u << static_cast<unsigned int>(type))) != 0u;
        }

        constexpr bool empty() const
        {
            return bits == 0u;
        }
    };

    static constexpr eventMask mask(eventType type)
    {
        return eventMask{1u << static_cast<unsigned int>(type)};
    }

    // Legacy single-type field kept for source compatibility. When several
    // types are supplied, this contains the first type in the mask. Internal
    // event matching uses `types`, not this field.
    eventType type = eventType::KEY_DOWN;

    // One control callback can now react to one or several event types.
    eventMask types = mask(eventType::KEY_DOWN);

    int keyEvent = KEY_EXE;
    std::function<void()> callback = nullptr;

    static constexpr eventType firstType(eventMask _types)
    {
        return _types.contains(eventType::HOVER)     ? eventType::HOVER :
               _types.contains(eventType::KEY_UP)    ? eventType::KEY_UP :
               _types.contains(eventType::KEY_DOWN)  ? eventType::KEY_DOWN :
               _types.contains(eventType::KEY_PRESS) ? eventType::KEY_PRESS :
               _types.contains(eventType::DRAG)      ? eventType::DRAG :
                                                       eventType::KEY_DOWN;
    }

    itemEvent() = default;

    // Existing API: one event type.
    itemEvent(
        eventType _type,
        int _keyEvent,
        std::function<void()> _callback)
        : type(_type),
          types(mask(_type)),
          keyEvent(_keyEvent),
          callback(_callback)
    {
    }

    // New API: several control event types at once.
    // Example:
    //   ItemEvent{KEY_DOWN | KEY_PRESS, KEY_LEFT, callback}
    itemEvent(
        eventMask _types,
        int _keyEvent,
        std::function<void()> _callback)
        : type(firstType(_types)),
          types(_types),
          keyEvent(_keyEvent),
          callback(_callback)
    {
    }

    bool accepts(eventType _type) const
    {
        return types.contains(_type);
    }

    void setType(eventType _type)
    {
        type = _type;
        types = mask(_type);
    }

    void setTypes(eventMask _types)
    {
        types = _types;
        type = firstType(_types);
    }
};

// Combine event types when registering a GUI/global event.
// Examples:
//   KEY_DOWN | KEY_PRESS
//   KEY_DOWN | KEY_PRESS | KEY_UP
constexpr itemEvent::eventMask operator|(
    itemEvent::eventType lhs,
    itemEvent::eventType rhs)
{
    return itemEvent::eventMask{
        itemEvent::mask(lhs).bits | itemEvent::mask(rhs).bits
    };
}

constexpr itemEvent::eventMask operator|(
    itemEvent::eventMask lhs,
    itemEvent::eventType rhs)
{
    return itemEvent::eventMask{lhs.bits | itemEvent::mask(rhs).bits};
}

constexpr itemEvent::eventMask operator|(
    itemEvent::eventType lhs,
    itemEvent::eventMask rhs)
{
    return itemEvent::eventMask{itemEvent::mask(lhs).bits | rhs.bits};
}

constexpr itemEvent::eventMask operator|(
    itemEvent::eventMask lhs,
    itemEvent::eventMask rhs)
{
    return itemEvent::eventMask{lhs.bits | rhs.bits};
}

inline static bool ctohx(char c , int *s) {
    char x = 0;
    if (c < '0' || c > 'f') return false;
    if(c >= '0' && c <= '9') x = 48;
    if(c >= 'A' && c <= 'F') x = 65;
    if(c >= 'a' && c <= 'f') x = 97;
    *s = c - x;
    return true;
}


//==============================================================================

class item{

    public:
    item();
    virtual ~item() = default;

    //********************************** Getters ***********************************

    // position
    int getX() const   {return param.pos.x;}
    int getY() const   {return param.pos.y;}
    int getW() const   {return param.pos.w;}
    int getH() const   {return param.pos.h;}
    STRUCT_pos getPosition() const {return param.pos;}
    int getZOrder() const {return zOrder;}
    int getPointerX() const {return pointerX;}
    int getPointerY() const {return pointerY;}
    int getPageId() const {return pageId;}

    STRUCT_pos getGeometry() const {return param.pos;}
    void setGeometry(STRUCT_pos pos) {param.pos = pos;}
    void setDimensions(int w, int h) {param.pos.w = w; param.pos.h = h;}
    void setItemVisible(bool value) {param.status.visible = value;}
    void setItemDimmed(bool value) {param.status.dimmed = value;}

    void setZOrder(int z) {zOrder = z;}
    void setPageId(int page) {pageId = page;}
    void setGlobalPage() {pageId = ITEM_PAGE_GLOBAL;}

    // status
    bool isVisible() const          {return param.status.visible;}
    bool isDimmed() const           {return param.status.dimmed;}
    item_status getStatus() const   {return param.status;}
    bool isToggle() const           {return param.status.toggle;}

    //color
    //getters
    Class_color getFillColorOff() const        {return param.color.offColor.fillColor;}
    Class_color getBorderColorOff() const      {return param.color.offColor.borderColor;}
    Class_color getTextColorOff() const        {return param.color.offColor.TextColor;}

    Class_color getFillColorOn() const         {return param.color.onColor.fillColor;}
    Class_color getBorderColorOn() const       {return param.color.onColor.borderColor;}
    Class_color getTextColorOn() const         {return param.color.onColor.TextColor;}

    Class_color getFillColorHoverOff() const   {return param.color.hoverOffColor.fillColor;}
    Class_color getBorderColorHoverOff() const {return param.color.hoverOffColor.borderColor;}
    Class_color getTextColorHoverOff() const   {return param.color.hoverOffColor.TextColor;}

    Class_color getFillColorHoverOn() const    {return param.color.hoverOnColor.fillColor;}
    Class_color getBorderColorHoverOn() const  {return param.color.hoverOnColor.borderColor;}
    Class_color getTextColorHoverOn() const    {return param.color.hoverOnColor.TextColor;}

    Class_color getFillColorClick() const      {return param.color.clickColor.fillColor;}
    Class_color getBorderColorClick() const    {return param.color.clickColor.borderColor;}
    Class_color getTextColorClick() const      {return param.color.clickColor.TextColor;}

    Class_color getFillColorDrag() const       {return param.color.dragColor.fillColor;}
    Class_color getBorderColorDrag() const     {return param.color.dragColor.borderColor;}
    Class_color getTextColorDrag() const       {return param.color.dragColor.TextColor;}
    
    //setters Color
    void setFillColorOff(Class_color c)        {param.color.offColor.fillColor = c;}
    void setBorderColorOff(Class_color c)      {param.color.offColor.borderColor = c;}
    void setTextColorOff(Class_color c)        {param.color.offColor.TextColor = c;}

    void setFillColorOn(Class_color c)         {param.color.onColor.fillColor = c;}
    void setBorderColorOn(Class_color c)       {param.color.onColor.borderColor = c;}
    void setTextColorOn(Class_color c)         {param.color.onColor.TextColor = c;}

    void setFillColorHoverOff(Class_color c)   {param.color.hoverOffColor.fillColor = c;}
    void setBorderColorHoverOff(Class_color c) {param.color.hoverOffColor.borderColor = c;}
    void setTextColorHoverOff(Class_color c)   {param.color.hoverOffColor.TextColor = c;}

    void setFillColorHoverOn(Class_color c)    {param.color.hoverOnColor.fillColor = c;}
    void setBorderColorHoverOn(Class_color c)  {param.color.hoverOnColor.borderColor = c;}
    void setTextColorHoverOn(Class_color c)    {param.color.hoverOnColor.TextColor = c;}

    void setFillColorClick(Class_color c)      {param.color.clickColor.fillColor = c;}
    void setBorderColorClick(Class_color c)    {param.color.clickColor.borderColor = c;}
    void setTextColorClick(Class_color c)      {param.color.clickColor.TextColor = c;}

    void setFillColorDrag(Class_color c)       {param.color.dragColor.fillColor = c;}
    void setBorderColorDrag(Class_color c)     {param.color.dragColor.borderColor = c;}
    void setTextColorDrag(Class_color c)       {param.color.dragColor.TextColor = c;}

    int getId() const {return id;};
    void setId(int _id) {id = _id;};

    // Get label
    std::string getLabelExtOn() const       {return param.label.label_ext_on;}
    std::string getLabelExtOff() const      {return param.label.label_ext_off;}
    std::string getLabelOn() const          {return param.label.label_on;}
    std::string getLabelOff() const         {return param.label.label_off;}
    std::string getLabelOnHover() const     {return param.label.label_on_hover;}
    std::string getLabelOffHover() const    {return param.label.label_off_hover;}

    // Set label
    void setLabelExtOn(std::string l)       {param.label.label_ext_on = l;}
    void setLabelExtOff(std::string l)      {param.label.label_ext_off = l;}
    void setLabelOn(std::string l)          {param.label.label_on = l;}
    void setLabelOff(std::string l)         {param.label.label_off = l;}
    void setLabelOnHover(std::string l)     {param.label.label_on_hover = l;}
    void setLabelOffHover(std::string l)    {param.label.label_off_hover = l;}

    void connectCallback(
        std::function<void()> cb,
        int keyEvent = KEY_EXE,
        itemEvent::eventType type = itemEvent::eventType::KEY_DOWN)
    {
        event = itemEvent{type, keyEvent, cb};
    };

    // Same callback for several control event types.
    // Example:
    //   button.connectCallback(cb, KEY_EXE, KEY_DOWN | KEY_PRESS);
    void connectCallback(
        std::function<void()> cb,
        int keyEvent,
        itemEvent::eventMask types)
    {
        event = itemEvent{types, keyEvent, cb};
    };
    void callCbFunc() {if(event.callback) event.callback();};

    virtual bool contains(int x, int y) const;
    virtual bool isHover(int x, int y);
    virtual void handleEvent(int eventType, int eventKey) {(void)eventType; (void)eventKey;};
    virtual void update(uint32_t nowTicks) {(void)nowTicks;};
    virtual void applyTheme(const item_theme& theme);
    virtual bool handleSystemExit()
    {
        if(isFocusable() && hasFocus())
        {
            setFocus(false);
            return true;
        }

        return false;
    };
    virtual bool isModal() const {return false;};
    virtual bool capturesKeyboard() const {return false;};

    // Common GUI focus API
    virtual bool isFocusable() const {return false;};
    virtual bool hasFocus() const {return false;};
    virtual void setFocus(bool focused) {(void)focused;};

    // Pointer focus follows the actual control under the cursor. Containers
    // override this recursively so wrappers don't steal focus from children.
    virtual bool wantsPointerFocus(int x, int y) const
    {
        return contains(x, y) && isFocusable();
    };

    // Items such as the bottom soft-key bar can listen to keyboard events
    // without taking focus or blocking the mouse.
    virtual bool wantsGlobalKeyboard() const {return false;};
    virtual void handleGlobalEvent(int eventType, int eventKey) {(void)eventType; (void)eventKey;};

    bool isClick(int clkEvent) {param.status.clicked = (param.status.hover && !param.status.dimmed && param.status.visible && event.keyEvent == clkEvent); return param.status.clicked;}
    bool isDrag() {return (param.status.draged && !param.status.dimmed && param.status.visible && event.accepts(itemEvent::eventType::DRAG));}
    void updateItemStatus(int x , int y) {isHover(x, y); isClick(event.keyEvent); isDrag();};
    void clearHover() {param.status.hover = false;};
    void clearPointerState() {param.status.hover = false; param.status.clicked = false; param.status.draged = false;};
    void chckEvent(int eventType, int eventKey);
    virtual void draw() = 0;

    protected:
    void setSize(int w , int h) {param.pos.w = w; param.pos.h = h;};
    void setStructPos(STRUCT_pos _pos) {param.pos = _pos;};
    void setVisible(bool v) {param.status.visible = v;};
    void setDimmed(bool d) {param.status.dimmed = d;};
    void setClicked(bool c) {param.status.clicked = c;};
    void setStatus(item_status s) {param.status = s;};
    void setDragged(bool d) {param.status.draged = d;};
    void setToggle(bool t) {param.status.toggle = t;};

    struct param{
        item_label label;
        STRUCT_pos pos;
        item_status status;
        itemColor color;
    }param;
    int id = 0;
    int zOrder = 0;
    int pageId = ITEM_PAGE_AUTO;
    int pointerX = 0;
    int pointerY = 0;
    itemEvent event;
};

class item_button : public item{
    public:
    item_button(
        STRUCT_pos pos = STRUCT_pos{0, 0, 100, 50},
        itemEvent event = itemEvent{itemEvent::eventType::KEY_DOWN, KEY_EXE, nullptr},
        item_status status = item_status{false, false, false, false, false, true, false},
        itemColor colors = itemColor{RawitemColor(C_WHITE, C_BLACK, C_BLACK), RawitemColor(C_BLACK, C_WHITE, C_WHITE), RawitemColor(C_LIGHT, C_WHITE, C_WHITE), RawitemColor(C_LIGHT, C_WHITE, C_WHITE), RawitemColor(C_RED, C_BLACK, C_BLACK), RawitemColor(C_BLUE, C_BLACK, C_BLACK), RawitemColor(C_DARK, C_BLACK, C_BLACK)},
        item_label label = item_label{"", "", "", "", "", ""}
    );
    ~item_button() override = default;
    void draw() override;
    private:
    bool isHover(int x, int y) override;
};



//******************************** Basic shapes *********************************

class item_rectangle : public item{
    public:
    item_rectangle(
        STRUCT_pos pos = STRUCT_pos{0, 0, 100, 50},
        Class_color fillColor = Class_color(255, 255, 255),
        Class_color borderColor = Class_color(0, 0, 0),
        item_draw_mode mode = item_draw_mode::FILLED,
        int borderSize = 1,
        int zOrder = 0
    );

    ~item_rectangle() override = default;

    void draw() override;

    void setDrawMode(item_draw_mode mode) {drawMode = mode;};
    item_draw_mode getDrawMode() const {return drawMode;};

    void setBorderSize(int size) {borderSize = (size > 0) ? size : 1;};
    int getBorderSize() const {return borderSize;};

    protected:
    item_draw_mode drawMode = item_draw_mode::FILLED;
    int borderSize = 1;
};

class item_square : public item_rectangle{
    public:
    item_square(
        int x = 0,
        int y = 0,
        int size = 50,
        Class_color fillColor = Class_color(255, 255, 255),
        Class_color borderColor = Class_color(0, 0, 0),
        item_draw_mode mode = item_draw_mode::FILLED,
        int borderSize = 1,
        int zOrder = 0
    ) : item_rectangle(
        STRUCT_pos{x, y, size, size},
        fillColor,
        borderColor,
        mode,
        borderSize,
        zOrder)
    {
    }

    ~item_square() override = default;
};

class item_circle : public item{
    public:
    item_circle(
        int centerX = 50,
        int centerY = 50,
        int radius = 20,
        Class_color fillColor = Class_color(255, 255, 255),
        Class_color borderColor = Class_color(0, 0, 0),
        item_draw_mode mode = item_draw_mode::FILLED,
        int borderSize = 1,
        int zOrder = 0
    );

    ~item_circle() override = default;

    bool contains(int x, int y) const override;
    void draw() override;

    void setCenter(int x, int y);
    void setRadius(int radius);

    int getCenterX() const {return centerX;};
    int getCenterY() const {return centerY;};
    int getRadius() const {return radius;};

    void setDrawMode(item_draw_mode mode) {drawMode = mode;};
    item_draw_mode getDrawMode() const {return drawMode;};

    private:
    int centerX;
    int centerY;
    int radius;
    int borderSize;
    item_draw_mode drawMode;
};

class item_triangle : public item{
    public:
    item_triangle(
        STRUCT_point p1 = STRUCT_point{10, 10},
        STRUCT_point p2 = STRUCT_point{60, 10},
        STRUCT_point p3 = STRUCT_point{35, 50},
        Class_color fillColor = Class_color(255, 255, 255),
        Class_color borderColor = Class_color(0, 0, 0),
        item_draw_mode mode = item_draw_mode::FILLED,
        int zOrder = 0
    );

    ~item_triangle() override = default;

    bool contains(int x, int y) const override;
    void draw() override;

    void setPoints(STRUCT_point p1, STRUCT_point p2, STRUCT_point p3);

    void rotate(float degrees);
    void rotate(float degrees, STRUCT_point center);
    void mirrorX();
    void mirrorX(int axisX);
    void mirrorY();
    void mirrorY(int axisY);
    void translate(int dx, int dy);
    void scale(float sx, float sy);
    void scale(float sx, float sy, STRUCT_point center);
    void applyTransform(const Transform2D& transform);

    STRUCT_point getP1() const {return p1;};
    STRUCT_point getP2() const {return p2;};
    STRUCT_point getP3() const {return p3;};

    void setDrawMode(item_draw_mode mode) {drawMode = mode;};
    item_draw_mode getDrawMode() const {return drawMode;};

    private:
    void updateBoundingBox();

    STRUCT_point p1;
    STRUCT_point p2;
    STRUCT_point p3;
    item_draw_mode drawMode;
};


// Generic polygon item. It is also the base for trapezoids, regular polygons
// and right triangles so the hit-test / transformations remain consistent.
class item_polygon : public item{
    public:
    item_polygon(
        std::vector<STRUCT_point> points = {},
        Class_color fillColor = Class_color(255, 255, 255),
        Class_color borderColor = Class_color(0, 0, 0),
        item_draw_mode mode = item_draw_mode::FILLED,
        int borderSize = 1,
        int zOrder = 0
    );

    ~item_polygon() override = default;

    bool contains(int x, int y) const override;
    void draw() override;

    void setPoints(const std::vector<STRUCT_point>& value);
    const std::vector<STRUCT_point>& getPoints() const {return points;};
    void addPoint(STRUCT_point point);

    void setDrawMode(item_draw_mode mode) {drawMode = mode;};
    item_draw_mode getDrawMode() const {return drawMode;};

    void setBorderSize(int value) {borderSize = (value > 0) ? value : 1;};
    int getBorderSize() const {return borderSize;};

    // Transformations are performed around the polygon's bounding-box center
    // unless an explicit axis/center is supplied.
    void rotate(float degrees);
    void rotate(float degrees, STRUCT_point center);
    void mirrorX();
    void mirrorX(int axisX);
    void mirrorY();
    void mirrorY(int axisY);
    void translate(int dx, int dy);
    void scale(float sx, float sy);
    void scale(float sx, float sy, STRUCT_point center);
    void applyTransform(const Transform2D& transform);

    protected:
    void updateBoundingBox();

    std::vector<STRUCT_point> points;

    // Reused by scanline filling. Capacity only grows when polygon complexity
    // grows, so draw() does not allocate each frame.
    std::vector<int> scanlineScratch;

    item_draw_mode drawMode = item_draw_mode::FILLED;
    int borderSize = 1;
};

class item_right_triangle : public item_polygon{
    public:
    item_right_triangle(
        STRUCT_pos bounds = STRUCT_pos{0, 0, 60, 40},
        item_right_triangle_corner corner = item_right_triangle_corner::TOP_LEFT,
        Class_color fillColor = Class_color(255, 255, 255),
        Class_color borderColor = Class_color(0, 0, 0),
        item_draw_mode mode = item_draw_mode::FILLED,
        int borderSize = 1,
        int zOrder = 0
    );

    ~item_right_triangle() override = default;
};

class item_trapezoid : public item_polygon{
    public:
    item_trapezoid(
        STRUCT_pos bounds = STRUCT_pos{0, 0, 100, 50},
        int topWidth = 60,
        int bottomWidth = 100,
        Class_color fillColor = Class_color(255, 255, 255),
        Class_color borderColor = Class_color(0, 0, 0),
        item_draw_mode mode = item_draw_mode::FILLED,
        int borderSize = 1,
        int zOrder = 0
    );

    ~item_trapezoid() override = default;
};

class item_regular_polygon : public item_polygon{
    public:
    item_regular_polygon(
        int centerX = 50,
        int centerY = 50,
        int radius = 25,
        unsigned int sides = 5,
        float rotationDegrees = -90.0f,
        Class_color fillColor = Class_color(255, 255, 255),
        Class_color borderColor = Class_color(0, 0, 0),
        item_draw_mode mode = item_draw_mode::FILLED,
        int borderSize = 1,
        int zOrder = 0
    );

    ~item_regular_polygon() override = default;
};

class item_ellipse : public item{
    public:
    item_ellipse(
        int centerX = 50,
        int centerY = 50,
        int radiusX = 30,
        int radiusY = 20,
        Class_color fillColor = Class_color(255, 255, 255),
        Class_color borderColor = Class_color(0, 0, 0),
        item_draw_mode mode = item_draw_mode::FILLED,
        int borderSize = 1,
        int zOrder = 0
    );

    ~item_ellipse() override = default;

    bool contains(int x, int y) const override;
    void draw() override;

    private:
    void rebuildExtentCache();

    int centerX = 0;
    int centerY = 0;
    int radiusX = 1;
    int radiusY = 1;
    int borderSize = 1;
    item_draw_mode drawMode = item_draw_mode::FILLED;

    // extentCache[dy + radiusY] = horizontal half-width for this scanline.
    std::vector<int> extentCache;
};

class item_line : public item{
    public:
    item_line(
        STRUCT_point p1 = STRUCT_point{0, 0},
        STRUCT_point p2 = STRUCT_point{50, 50},
        Class_color color = Class_color(0, 0, 0),
        int thickness = 1,
        int zOrder = 0
    );

    ~item_line() override = default;

    bool contains(int x, int y) const override;
    void draw() override;

    void setPoints(STRUCT_point p1, STRUCT_point p2);

    STRUCT_point getP1() const {return p1;};
    STRUCT_point getP2() const {return p2;};

    void translate(int dx, int dy);
    void rotate(float degrees);
    void rotate(float degrees, STRUCT_point center);
    void scale(float sx, float sy);
    void scale(float sx, float sy, STRUCT_point center);
    void mirrorX();
    void mirrorX(int axisX);
    void mirrorY();
    void mirrorY(int axisY);
    void applyTransform(const Transform2D& transform);

    private:
    void updateBoundingBox();

    STRUCT_point p1;
    STRUCT_point p2;
    Class_color lineColor;
    int thickness = 1;
};


// Open polyline. Unlike item_polygon, the last point is not connected
// automatically to the first one.
class item_polyline : public item{
    public:
    item_polyline(
        std::vector<STRUCT_point> points = {},
        Class_color color = Class_color(0, 0, 0),
        int thickness = 1,
        int zOrder = 0
    );

    ~item_polyline() override = default;

    bool contains(int x, int y) const override;
    void draw() override;

    void setPoints(const std::vector<STRUCT_point>& value);
    const std::vector<STRUCT_point>& getPoints() const {return points;};
    void addPoint(STRUCT_point point);

    void translate(int dx, int dy);
    void rotate(float degrees);
    void rotate(float degrees, STRUCT_point center);
    void scale(float sx, float sy);
    void scale(float sx, float sy, STRUCT_point center);
    void mirrorX();
    void mirrorX(int axisX);
    void mirrorY();
    void mirrorY(int axisY);
    void applyTransform(const Transform2D& transform);

    private:
    void updateBoundingBox();

    std::vector<STRUCT_point> points;
    Class_color lineColor;
    int thickness = 1;
};


// Arc represented as an open polyline approximation.
class item_arc : public item_polyline{
    public:
    item_arc(
        int centerX = 50,
        int centerY = 50,
        int radius = 25,
        float startDegrees = 0.0f,
        float endDegrees = 180.0f,
        Class_color color = Class_color(0, 0, 0),
        int thickness = 1,
        unsigned int segments = 36,
        int zOrder = 0
    );

    ~item_arc() override = default;

    void setGeometry(
        int centerX,
        int centerY,
        int radius,
        float startDegrees,
        float endDegrees
    );

    private:
    static std::vector<STRUCT_point> buildPoints(
        int centerX,
        int centerY,
        int radius,
        float startDegrees,
        float endDegrees,
        unsigned int segments
    );

    int centerX = 0;
    int centerY = 0;
    int radius = 1;
    float startDegrees = 0.0f;
    float endDegrees = 180.0f;
    unsigned int segments = 36;
};


// Circular sector / pie slice. It inherits polygon transformations.
class item_sector : public item_polygon{
    public:
    item_sector(
        int centerX = 50,
        int centerY = 50,
        int radius = 25,
        float startDegrees = 0.0f,
        float endDegrees = 90.0f,
        Class_color fillColor = Class_color(255, 255, 255),
        Class_color borderColor = Class_color(0, 0, 0),
        item_draw_mode mode = item_draw_mode::FILLED,
        int borderSize = 1,
        unsigned int segments = 36,
        int zOrder = 0
    );

    ~item_sector() override = default;

    void setGeometry(
        int centerX,
        int centerY,
        int radius,
        float startDegrees,
        float endDegrees
    );

    private:
    static std::vector<STRUCT_point> buildPoints(
        int centerX,
        int centerY,
        int radius,
        float startDegrees,
        float endDegrees,
        unsigned int segments
    );

    int centerX = 0;
    int centerY = 0;
    int radius = 1;
    float startDegrees = 0.0f;
    float endDegrees = 90.0f;
    unsigned int segments = 36;
};

using item_pie = item_sector;


// Rounded rectangle implemented as a polygon approximation so it naturally
// benefits from Transform2D, Z-order and polygon hit-testing.
class item_rounded_rectangle : public item_polygon{
    public:
    item_rounded_rectangle(
        STRUCT_pos bounds = STRUCT_pos{0, 0, 100, 50},
        int radius = 8,
        Class_color fillColor = Class_color(255, 255, 255),
        Class_color borderColor = Class_color(0, 0, 0),
        item_draw_mode mode = item_draw_mode::FILLED,
        int borderSize = 1,
        unsigned int segmentsPerCorner = 5,
        int zOrder = 0
    );

    ~item_rounded_rectangle() override = default;

    void setBounds(STRUCT_pos bounds);
    void setRadius(int radius);

    private:
    void rebuild();

    STRUCT_pos bounds{0, 0, 100, 50};
    int radius = 8;
    unsigned int segmentsPerCorner = 5;
};


//******************************** Basic controls *********************************

class item_toggle_button : public item_button{
    public:
    item_toggle_button(
        STRUCT_pos pos = STRUCT_pos{0, 0, 100, 50},
        itemEvent event = itemEvent{itemEvent::eventType::KEY_DOWN, KEY_EXE, nullptr},
        item_status status = item_status{false, false, false, false, true, true, false},
        itemColor colors = itemColor{RawitemColor(C_WHITE, C_BLACK, C_BLACK), RawitemColor(C_BLACK, C_WHITE, C_WHITE), RawitemColor(C_LIGHT, C_WHITE, C_WHITE), RawitemColor(C_LIGHT, C_WHITE, C_WHITE), RawitemColor(C_RED, C_BLACK, C_BLACK), RawitemColor(C_BLUE, C_BLACK, C_BLACK), RawitemColor(C_DARK, C_BLACK, C_BLACK)},
        item_label label = item_label{"", "", "", "", "", ""}
    );

    ~item_toggle_button() override = default;

    void handleEvent(int eventType, int eventKey) override;
    bool isOn() const {return param.status.ON;};
    void setOn(bool on) {param.status.ON = on;};
};

class item_textbox : public item{
    public:
    item_textbox(
        STRUCT_pos pos = STRUCT_pos{0, 0, 120, 28},
        std::string text = "",
        std::string placeholder = "",
        unsigned int maxLength = 32,
        int zOrder = 0,
        item_textbox_input_mode inputMode = item_textbox_input_mode::MIXED
    );

    ~item_textbox() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;
    void applyTheme(const item_theme& theme) override;
    bool capturesKeyboard() const override {return focused;};
    bool isFocusable() const override {return true;};
    bool hasFocus() const override {return focused;};
    void setFocus(bool value) override;

    const std::string& getText() const {return text;};
    void setText(const std::string& value);
    void clear();

    bool isFocused() const {return focused;};
    void setFocused(bool value) {setFocus(value);};

    unsigned int getMaxLength() const {return maxLength;};
    void setMaxLength(unsigned int value) {maxLength = (value > 0) ? value : 1;};

    const std::string& getPlaceholder() const {return placeholder;};
    void setPlaceholder(const std::string& value) {placeholder = value;};

    const item_text_input_state& getInputState() const {return inputState;};
    void resetInputModifiers();
    void setAlphaLock(bool value);

    bool isAlphaLock() const {return inputState.isAlphaLocked();};
    bool isShiftPending() const {return inputState.shift;};

    void setInputMode(item_textbox_input_mode value);
    item_textbox_input_mode getInputMode() const {return inputMode;};

    // MIXED only: true starts in text mode when focus is acquired.
    void setAutoAlphaOnFocus(bool value) {autoAlphaOnFocus = value;};
    bool getAutoAlphaOnFocus() const {return autoAlphaOnFocus;};

    // SHIFT toggles this state while the field is in text entry mode.
    void setCapsLock(bool value) {capsLock = value;};
    bool getCapsLock() const {return capsLock;};

    bool isTextEntryMode() const
    {
        return inputMode == item_textbox_input_mode::TEXT_ONLY ||
               (
                   inputMode == item_textbox_input_mode::MIXED &&
                   inputState.isAlpha()
               );
    };

    bool isNumericEntryMode() const
    {
        return inputMode == item_textbox_input_mode::NUMERIC_ONLY ||
               (
                   inputMode == item_textbox_input_mode::MIXED &&
                   !inputState.isAlpha()
               );
    };

    void setOnTextChanged(
        std::function<void(const std::string&)> callback)
    {
        onTextChanged = callback;
    };

    // Optional application-specific mapping. When it returns 0, the default
    // GRAPH 90+E ALPHA/SHIFT mapping is used.
    void setKeyMapper(
        std::function<char(
            int,
            const item_text_input_state&
        )> value)
    {
        keyMapper = value;
    };

    private:
    char keyToChar(int key) const;
    void notifyTextChanged();

    std::string text;
    std::string placeholder;
    unsigned int maxLength;
    bool focused = false;
    bool autoAlphaOnFocus = true;
    bool capsLock = false;

    item_textbox_input_mode inputMode =
        item_textbox_input_mode::MIXED;

    item_text_input_state inputState;

    std::function<void(const std::string&)> onTextChanged = nullptr;

    std::function<char(
        int,
        const item_text_input_state&
    )> keyMapper = nullptr;
};

class item_numeric : public item{
    public:
    item_numeric(
        STRUCT_pos pos = STRUCT_pos{0, 0, 100, 28},
        int value = 0,
        int minValue = 0,
        int maxValue = 100,
        int step = 1,
        int zOrder = 0
    );

    ~item_numeric() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;
    bool capturesKeyboard() const override {return active;};
    bool isFocusable() const override {return true;};
    bool hasFocus() const override {return active;};
    void setFocus(bool value) override {active = value;};

    int getValue() const {return value;};
    void setValue(int value);

    int getMinValue() const {return minValue;};
    int getMaxValue() const {return maxValue;};
    int getStep() const {return step;};

    void setRange(int minValue, int maxValue);
    void setStep(int step) {this->step = (step > 0) ? step : 1;};

    void setOnChanged(std::function<void(int)> callback)
    {
        onChanged = callback;
    };

    bool isActive() const {return active;};

    private:
    int value;
    int minValue;
    int maxValue;
    int step;
    bool active = false;

    std::function<void(int)> onChanged = nullptr;
};

class item_slider : public item{
    public:
    item_slider(
        STRUCT_pos pos = STRUCT_pos{0, 0, 140, 24},
        int value = 0,
        int minValue = 0,
        int maxValue = 100,
        int step = 1,
        int zOrder = 0,
        item_orientation orientation = item_orientation::HORIZONTAL
    );

    ~item_slider() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;
    bool capturesKeyboard() const override {return active;};
    bool isFocusable() const override {return true;};
    bool hasFocus() const override {return active;};
    void setFocus(bool value) override {active = value;};

    int getValue() const {return value;};
    void setValue(int value);

    int getMinValue() const {return minValue;};
    int getMaxValue() const {return maxValue;};
    int getStep() const {return step;};

    void setRange(int minValue, int maxValue);
    void setStep(int step) {this->step = (step > 0) ? step : 1;};

    void setOrientation(item_orientation value) {orientation = value;};
    item_orientation getOrientation() const {return orientation;};

    void setOnChanged(std::function<void(int)> callback) {onChanged = callback;};

    bool isActive() const {return active;};

    private:
    int value;
    int minValue;
    int maxValue;
    int step;
    bool active = false;
    item_orientation orientation = item_orientation::HORIZONTAL;
    std::function<void(int)> onChanged = nullptr;
};

using item_horizontal_slider = item_slider;

class item_vertical_slider : public item_slider{
    public:
    item_vertical_slider(
        STRUCT_pos pos = STRUCT_pos{0, 0, 24, 140},
        int value = 0,
        int minValue = 0,
        int maxValue = 100,
        int step = 1,
        int zOrder = 0
    )
        : item_slider(
            pos,
            value,
            minValue,
            maxValue,
            step,
            zOrder,
            item_orientation::VERTICAL)
    {
    }
};


//******************************** Extended controls *********************************

class item_checkbox : public item{
    public:
    item_checkbox(
        STRUCT_pos pos = STRUCT_pos{0, 0, 120, 24},
        std::string label = "Checkbox",
        bool checked = false,
        int zOrder = 0
    );

    ~item_checkbox() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;

    // Standalone Checkbox is pointer/cursor-driven; it must not capture
    // directional keys after EXE.
    bool isFocusable() const override {return false;};
    bool capturesKeyboard() const override {return false;};
    bool hasFocus() const override {return false;};
    void setFocus(bool value) override {(void)value;};

    bool isChecked() const {return checked;};
    void setChecked(bool value);
    void toggle();

    void setLabel(const std::string& value) {label = value;};
    const std::string& getCheckboxLabel() const {return label;};

    void setOnChanged(std::function<void(bool)> callback) {onChanged = callback;};

    private:
    bool checked = false;
    std::string label;
    std::function<void(bool)> onChanged = nullptr;
};


class item_progress_bar : public item{
    public:
    item_progress_bar(
        STRUCT_pos pos = STRUCT_pos{0, 0, 140, 20},
        int value = 0,
        int minValue = 0,
        int maxValue = 100,
        item_orientation orientation = item_orientation::HORIZONTAL,
        bool showPercent = true,
        int zOrder = 0
    );

    ~item_progress_bar() override = default;

    void draw() override;
    void applyTheme(const item_theme& theme) override;

    int getValue() const {return value;};
    void setValue(int value);

    int getMinValue() const {return minValue;};
    int getMaxValue() const {return maxValue;};
    void setRange(int minValue, int maxValue);

    void setOrientation(item_orientation value) {orientation = value;};
    item_orientation getOrientation() const {return orientation;};

    void setShowPercent(bool value) {showPercent = value;};
    bool getShowPercent() const {return showPercent;};

    void setProgressColor(Class_color value) {progressColor = value;};
    void setBackgroundColor(Class_color value) {backgroundColor = value;};
    void setProgressBorderColor(Class_color value) {borderColor = value;};

    private:
    int value = 0;
    int minValue = 0;
    int maxValue = 100;
    item_orientation orientation = item_orientation::HORIZONTAL;
    bool showPercent = true;

    Class_color progressColor = Class_color(0, 120, 255);
    Class_color backgroundColor = Class_color(230, 230, 230);
    Class_color borderColor = Class_color(0, 0, 0);
};

using item_horizontal_progress_bar = item_progress_bar;

class item_vertical_progress_bar : public item_progress_bar{
    public:
    item_vertical_progress_bar(
        STRUCT_pos pos = STRUCT_pos{0, 0, 20, 140},
        int value = 0,
        int minValue = 0,
        int maxValue = 100,
        bool showPercent = true,
        int zOrder = 0
    )
        : item_progress_bar(
            pos,
            value,
            minValue,
            maxValue,
            item_orientation::VERTICAL,
            showPercent,
            zOrder)
    {
    }
};


class item_scrollbar : public item{
    public:
    item_scrollbar(
        STRUCT_pos pos = STRUCT_pos{0, 0, 140, 16},
        int position = 0,
        int minValue = 0,
        int maxValue = 100,
        int pageSize = 10,
        int step = 1,
        item_orientation orientation = item_orientation::HORIZONTAL,
        int zOrder = 0
    );

    ~item_scrollbar() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;

    bool isFocusable() const override {return true;};
    bool capturesKeyboard() const override {return active;};
    bool hasFocus() const override {return active;};
    void setFocus(bool value) override {active = value;};

    int getPosition() const {return position;};
    void setPosition(int value);

    int getMinValue() const {return minValue;};
    int getMaxValue() const {return maxValue;};
    void setRange(int minValue, int maxValue);

    int getPageSize() const {return pageSize;};
    void setPageSize(int value) {pageSize = (value > 0) ? value : 1;};

    int getStep() const {return step;};
    void setStep(int value) {step = (value > 0) ? value : 1;};

    void setOrientation(item_orientation value) {orientation = value;};
    item_orientation getOrientation() const {return orientation;};

    void setOnScroll(std::function<void(int)> callback) {onScroll = callback;};

    bool isActive() const {return active;};

    private:
    int thumbLength() const;
    int thumbOffset() const;
    void setPositionFromPointer();

    int position = 0;
    int minValue = 0;
    int maxValue = 100;
    int pageSize = 10;
    int step = 1;
    item_orientation orientation = item_orientation::HORIZONTAL;
    bool active = false;
    std::function<void(int)> onScroll = nullptr;
};

using item_horizontal_scrollbar = item_scrollbar;

class item_vertical_scrollbar : public item_scrollbar{
    public:
    item_vertical_scrollbar(
        STRUCT_pos pos = STRUCT_pos{0, 0, 16, 140},
        int position = 0,
        int minValue = 0,
        int maxValue = 100,
        int pageSize = 10,
        int step = 1,
        int zOrder = 0
    )
        : item_scrollbar(
            pos,
            position,
            minValue,
            maxValue,
            pageSize,
            step,
            item_orientation::VERTICAL,
            zOrder)
    {
    }
};


class item_led : public item{
    public:
    item_led(
        STRUCT_pos pos = STRUCT_pos{0, 0, 24, 24},
        item_led_state state = item_led_state::OFF,
        std::string label = "",
        item_led_shape shape = item_led_shape::CIRCLE,
        int zOrder = 0
    );

    ~item_led() override = default;

    void draw() override;
    void update(uint32_t nowTicks) override;

    item_led_state getState() const {return state;};
    void setState(item_led_state value);

    void setLabel(const std::string& value) {label = value;};
    const std::string& getLedLabel() const {return label;};

    void setShape(item_led_shape value) {shape = value;};
    item_led_shape getShape() const {return shape;};

    void setBlink(bool enabled, uint32_t intervalMs = 500);
    bool isBlinking() const {return blinkEnabled;};

    void setOffColor(Class_color value) {offColor = value;};
    void setOnColor(Class_color value) {onColor = value;};
    void setWarningColor(Class_color value) {warningColor = value;};
    void setErrorColor(Class_color value) {errorColor = value;};

    Class_color getOffColor() const {return offColor;};
    Class_color getOnColor() const {return onColor;};
    Class_color getWarningColor() const {return warningColor;};
    Class_color getErrorColor() const {return errorColor;};

    void setOnChanged(std::function<void(item_led_state)> callback) {onChanged = callback;};

    private:
    Class_color currentColor() const;

    item_led_state state = item_led_state::OFF;
    item_led_shape shape = item_led_shape::CIRCLE;
    std::string label;

    Class_color offColor = Class_color(70, 70, 70);
    Class_color onColor = Class_color(0, 220, 0);
    Class_color warningColor = Class_color(255, 180, 0);
    Class_color errorColor = Class_color(255, 0, 0);

    bool blinkEnabled = false;
    bool blinkVisible = true;
    uint32_t blinkIntervalMs = 500;
    uint32_t lastBlinkTicks = 0;

    std::function<void(item_led_state)> onChanged = nullptr;
};


class item_color_wheel : public item{
    public:
    item_color_wheel(
        STRUCT_pos pos = STRUCT_pos{0, 0, 100, 100},
        float hue = 0.0f,
        float step = 2.0f,
        int zOrder = 0,
        unsigned int segments = 240
    );

    ~item_color_wheel() override = default;

    void draw() override;
    bool contains(int x, int y) const override;
    bool isHover(int x, int y) override;
    void handleEvent(int eventType, int eventKey) override;

    bool isFocusable() const override {return true;};
    bool capturesKeyboard() const override {return focused;};
    bool hasFocus() const override {return focused;};
    void setFocus(bool value) override {focused = value;};

    float getHue() const {return hue;};
    void setHue(float value);

    float getStep() const {return step;};
    void setStep(float value) {step = (value > 0.0f) ? value : 1.0f;};

    Class_color getColor() const;

    void setRingRatio(float value);
    float getRingRatio() const {return ringRatio;};

    void setOnChanged(std::function<void(float, Class_color)> callback) {onChanged = callback;};

    private:
    static float normalizeHue(float value);
    static Class_color hueToColor(float value);
    void rebuildColorCache();
    void setHueFromPointer();

    float hue = 0.0f;
    float step = 2.0f;
    float ringRatio = 0.58f;
    bool focused = false;
    unsigned int segments = 240;
    std::vector<int> wheelColors;

    // Unit-circle cache avoids 2 * segments trigonometric calls per frame.
    std::vector<float> wheelCos;
    std::vector<float> wheelSin;

    std::function<void(float, Class_color)> onChanged = nullptr;
};

using item_hue_selector = item_color_wheel;


//******************************** Instruments *********************************

typedef struct gauge_zone{
    int minValue = 0;
    int maxValue = 0;
    Class_color color = Class_color(0, 180, 0);

    gauge_zone() = default;

    gauge_zone(
        int _minValue,
        int _maxValue,
        Class_color _color)
        : minValue(_minValue),
          maxValue(_maxValue),
          color(_color)
    {
    }
} gauge_zone;


class item_gauge : public item{
    public:
    item_gauge(
        STRUCT_pos pos = STRUCT_pos{0, 0, 120, 100},
        int value = 0,
        int minValue = 0,
        int maxValue = 100,
        float startAngle = 135.0f,
        float endAngle = 405.0f,
        unsigned int majorDivisions = 10,
        unsigned int minorDivisions = 4,
        std::string unit = "",
        int zOrder = 0
    );

    ~item_gauge() override = default;

    void draw() override;
    void applyTheme(const item_theme& theme) override;

    int getValue() const {return value;};
    virtual void setValue(int value);

    int getMinValue() const {return minValue;};
    int getMaxValue() const {return maxValue;};
    void setRange(int minValue, int maxValue);

    float getStartAngle() const {return startAngle;};
    float getEndAngle() const {return endAngle;};
    void setAngles(float startAngle, float endAngle);

    unsigned int getMajorDivisions() const {return majorDivisions;};
    unsigned int getMinorDivisions() const {return minorDivisions;};
    void setGraduations(
        unsigned int majorDivisions,
        unsigned int minorDivisions
    );

    void setUnit(const std::string& value) {unit = value;};
    const std::string& getUnit() const {return unit;};

    void setShowValue(bool value) {showValue = value;};
    bool getShowValue() const {return showValue;};

    void setShowMinMax(bool value) {showMinMax = value;};
    bool getShowMinMax() const {return showMinMax;};

    void setFaceColor(Class_color value) {faceColor = value;};
    void setScaleColor(Class_color value) {scaleColor = value;};
    void setNeedleColor(Class_color value) {needleColor = value;};
    void setHubColor(Class_color value) {hubColor = value;};
    void setTextColor(Class_color value) {textColor = value;};

    void addZone(
        int minValue,
        int maxValue,
        Class_color color
    );

    void clearZones() {zones.clear();};
    const std::vector<gauge_zone>& getZones() const {return zones;};

    protected:
    int clampValue(int value) const;
    float valueToAngle(int value) const;

    int gaugeCenterX() const;
    int gaugeCenterY() const;
    int gaugeRadius() const;

    STRUCT_point pointOnGauge(
        float angle,
        int radius
    ) const;

    int value = 0;
    int minValue = 0;
    int maxValue = 100;

    float startAngle = 135.0f;
    float endAngle = 405.0f;

    unsigned int majorDivisions = 10;
    unsigned int minorDivisions = 4;

    std::string unit;

    bool showValue = true;
    bool showMinMax = true;

    Class_color faceColor = Class_color(245, 245, 245);
    Class_color scaleColor = Class_color(0, 0, 0);
    Class_color needleColor = Class_color(220, 0, 0);
    Class_color hubColor = Class_color(30, 30, 30);
    Class_color textColor = Class_color(0, 0, 0);

    std::vector<gauge_zone> zones;
};

using item_needle_meter = item_gauge;
using item_meter = item_gauge;


class item_dial : public item_gauge{
    public:
    item_dial(
        STRUCT_pos pos = STRUCT_pos{0, 0, 120, 100},
        int value = 0,
        int minValue = 0,
        int maxValue = 100,
        int step = 1,
        float startAngle = 135.0f,
        float endAngle = 405.0f,
        unsigned int majorDivisions = 10,
        unsigned int minorDivisions = 4,
        std::string unit = "",
        int zOrder = 0
    );

    ~item_dial() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;

    bool isFocusable() const override {return true;};
    bool capturesKeyboard() const override {return active;};
    bool hasFocus() const override {return active;};
    void setFocus(bool value) override {active = value;};

    void setValue(int value) override;

    int getStep() const {return step;};
    void setStep(int value) {step = (value > 0) ? value : 1;};

    bool isActive() const {return active;};

    void setOnChanged(
        std::function<void(int)> callback)
    {
        onChanged = callback;
    };

    private:
    void setValueFromPointer();

    int step = 1;
    bool active = false;
    std::function<void(int)> onChanged = nullptr;
};


class item_tab : public item{
    public:
    item_tab(
        STRUCT_pos pos = STRUCT_pos{0, 0, 180, 28},
        std::vector<std::string> tabs = std::vector<std::string>{"Tab 1", "Tab 2"},
        int selectedTab = 0,
        int zOrder = 0
    );

    ~item_tab() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;
    bool capturesKeyboard() const override {return focused;};
    bool isFocusable() const override {return true;};
    bool hasFocus() const override {return focused;};
    void setFocus(bool value) override;

    int getSelectedTab() const {return selectedTab;};
    void setSelectedTab(int index);

    unsigned int getTabCount() const {return tabs.size();};
    const std::string& getTab(unsigned int index) const;

    void setOnTabChanged(std::function<void(int)> callback) {onTabChanged = callback;};

    private:
    int pointerTabIndex() const;

    std::vector<std::string> tabs;
    int selectedTab = 0;
    bool focused = false;
    std::function<void(int)> onTabChanged = nullptr;
};


//******************************** Menus *********************************

class item_menu_bar : public item{
    public:
    item_menu_bar(
        STRUCT_pos pos = STRUCT_pos{0, 0, DWIDTH, 24},
        std::vector<std::string> menus = std::vector<std::string>{"File", "Edit", "View"},
        int selectedMenu = 0,
        int zOrder = 100
    );

    ~item_menu_bar() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;

    bool capturesKeyboard() const override {return focused;};
    bool isFocusable() const override {return true;};
    bool hasFocus() const override {return focused;};
    void setFocus(bool value) override;

    int getSelectedMenu() const {return selectedMenu;};
    void setSelectedMenu(int index);

    unsigned int getMenuCount() const {return menus.size();};
    const std::string& getMenu(unsigned int index) const;

    void setCallback(unsigned int index, std::function<void()> callback);

    private:
    void invokeSelected();
    int pointerMenuIndex() const;

    std::vector<std::string> menus;
    std::vector<std::function<void()>> callbacks;
    int selectedMenu = 0;
    bool focused = false;
};

class item_softkey_bar : public item{
    public:
    item_softkey_bar(
        std::vector<std::string> labels = std::vector<std::string>{"", "", "", "", "", ""},
        int height = 24,
        int zOrder = 1000
    );

    ~item_softkey_bar() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;

    bool wantsGlobalKeyboard() const override {return true;};
    void handleGlobalEvent(int eventType, int eventKey) override;

    void setLabel(unsigned int index, const std::string& label);
    const std::string& getLabel(unsigned int index) const;

    void setCallback(unsigned int index, std::function<void()> callback);
    bool trigger(unsigned int index);

    private:
    int keyToIndex(int key) const;
    int pointerToIndex() const;

    std::vector<std::string> labels;
    std::vector<std::function<void()>> callbacks;
};

using item_bottom_menu = item_softkey_bar;


//******************************** Graph *********************************

enum class graph_scale{
    LINEAR,
    LOG10
};

typedef struct graph_axis{
    double minValue;
    double maxValue;
    graph_scale scale;
    int divisions;
    bool showAxis;
    bool showGrid;
    std::string label;
    bool showGraduations;
    int graduationPrecision;

    graph_axis(
        double _minValue = -10.0,
        double _maxValue = 10.0,
        graph_scale _scale = graph_scale::LINEAR,
        int _divisions = 5,
        bool _showAxis = true,
        bool _showGrid = true,
        std::string _label = "",
        bool _showGraduations = true,
        int _graduationPrecision = 3)
        : minValue(_minValue),
          maxValue(_maxValue),
          scale(_scale),
          divisions(_divisions),
          showAxis(_showAxis),
          showGrid(_showGrid),
          label(_label),
          showGraduations(_showGraduations),
          graduationPrecision(_graduationPrecision)
    {
    }
}graph_axis;

typedef struct graph_point{
    double x;
    double y;

    graph_point(double _x = 0.0, double _y = 0.0)
        : x(_x), y(_y)
    {
    }
}graph_point;

typedef struct graph_series{
    std::vector<graph_point> points;
    Class_color color;
    bool connected;
    bool showPoints;
    bool visible;

    graph_series(
        Class_color _color = Class_color(0, 0, 0),
        bool _connected = true,
        bool _showPoints = false,
        bool _visible = true)
        : color(_color),
          connected(_connected),
          showPoints(_showPoints),
          visible(_visible)
    {
    }
}graph_series;

enum class graph_cursor_style{
    CROSSHAIR,
    VERTICAL,
    HORIZONTAL
};

typedef struct graph_cursor{
    double x;
    double y;
    Class_color color;
    bool visible;
    bool showValues;
    std::string label;
    graph_cursor_style style;

    graph_cursor(
        double _x = 0.0,
        double _y = 0.0,
        Class_color _color = Class_color(255, 0, 0),
        bool _visible = true,
        bool _showValues = true,
        std::string _label = "C",
        graph_cursor_style _style = graph_cursor_style::CROSSHAIR)
        : x(_x),
          y(_y),
          color(_color),
          visible(_visible),
          showValues(_showValues),
          label(_label),
          style(_style)
    {
    }
}graph_cursor;

class item_graph : public item{
    public:
    item_graph(
        STRUCT_pos pos = STRUCT_pos{0, 0, 240, 140},
        graph_axis xAxis = graph_axis{-10.0, 10.0, graph_scale::LINEAR, 5, true, true, "X"},
        graph_axis yAxis = graph_axis{-10.0, 10.0, graph_scale::LINEAR, 5, true, true, "Y"},
        int zOrder = 0
    );

    ~item_graph() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;

    bool capturesKeyboard() const override {return focused;};
    bool isFocusable() const override {return true;};
    bool hasFocus() const override {return focused;};
    void setFocus(bool value) override {focused = value;};

    int addSeries(
        Class_color color = Class_color(0, 0, 0),
        bool connected = true,
        bool showPoints = false);

    bool addPoint(unsigned int seriesIndex, double x, double y);
    bool clearSeries(unsigned int seriesIndex);
    void clearAllSeries();

    unsigned int getSeriesCount() const {return series.size();};
    graph_series* getSeries(unsigned int index);
    const graph_series* getSeries(unsigned int index) const;

    void setXAxis(const graph_axis& axis);
    void setYAxis(const graph_axis& axis);

    const graph_axis& getXAxis() const {return xAxis;};
    const graph_axis& getYAxis() const {return yAxis;};

    void setXRange(double minValue, double maxValue);
    void setYRange(double minValue, double maxValue);

    void setLogX(bool enabled);
    void setLogY(bool enabled);

    void setGridVisible(bool xGrid, bool yGrid)
    {
        xAxis.showGrid = xGrid;
        yAxis.showGrid = yGrid;
    };

    void setPanStep(double value) {panStep = (value > 0.0) ? value : 0.10;};
    void setZoomFactor(double value)
    {
        if(value > 0.05 && value < 1.0)
            zoomFactor = value;
    };

    void panX(int direction);
    void panY(int direction);
    void zoomIn();
    void zoomOut();
    void resetView();

    // Numeric graduations
    void setGraduationsVisible(bool xVisible, bool yVisible)
    {
        xAxis.showGraduations = xVisible;
        yAxis.showGraduations = yVisible;
    };

    void setGraduationPrecision(int xPrecision, int yPrecision)
    {
        xAxis.graduationPrecision = (xPrecision < 1) ? 1 : xPrecision;
        yAxis.graduationPrecision = (yPrecision < 1) ? 1 : yPrecision;
    };

    // Measurement cursors
    int addCursor(
        double x,
        double y,
        Class_color color = Class_color(255, 0, 0),
        std::string label = "C",
        graph_cursor_style style = graph_cursor_style::CROSSHAIR);

    bool removeCursor(unsigned int index);
    void clearCursors();
    graph_cursor* getCursor(unsigned int index);
    const graph_cursor* getCursor(unsigned int index) const;
    unsigned int getCursorCount() const {return cursors.size();};

    bool setActiveCursor(unsigned int index);
    int getActiveCursor() const {return activeCursor;};
    void setCursorControl(bool value) {cursorControl = value;};
    bool getCursorControl() const {return cursorControl;};
    void setCursorStep(double value) {cursorStep = (value > 0.0) ? value : 0.05;};
    void setShowCursorDelta(bool value) {showCursorDelta = value;};

    void setOnCursorMoved(
        std::function<void(int, double, double)> callback)
    {
        onCursorMoved = callback;
    };

    private:
    static bool normalizeAxis(graph_axis& axis);
    static double transformValue(double value, graph_scale scale, bool& valid);
    static double inverseValue(double value, graph_scale scale);

    bool mapX(double value, int& screenX) const;
    bool mapY(double value, int& screenY) const;

    int plotLeft() const;
    int plotRight() const;
    int plotTop() const;
    int plotBottom() const;

    void panAxis(graph_axis& axis, int direction);
    void zoomAxis(graph_axis& axis, double factor);
    void moveCursorAxis(double& value, const graph_axis& axis, int direction);
    void moveActiveCursor(int dx, int dy);
    double axisGraduationValue(const graph_axis& axis, int division) const;

    graph_axis xAxis;
    graph_axis yAxis;
    graph_axis initialXAxis;
    graph_axis initialYAxis;

    std::vector<graph_series> series;
    std::vector<graph_cursor> cursors;

    int activeCursor = -1;
    bool cursorControl = false;
    bool showCursorDelta = true;
    double cursorStep = 0.05;
    std::function<void(int, double, double)> onCursorMoved = nullptr;

    bool focused = false;
    double panStep = 0.10;
    double zoomFactor = 0.80;

    int leftMargin = 42;
    int rightMargin = 6;
    int topMargin = 6;
    int bottomMargin = 30;
};


//******************************** Table / spreadsheet *********************************






//******************************** Canvas *********************************

enum class canvas_command_type{
    PIXEL,
    LINE,
    RECTANGLE,
    CIRCLE,
    ELLIPSE,
    POLYLINE,
    POLYGON,
    TEXT
};

typedef struct canvas_command{
    int id = -1;
    canvas_command_type type = canvas_command_type::PIXEL;

    bool visible = true;
    int zOrder = 0;

    // Primary / secondary colors. Their meaning depends on the command:
    // line/text/pixel -> color
    // filled shapes -> fillColor + borderColor
    Class_color color = Class_color(0, 0, 0);
    Class_color fillColor = Class_color(255, 255, 255);
    Class_color borderColor = Class_color(0, 0, 0);

    int thickness = 1;
    int borderSize = 1;
    item_draw_mode drawMode = item_draw_mode::FILLED;

    STRUCT_point p1 = STRUCT_point{0, 0};
    STRUCT_point p2 = STRUCT_point{0, 0};
    STRUCT_pos rect = STRUCT_pos{0, 0, 0, 0};

    int radius = 0;
    int radiusX = 0;
    int radiusY = 0;

    std::vector<STRUCT_point> points;
    std::string text;
} canvas_command;


class item_canvas : public item{
    public:
    item_canvas(
        STRUCT_pos pos = STRUCT_pos{0, 0, 200, 120},
        bool drawBackground = true,
        bool drawBorder = true,
        int zOrder = 0
    );

    ~item_canvas() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;
    void applyTheme(const item_theme& theme) override;

    // Canvas commands use LOCAL coordinates:
    // (0,0) is the top-left corner of the Canvas.
    int addPixel(
        int x,
        int y,
        Class_color color,
        int zOrder = 0
    );

    int addLine(
        STRUCT_point p1,
        STRUCT_point p2,
        Class_color color,
        int thickness = 1,
        int zOrder = 0
    );

    int addRectangle(
        STRUCT_pos rect,
        Class_color fillColor = Class_color(255, 255, 255),
        Class_color borderColor = Class_color(0, 0, 0),
        item_draw_mode mode = item_draw_mode::FILLED,
        int borderSize = 1,
        int zOrder = 0
    );

    int addCircle(
        STRUCT_point center,
        int radius,
        Class_color fillColor = Class_color(255, 255, 255),
        Class_color borderColor = Class_color(0, 0, 0),
        item_draw_mode mode = item_draw_mode::FILLED,
        int borderSize = 1,
        int zOrder = 0
    );

    int addEllipse(
        STRUCT_point center,
        int radiusX,
        int radiusY,
        Class_color fillColor = Class_color(255, 255, 255),
        Class_color borderColor = Class_color(0, 0, 0),
        item_draw_mode mode = item_draw_mode::FILLED,
        int borderSize = 1,
        int zOrder = 0
    );

    int addPolyline(
        const std::vector<STRUCT_point>& points,
        Class_color color,
        int thickness = 1,
        int zOrder = 0
    );

    int addPolygon(
        const std::vector<STRUCT_point>& points,
        Class_color fillColor = Class_color(255, 255, 255),
        Class_color borderColor = Class_color(0, 0, 0),
        item_draw_mode mode = item_draw_mode::FILLED,
        int borderSize = 1,
        int zOrder = 0
    );

    // Convenience primitives generated as retained polylines/polygons.
    int addArc(
        STRUCT_point center,
        int radius,
        float startAngleDegrees,
        float endAngleDegrees,
        Class_color color,
        int thickness = 1,
        unsigned int segments = 32,
        int zOrder = 0
    );

    int addSector(
        STRUCT_point center,
        int radius,
        float startAngleDegrees,
        float endAngleDegrees,
        Class_color fillColor = Class_color(255, 255, 255),
        Class_color borderColor = Class_color(0, 0, 0),
        int borderSize = 1,
        unsigned int segments = 32,
        int zOrder = 0
    );

    int addText(
        STRUCT_point position,
        const std::string& text,
        Class_color color = Class_color(0, 0, 0),
        int zOrder = 0
    );

    bool removeCommand(int commandId);
    void clearCommands();

    canvas_command* getCommand(int commandId);
    const canvas_command* getCommand(int commandId) const;

    const std::vector<canvas_command>& getCommands() const {return commands;};

    bool setCommandVisible(int commandId, bool visible);
    bool setCommandZOrder(int commandId, int zOrder);

    unsigned int getCommandCount() const {return commands.size();};

    // Reserve retained-command storage to avoid reallocation when building a
    // large drawing.
    void reserveCommands(unsigned int count)
    {
        commands.reserve(count);
    };

    void setBackgroundEnabled(bool value) {backgroundEnabled = value;};
    bool getBackgroundEnabled() const {return backgroundEnabled;};

    void setBorderEnabled(bool value) {borderEnabled = value;};
    bool getBorderEnabled() const {return borderEnabled;};

    void setBackgroundColor(Class_color value) {backgroundColor = value;};
    Class_color getBackgroundColor() const {return backgroundColor;};

    void setCanvasBorderColor(Class_color value) {canvasBorderColor = value;};
    Class_color getCanvasBorderColor() const {return canvasBorderColor;};

    void setCanvasBorderSize(int value) {canvasBorderSize = (value > 0) ? value : 1;};
    int getCanvasBorderSize() const {return canvasBorderSize;};

    STRUCT_point localToScreen(STRUCT_point local) const
    {
        return STRUCT_point{
            getX() + local.x,
            getY() + local.y
        };
    };

    STRUCT_point screenToLocal(STRUCT_point screen) const
    {
        return STRUCT_point{
            screen.x - getX(),
            screen.y - getY()
        };
    };

    void setOnCanvasClick(
        std::function<void(int, int)> callback)
    {
        onCanvasClick = callback;
    };

    private:
    int appendCommand(canvas_command command);
    void sortCommands();
    void ensurePolygonScratch(unsigned int count);

    int resolvedColor(const Class_color& value) const;

    void drawCommand(canvas_command& command);
    void drawPolygonCommand(
        canvas_command& command,
        bool closePath
    );

    int nextCommandId = 1;

    std::vector<canvas_command> commands;
    std::vector<int> polygonScratch;

    bool backgroundEnabled = true;
    bool borderEnabled = true;

    int canvasBorderSize = 1;

    Class_color backgroundColor = Class_color(255, 255, 255);
    Class_color canvasBorderColor = Class_color(0, 0, 0);

    std::function<void(int, int)> onCanvasClick = nullptr;
};


//******************************** Lightweight utility controls *********************************


enum class item_text_align{
    LEFT,
    CENTER,
    RIGHT
};

class item_text_label : public item{
    public:
    item_text_label(
        STRUCT_pos pos = STRUCT_pos{0, 0, 120, 20},
        std::string text = "Label",
        item_text_align align = item_text_align::LEFT,
        bool opaque = false,
        int zOrder = 0
    );

    ~item_text_label() override = default;

    void draw() override;
    void applyTheme(const item_theme& theme) override;

    const std::string& getText() const {return text;};
    void setText(const std::string& value) {text = value;};

    item_text_align getAlignment() const {return alignment;};
    void setAlignment(item_text_align value) {alignment = value;};

    bool isOpaque() const {return opaque;};
    void setOpaque(bool value) {opaque = value;};

    void setTextColor(Class_color value) {textColor = value;};
    void setBackgroundColor(Class_color value) {backgroundColor = value;};

    private:
    std::string text;
    item_text_align alignment = item_text_align::LEFT;
    bool opaque = false;

    Class_color textColor = Class_color(0, 0, 0);
    Class_color backgroundColor = Class_color(255, 255, 255);
};


class item_separator : public item{
    public:
    item_separator(
        STRUCT_pos pos = STRUCT_pos{0, 0, 120, 1},
        item_orientation orientation = item_orientation::HORIZONTAL,
        int thickness = 1,
        int zOrder = 0
    );

    ~item_separator() override = default;

    void draw() override;
    void applyTheme(const item_theme& theme) override;

    item_orientation getOrientation() const {return orientation;};
    void setOrientation(item_orientation value) {orientation = value;};

    int getThickness() const {return thickness;};
    void setThickness(int value) {thickness = (value > 0) ? value : 1;};

    void setColor(Class_color value) {color = value;};

    private:
    item_orientation orientation = item_orientation::HORIZONTAL;
    int thickness = 1;
    Class_color color = Class_color(80, 80, 80);
};


typedef struct toolbar_action{
    int id = -1;
    std::string label;
    int shortcutKey = 0;
    bool enabled = true;
    std::function<void()> callback = nullptr;

    toolbar_action() = default;

    toolbar_action(
        int _id,
        const std::string& _label,
        int _shortcutKey,
        bool _enabled,
        std::function<void()> _callback)
        : id(_id),
          label(_label),
          shortcutKey(_shortcutKey),
          enabled(_enabled),
          callback(_callback)
    {
    }
} toolbar_action;


class item_toolbar : public item{
    public:
    item_toolbar(
        STRUCT_pos pos = STRUCT_pos{0, 0, DWIDTH, 28},
        int zOrder = 0
    );

    ~item_toolbar() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;
    void handleGlobalEvent(int eventType, int eventKey) override;
    bool wantsGlobalKeyboard() const override {return true;};
    void applyTheme(const item_theme& theme) override;

    int addAction(
        const std::string& label,
        std::function<void()> callback = nullptr,
        int shortcutKey = 0,
        bool enabled = true
    );

    bool removeAction(int id);
    void clearActions();

    bool setActionEnabled(int id, bool enabled);
    bool setActionLabel(int id, const std::string& label);
    bool setActionShortcut(int id, int shortcutKey);
    bool setActionCallback(int id, std::function<void()> callback);

    const std::vector<toolbar_action>& getActions() const {return actions;};

    private:
    int pointerActionIndex() const;
    int actionIndexById(int id) const;
    void triggerAction(int index);

    int nextActionId = 1;
    std::vector<toolbar_action> actions;

    Class_color backgroundColor = Class_color(245, 245, 245);
    Class_color borderColor = Class_color(70, 70, 70);
    Class_color textColor = Class_color(0, 0, 0);
    Class_color disabledColor = Class_color(150, 150, 150);
    Class_color hoverColor = Class_color(220, 230, 250);
};


class item_status_bar : public item{
    public:
    item_status_bar(
        STRUCT_pos pos = STRUCT_pos{0, DHEIGHT - 22, DWIDTH, 22},
        std::string leftText = "",
        std::string centerText = "",
        std::string rightText = "",
        int zOrder = 0
    );

    ~item_status_bar() override = default;

    void draw() override;
    void applyTheme(const item_theme& theme) override;

    void setLeftText(const std::string& value) {leftText = value;};
    void setCenterText(const std::string& value) {centerText = value;};
    void setRightText(const std::string& value) {rightText = value;};

    const std::string& getLeftText() const {return leftText;};
    const std::string& getCenterText() const {return centerText;};
    const std::string& getRightText() const {return rightText;};

    void setStatus(
        item_led_state value,
        bool showIndicator = true
    )
    {
        status = value;
        indicatorVisible = showIndicator;
    };

    item_led_state getStatus() const {return status;};

    private:
    std::string leftText;
    std::string centerText;
    std::string rightText;

    item_led_state status = item_led_state::OFF;
    bool indicatorVisible = false;

    Class_color backgroundColor = Class_color(245, 245, 245);
    Class_color borderColor = Class_color(80, 80, 80);
    Class_color textColor = Class_color(0, 0, 0);
};


//******************************** Containers / layouts *********************************


typedef struct container_child{
    int id = -1;
    item* control = nullptr;

    // Relative geometry used by ABSOLUTE layout. For automatic layouts, w/h
    // are also used as preferred sizes.
    STRUCT_pos local = STRUCT_pos{0, 0, 40, 20};

    item_margins margins{};
    int anchors =
        ITEM_ANCHOR_LEFT |
        ITEM_ANCHOR_TOP;

    bool visible = true;

    // Optional logical group. TabView uses it as tab index.
    int group = -1;

    int referenceContentW = 0;
    int referenceContentH = 0;
} container_child;


class item_container : public item{
    public:
    item_container(
        STRUCT_pos pos = STRUCT_pos{0, 0, 200, 140},
        item_layout_mode layout = item_layout_mode::ABSOLUTE,
        int padding = 6,
        int gap = 4,
        int zOrder = 0
    );

    ~item_container() override = default;

    void draw() override;
    void update(uint32_t nowTicks) override;
    void applyTheme(const item_theme& theme) override;
    bool isHover(int x, int y) override;
    void handleEvent(int eventType, int eventKey) override;
    bool handleSystemExit() override;

    // A plain Container only needs top-level focus when an active child can
    // actually receive keyboard focus.
    bool isFocusable() const override;
    bool wantsPointerFocus(int x, int y) const override;
    bool capturesKeyboard() const override {return focused;};
    bool hasFocus() const override {return focused;};
    void setFocus(bool value) override;

    int addChild(
        item* control,
        STRUCT_pos localGeometry = STRUCT_pos{0, 0, 40, 20},
        item_margins margins = item_margins{},
        int anchors =
            ITEM_ANCHOR_LEFT |
            ITEM_ANCHOR_TOP
    );

    bool removeChild(int childId);
    void clearChildren();

    item* getChild(int childId);
    const item* getChild(int childId) const;

    unsigned int getChildCount() const {return children.size();};

    bool setChildGeometry(
        int childId,
        STRUCT_pos localGeometry
    );

    bool setChildMargins(
        int childId,
        item_margins margins
    );

    bool setChildAnchors(
        int childId,
        int anchors
    );

    bool setChildVisible(
        int childId,
        bool visible
    );

    bool setChildGroup(
        int childId,
        int group
    );

    int getFocusedChildId() const {return focusedChildId;};
    bool setChildFocus(int childId);
    void clearChildFocus();
    bool focusNextChild();
    bool focusPreviousChild();

    item_layout_mode getLayoutMode() const {return layoutMode;};
    void setLayoutMode(item_layout_mode value);

    int getPadding() const {return padding;};
    void setPadding(int value);

    int getGap() const {return gap;};
    void setGap(int value);

    int getGridColumns() const {return gridColumns;};
    void setGridColumns(int value);

    const std::string& getContainerTitle() const {return title;};
    void setContainerTitle(const std::string& value);

    void setContainerColors(
        Class_color background,
        Class_color border,
        Class_color titleColor = Class_color(0, 0, 0)
    );

    // Force a layout immediately. Normally draw/update/events do it
    // automatically.
    void performLayout();

    protected:
    virtual STRUCT_pos getContentRect() const;
    virtual void drawChrome();
    virtual bool childIsActive(const container_child& child) const;

    container_child* getChildLink(int childId);
    const container_child* getChildLink(int childId) const;

    item* getTopChildAt(int x, int y);
    const item* getTopChildAt(int x, int y) const;
    item* getFocusedChild();

    void drawChildren();
    void updateChildren(uint32_t nowTicks);
    void clearChildHover();

    void setScrollOffsetInternal(int x, int y);
    int getScrollXInternal() const {return scrollX;};
    int getScrollYInternal() const {return scrollY;};

    std::vector<container_child> children;

    // Reused transient lists. These vectors grow with child count but are not
    // recreated by performLayout()/drawChildren() every frame.
    std::vector<container_child*> layoutScratch;
    std::vector<container_child*> drawScratch;

    int nextChildId = 1;
    int focusedChildId = -1;

    item_layout_mode layoutMode = item_layout_mode::ABSOLUTE;

    int padding = 6;
    int gap = 4;
    int gridColumns = 2;

    int scrollX = 0;
    int scrollY = 0;

    bool focused = false;

    std::string title;

    Class_color backgroundColor = Class_color(250, 250, 250);
    Class_color borderColor = Class_color(0, 0, 0);
    Class_color titleColor = Class_color(0, 0, 0);
};

using item_panel = item_container;
using item_group_box = item_container;


class item_scroll_view : public item_container{
    public:
    item_scroll_view(
        STRUCT_pos pos = STRUCT_pos{0, 0, 200, 140},
        item_layout_mode layout = item_layout_mode::ABSOLUTE,
        int padding = 6,
        int gap = 4,
        bool showScrollbars = true,
        int zOrder = 0
    );

    ~item_scroll_view() override = default;

    void draw() override;
    bool isHover(int x, int y) override;
    void handleEvent(int eventType, int eventKey) override;

    bool isFocusable() const override {return true;};
    bool wantsPointerFocus(int x, int y) const override;

    void setScroll(int x, int y);
    void scrollBy(int dx, int dy);

    int getScrollX() const {return getScrollXInternal();};
    int getScrollY() const {return getScrollYInternal();};

    int getContentWidth();
    int getContentHeight();

    void setScrollStep(int value) {scrollStep = (value > 0) ? value : 1;};
    int getScrollStep() const {return scrollStep;};

    void setShowScrollbars(bool value) {showScrollbars = value;};
    bool getShowScrollbars() const {return showScrollbars;};

    bool scrollToChild(int childId);

    protected:
    void drawScrollbars();

    private:
    int maxScrollX();
    int maxScrollY();
    void clampScroll();

    bool pointerOnVerticalScrollbar() const;
    bool pointerOnHorizontalScrollbar() const;

    int scrollStep = 16;
    bool showScrollbars = true;
};


class item_tab_view : public item_container{
    public:
    item_tab_view(
        STRUCT_pos pos = STRUCT_pos{0, 0, 240, 150},
        std::vector<std::string> tabs =
            std::vector<std::string>{"Tab 1", "Tab 2"},
        int selectedTab = 0,
        int headerHeight = 24,
        int padding = 6,
        int zOrder = 0
    );

    ~item_tab_view() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;
    void setFocus(bool value) override;

    bool isFocusable() const override {return true;};
    bool wantsPointerFocus(int x, int y) const override;

    int addChildToTab(
        int tabIndex,
        item* control,
        STRUCT_pos localGeometry = STRUCT_pos{0, 0, 40, 20},
        item_margins margins = item_margins{},
        int anchors =
            ITEM_ANCHOR_LEFT |
            ITEM_ANCHOR_TOP
    );

    int addSharedChild(
        item* control,
        STRUCT_pos localGeometry = STRUCT_pos{0, 0, 40, 20},
        item_margins margins = item_margins{},
        int anchors =
            ITEM_ANCHOR_LEFT |
            ITEM_ANCHOR_TOP
    );

    int addTab(const std::string& label);
    bool removeTab(int index);

    int getSelectedTab() const {return selectedTab;};
    bool setSelectedTab(int index, bool notify = true);

    unsigned int getTabCount() const {return tabs.size();};
    const std::string& getTab(unsigned int index) const;
    const std::vector<std::string>& getTabs() const {return tabs;};
    void setTabs(const std::vector<std::string>& value);

    int getHeaderHeight() const {return headerHeight;};
    void setHeaderHeight(int value);

    void setOnTabChanged(
        std::function<void(int)> callback)
    {
        onTabChanged = callback;
    };

    protected:
    STRUCT_pos getContentRect() const override;
    void drawChrome() override;
    bool childIsActive(const container_child& child) const override;

    private:
    int pointerTabIndex() const;

    std::vector<std::string> tabs;
    int selectedTab = 0;
    int headerHeight = 24;
    bool headerFocused = false;

    std::function<void(int)> onTabChanged = nullptr;
};

using item_tab_pages = item_tab_view;


//******************************** Selection controls *********************************


class item_radio_button : public item{
    public:
    item_radio_button(
        STRUCT_pos pos = STRUCT_pos{0, 0, 120, 24},
        std::string label = "Radio",
        bool checked = false,
        int zOrder = 0
    );

    ~item_radio_button() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;

    // Keyboard selection belongs to RadioGroup. A standalone RadioButton
    // behaves like a one-shot selector and never captures the cursor keys.
    bool isFocusable() const override {return false;};
    bool capturesKeyboard() const override {return false;};
    bool hasFocus() const override {return false;};
    void setFocus(bool value) override {(void)value;};

    bool isChecked() const {return checked;};
    void setChecked(bool value, bool notify = true);

    void setAllowUncheck(bool value) {allowUncheck = value;};
    bool getAllowUncheck() const {return allowUncheck;};

    void setLabel(const std::string& value) {label = value;};
    const std::string& getRadioLabel() const {return label;};

    void setOnChanged(std::function<void(bool)> callback) {onChanged = callback;};

    private:
    bool checked = false;
    bool allowUncheck = false;
    std::string label;
    std::function<void(bool)> onChanged = nullptr;
};


class item_radio_group : public item{
    public:
    item_radio_group(
        STRUCT_pos pos = STRUCT_pos{0, 0, 150, 100},
        std::vector<std::string> labels = std::vector<std::string>{},
        int selectedIndex = 0,
        int rowHeight = 24,
        int zOrder = 0
    );

    ~item_radio_group() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;

    bool isFocusable() const override {return true;};
    bool capturesKeyboard() const override {return focused;};
    bool hasFocus() const override {return focused;};
    void setFocus(bool value) override {focused = value;};

    void setItems(const std::vector<std::string>& value);
    const std::vector<std::string>& getItems() const {return labels;};

    int addItem(const std::string& label);
    bool removeItem(unsigned int index);
    void clearItems();

    int getSelectedIndex() const {return selectedIndex;};
    bool setSelectedIndex(int index, bool notify = true);

    std::string getSelectedText() const;

    int getRowHeight() const {return rowHeight;};
    void setRowHeight(int value) {rowHeight = (value >= 18) ? value : 18;};

    void setOnChanged(
        std::function<void(int, const std::string&)> callback)
    {
        onChanged = callback;
    };

    private:
    int pointerIndex() const;

    std::vector<std::string> labels;
    int selectedIndex = -1;
    int rowHeight = 24;
    bool focused = false;

    std::function<void(int, const std::string&)> onChanged = nullptr;
};


class item_list_box : public item{
    public:
    item_list_box(
        STRUCT_pos pos = STRUCT_pos{0, 0, 160, 120},
        std::vector<std::string> items = std::vector<std::string>{},
        int selectedIndex = 0,
        int rowHeight = 20,
        bool showScrollbar = true,
        int zOrder = 0
    );

    ~item_list_box() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;
    void applyTheme(const item_theme& theme) override;

    bool isFocusable() const override {return true;};
    bool capturesKeyboard() const override {return focused;};
    bool hasFocus() const override {return focused;};
    void setFocus(bool value) override;

    void setItems(const std::vector<std::string>& value);
    const std::vector<std::string>& getItems() const {return items;};

    int addItem(const std::string& value);
    bool removeItem(unsigned int index);
    void clearItems();

    int getSelectedIndex() const {return selectedIndex;};
    bool setSelectedIndex(int index, bool notify = true);

    std::string getSelectedText() const;

    int getScrollOffset() const {return scrollOffset;};
    void setScrollOffset(int value);

    int getRowHeight() const {return rowHeight;};
    void setRowHeight(int value);

    bool getShowScrollbar() const {return showScrollbar;};
    void setShowScrollbar(bool value) {showScrollbar = value;};

    void setOnSelected(
        std::function<void(int, const std::string&)> callback)
    {
        onSelected = callback;
    };

    void setOnActivated(
        std::function<void(int, const std::string&)> callback)
    {
        onActivated = callback;
    };

    private:
    int rowCapacity() const;
    int maxScrollOffset() const;
    int pointerIndex() const;
    void ensureSelectionVisible();
    void moveSelection(int delta);
    void drawScrollbar();

    std::vector<std::string> items;
    int selectedIndex = -1;
    int scrollOffset = 0;
    int rowHeight = 20;
    bool showScrollbar = true;
    bool focused = false;
    bool focusJustAcquired = false;

    Class_color backgroundColor = Class_color(255, 255, 255);
    Class_color borderColor = Class_color(0, 0, 0);
    Class_color textColor = Class_color(0, 0, 0);
    Class_color selectionColor = Class_color(210, 225, 255);

    std::function<void(int, const std::string&)> onSelected = nullptr;
    std::function<void(int, const std::string&)> onActivated = nullptr;
};


class item_combo_box : public item{
    public:
    item_combo_box(
        STRUCT_pos pos = STRUCT_pos{0, 0, 150, 26},
        std::vector<std::string> items = std::vector<std::string>{},
        int selectedIndex = 0,
        int maxVisibleRows = 5,
        int rowHeight = 22,
        int zOrder = 0
    );

    ~item_combo_box() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;
    void applyTheme(const item_theme& theme) override;
    bool handleSystemExit() override;

    bool contains(int x, int y) const override;

    bool isFocusable() const override {return true;};
    bool capturesKeyboard() const override {return focused || opened;};
    bool hasFocus() const override {return focused;};
    void setFocus(bool value) override;

    void setItems(const std::vector<std::string>& value);
    const std::vector<std::string>& getItems() const {return items;};

    int addItem(const std::string& value);
    bool removeItem(unsigned int index);
    void clearItems();

    int getSelectedIndex() const {return selectedIndex;};
    bool setSelectedIndex(int index, bool notify = true);

    std::string getSelectedText() const;

    void open();
    void close();
    void toggle();
    bool isOpen() const {return opened;};

    void setMaxVisibleRows(int value) {maxVisibleRows = (value > 0) ? value : 1;};
    int getMaxVisibleRows() const {return maxVisibleRows;};

    void setOnChanged(
        std::function<void(int, const std::string&)> callback)
    {
        onChanged = callback;
    };

    private:
    int dropdownRowCount() const;
    int dropdownHeight() const;
    int pointerDropdownIndex() const;
    void ensureHighlightVisible();
    void moveHighlight(int delta);

    std::vector<std::string> items;

    int selectedIndex = -1;
    int highlightedIndex = -1;
    int scrollOffset = 0;

    int maxVisibleRows = 5;
    int rowHeight = 22;

    bool opened = false;
    bool focused = false;
    bool focusJustAcquired = false;

    Class_color backgroundColor = Class_color(255, 255, 255);
    Class_color borderColor = Class_color(0, 0, 0);
    Class_color textColor = Class_color(0, 0, 0);
    Class_color selectionColor = Class_color(210, 225, 255);

    std::function<void(int, const std::string&)> onChanged = nullptr;
};


enum class option_popup_type{
    CHECKBOX,
    TOGGLE,
    RADIO
};

typedef struct option_popup_entry{
    int id = -1;
    option_popup_type type = option_popup_type::CHECKBOX;
    std::string label;
    bool value = false;
    int radioGroup = 0;

    option_popup_entry() = default;

    option_popup_entry(
        int _id,
        option_popup_type _type,
        const std::string& _label,
        bool _value = false,
        int _radioGroup = 0)
        : id(_id),
          type(_type),
          label(_label),
          value(_value),
          radioGroup(_radioGroup)
    {
    }
} option_popup_entry;


//******************************** Tree *********************************


typedef struct tree_node{
    int id = -1;
    int parentId = -1;
    std::string label;
    bool expanded = false;
    bool enabled = true;
    bool visible = true;
    const bopti_image_t* icon = nullptr;
    std::vector<int> children;

    tree_node() = default;

    tree_node(
        int _id,
        int _parentId,
        const std::string& _label,
        bool _expanded = false,
        bool _enabled = true,
        bool _visible = true,
        const bopti_image_t* _icon = nullptr)
        : id(_id),
          parentId(_parentId),
          label(_label),
          expanded(_expanded),
          enabled(_enabled),
          visible(_visible),
          icon(_icon)
    {
    }
} tree_node;


class item_tree : public item{
    public:
    item_tree(
        STRUCT_pos pos = STRUCT_pos{0, 0, 180, 140},
        int rowHeight = 18,
        int indentWidth = 14,
        bool showLines = true,
        bool showScrollbar = true,
        int zOrder = 0
    );

    ~item_tree() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;

    bool isFocusable() const override {return true;};
    bool capturesKeyboard() const override {return focused;};
    bool hasFocus() const override {return focused;};
    void setFocus(bool value) override;

    int addNode(
        int parentId,
        const std::string& label,
        bool expanded = false,
        const bopti_image_t* icon = nullptr
    );

    int addRoot(
        const std::string& label,
        bool expanded = false,
        const bopti_image_t* icon = nullptr
    )
    {
        return addNode(
            -1,
            label,
            expanded,
            icon
        );
    }

    bool addNodeWithId(
        int id,
        int parentId,
        const std::string& label,
        bool expanded = false,
        const bopti_image_t* icon = nullptr
    );

    bool removeNode(
        int id,
        bool recursive = true
    );

    void clear();

    tree_node* getNode(int id);
    const tree_node* getNode(int id) const;

    bool hasNode(int id) const;

    unsigned int getNodeCount() const {return nodes.size();};

    int getSelectedNodeId() const {return selectedNodeId;};
    bool setSelectedNode(int id);

    // Arrow keys move the navigation selection; EXE confirms/activates it.
    int getActivatedNodeId() const {return activatedNodeId;};
    void clearActivatedNode() {activatedNodeId = -1;};

    bool setNodeExpanded(int id, bool expanded);
    bool toggleNode(int id);

    bool setNodeLabel(
        int id,
        const std::string& label
    );

    bool setNodeVisible(int id, bool visible);
    bool setNodeEnabled(int id, bool enabled);
    bool setNodeIcon(int id, const bopti_image_t* icon);

    int getRowHeight() const {return rowHeight;};
    void setRowHeight(int value);

    int getIndentWidth() const {return indentWidth;};
    void setIndentWidth(int value);

    bool getShowLines() const {return showLines;};
    void setShowLines(bool value) {showLines = value;};

    bool getShowScrollbar() const {return showScrollbar;};
    void setShowScrollbar(bool value) {showScrollbar = value;};

    int getScrollOffset() const {return scrollOffset;};
    void setScrollOffset(int value);

    std::vector<int> getVisibleNodeIds() const;
    std::vector<int> getAllNodeIds() const;

    void setOnSelected(
        std::function<void(int, const std::string&)> callback)
    {
        onSelected = callback;
    };

    void setOnExpanded(
        std::function<void(int, bool)> callback)
    {
        onExpanded = callback;
    };

    void setOnActivated(
        std::function<void(int, const std::string&)> callback)
    {
        onActivated = callback;
    };

    private:
    void appendVisibleNodes(
        int id,
        std::vector<int>& output
    ) const;

    void invalidateVisibleCache();
    const std::vector<int>& visibleNodeIds() const;

    int getDepth(int id) const;
    int getVisibleIndex(int id) const;
    int getNodeAtPointer() const;
    bool pointerOnExpander(int id) const;

    int visibleRowCapacity() const;
    int maxScrollOffset() const;

    void ensureSelectedVisible();
    void selectRelative(int delta);
    void selectParentOrCollapse();
    void expandOrSelectChild();
    void activateSelected();

    void removeNodeRecursive(int id);
    void removeFromParent(int id);

    void drawRow(
        int id,
        int rowIndex
    );

    void drawScrollbar();

    std::map<int, tree_node> nodes;
    std::vector<int> roots;

    mutable std::vector<int> visibleNodeCache;
    mutable bool visibleNodeCacheDirty = true;

    int nextNodeId = 1;
    int selectedNodeId = -1;
    int activatedNodeId = -1;
    int scrollOffset = 0;

    int rowHeight = 18;
    int indentWidth = 14;

    bool showLines = true;
    bool showScrollbar = true;
    bool focused = false;
    bool focusJustAcquired = false;

    Class_color backgroundColor = Class_color(255, 255, 255);
    Class_color borderColor = Class_color(0, 0, 0);
    Class_color textColor = Class_color(0, 0, 0);
    Class_color lineColor = Class_color(140, 140, 140);
    Class_color selectionColor = Class_color(210, 225, 255);
    Class_color disabledTextColor = Class_color(140, 140, 140);

    std::function<void(int, const std::string&)> onSelected = nullptr;
    std::function<void(int, bool)> onExpanded = nullptr;
    std::function<void(int, const std::string&)> onActivated = nullptr;
};


class item_table : public item{
    public:
    item_table(
        STRUCT_pos pos = STRUCT_pos{0, 0, 240, 120},
        unsigned int rows = 5,
        unsigned int columns = 4,
        int cellWidth = 60,
        int cellHeight = 20,
        bool showHeaders = true,
        int zOrder = 0
    );

    ~item_table() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;

    bool capturesKeyboard() const override {return focused;};
    bool isFocusable() const override {return true;};
    bool hasFocus() const override {return focused;};
    void setFocus(bool value) override
    {
        focused = value;
        if(!focused)
            editing = false;
    };

    void resize(unsigned int rows, unsigned int columns);

    unsigned int getRowCount() const {return data.size();};
    unsigned int getColumnCount() const
    {
        return data.empty() ? 0 : data[0].size();
    };

    bool setCell(unsigned int row, unsigned int column, const std::string& value);
    const std::string& getCell(unsigned int row, unsigned int column) const;
    void clearCell(unsigned int row, unsigned int column);
    void clearAll();

    void setColumnLabel(unsigned int column, const std::string& label);
    const std::string& getColumnLabel(unsigned int column) const;

    void setShowHeaders(bool value) {showHeaders = value;};
    bool getShowHeaders() const {return showHeaders;};

    void setCellSize(int width, int height);
    int getCellWidth() const {return cellWidth;};
    int getCellHeight() const {return cellHeight;};

    void setSelectedCell(unsigned int row, unsigned int column);
    unsigned int getSelectedRow() const {return selectedRow;};
    unsigned int getSelectedColumn() const {return selectedColumn;};

    bool isEditing() const {return editing;};

    void setOnCellChanged(
        std::function<void(unsigned int, unsigned int, const std::string&)> callback)
    {
        onCellChanged = callback;
    };

    void setOnSelectionChanged(
        std::function<void(unsigned int, unsigned int)> callback)
    {
        onSelectionChanged = callback;
    };

    private:
    char keyToChar(int key) const;
    void ensureSelectionVisible();
    void selectFromPointer();
    void notifyCellChanged();
    void notifySelectionChanged();

    unsigned int visibleRows() const;
    unsigned int visibleColumns() const;

    std::vector<std::vector<std::string>> data;
    std::vector<std::string> columnLabels;

    unsigned int selectedRow = 0;
    unsigned int selectedColumn = 0;
    unsigned int rowOffset = 0;
    unsigned int columnOffset = 0;

    int cellWidth = 60;
    int cellHeight = 20;
    int headerHeight = 18;
    int rowHeaderWidth = 28;

    bool showHeaders = true;
    bool focused = false;
    bool editing = false;

    std::function<void(unsigned int, unsigned int, const std::string&)> onCellChanged = nullptr;
    std::function<void(unsigned int, unsigned int)> onSelectionChanged = nullptr;
};

using item_spreadsheet = item_table;


//******************************** Image controls *********************************

class item_image : public item{
    public:
    item_image(
        STRUCT_pos pos = STRUCT_pos{0, 0, 32, 32},
        bopti_image_t const* image = nullptr,
        int sourceX = 0,
        int sourceY = 0,
        bool drawBorder = false,
        Class_color borderColor = Class_color(0, 0, 0),
        int zOrder = 0,
        item_image_mode mode = item_image_mode::ORIGINAL
    );

    ~item_image() override;

    void draw() override;
    void applyTheme(const item_theme& theme) override;

    void setImage(bopti_image_t const* value);
    bopti_image_t const* getImage() const {return image;};

    void setSource(int x, int y);
    int getSourceX() const {return sourceX;};
    int getSourceY() const {return sourceY;};

    void setImageMode(item_image_mode value);
    item_image_mode getImageMode() const {return imageMode;};

    void setMirrorX(bool value) {mirrorX = value;};
    void setMirrorY(bool value) {mirrorY = value;};
    void setMirror(bool x, bool y) {mirrorX = x; mirrorY = y;};
    bool getMirrorX() const {return mirrorX;};
    bool getMirrorY() const {return mirrorY;};

    // Public API uses degrees. The gint transform is generated internally.
    void setRotation(float degrees);
    float getRotation() const {return rotationDegrees;};

    void setDrawBorder(bool value) {drawBorder = value;};
    bool getDrawBorder() const {return drawBorder;};

    void setImageBorderColor(Class_color color)
    {
        borderColor = color;
        useThemeBorder = false;
    };

    Class_color getImageBorderColor() const {return borderColor;};

    void setUseThemeBorder(bool value)
    {
        useThemeBorder = value;

        if(useThemeBorder)
            borderColor = current_item_theme().border;
    };

    bool getUseThemeBorder() const {return useThemeBorder;};

    // Rebuild only when source/size/mode/rotation changes. Mirror is a
    // draw-time effect and does not invalidate the cache.
    void invalidateImageCache();

    protected:
    void rebuildImageCache();
    void clearImageCache();
    void drawImageWithMode();

    bopti_image_t const* image = nullptr;
    int sourceX = 0;
    int sourceY = 0;

    item_image_mode imageMode = item_image_mode::ORIGINAL;
    bool mirrorX = false;
    bool mirrorY = false;
    float rotationDegrees = 0.0f;

    bool drawBorder = false;
    Class_color borderColor = Class_color(0, 0, 0);
    bool useThemeBorder = true;

    image_t* transformedImage = nullptr;
    bool imageCacheDirty = true;

    int cacheItemW = -1;
    int cacheItemH = -1;
};

class item_image_button : public item_image{
    public:
    item_image_button(
        STRUCT_pos pos = STRUCT_pos{0, 0, 32, 32},
        bopti_image_t const* image = nullptr,
        std::function<void()> callback = nullptr,
        int sourceX = 0,
        int sourceY = 0,
        bool drawBorder = true,
        int zOrder = 0,
        item_image_mode mode = item_image_mode::ORIGINAL
    );

    ~item_image_button() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;

    void setCallback(std::function<void()> value) {callback = value;};

    private:
    std::function<void()> callback = nullptr;
};

using item_icon_button = item_image_button;


//******************************** Invisible time item *********************************

class item_time : public item{
    public:
    item_time(
        uint32_t durationMs = 1000,
        std::function<void()> callback = nullptr,
        bool repeat = false,
        bool autoStart = true
    );

    ~item_time() override = default;

    void draw() override {};
    bool contains(int x, int y) const override {(void)x; (void)y; return false;};
    bool isHover(int x, int y) override {(void)x; (void)y; return false;};
    void update(uint32_t nowTicks) override;

    void start();
    void pause();
    void stop();
    void reset();
    void restart();

    bool isRunning() const {return running;};
    bool isPaused() const {return paused;};
    bool isRepeat() const {return repeat;};

    void setRepeat(bool value) {repeat = value;};
    void setDurationMs(uint32_t value);
    uint32_t getDurationMs() const {return durationMs;};

    uint64_t getElapsedMs() const;
    uint64_t getRemainingMs() const;
    double getElapsedSeconds() const {return static_cast<double>(getElapsedMs()) / 1000.0;};

    void setCallback(std::function<void()> value) {callback = value;};
    void setUpdateCallback(std::function<void(uint64_t)> value) {updateCallback = value;};

    rtc_time_t getCurrentTime() const
    {
        rtc_time_t current{};
        rtc_get_time(&current);
        return current;
    };

    private:
    static uint32_t tickDelta(uint32_t nowTicks, uint32_t previousTicks);
    uint64_t durationTicks() const;

    uint32_t durationMs = 1000;
    uint64_t elapsedTicks = 0;
    uint32_t lastTicks = 0;

    bool running = false;
    bool paused = false;
    bool repeat = false;

    std::function<void()> callback = nullptr;
    std::function<void(uint64_t)> updateCallback = nullptr;
};

using item_timer = item_time;


//******************************** Popup *********************************

class item_popup : public item{
    public:
    item_popup(
        std::string title = "Popup",
        std::string message = "",
        int width = (DWIDTH * 2) / 3,
        int height = (DHEIGHT * 2) / 3,
        bool opened = false,
        int zOrder = 1000000
    );

    ~item_popup() override = default;

    void draw() override;
    bool contains(int x, int y) const override;
    bool isHover(int x, int y) override;
    void handleEvent(int eventType, int eventKey) override;
    bool handleSystemExit() override;
    bool isModal() const override {return opened;};
    bool capturesKeyboard() const override {return opened;};

    void open();
    void close();
    void toggle();
    bool isOpen() const {return opened;};

    void setTitle(const std::string& value) {title = value;};
    const std::string& getTitle() const {return title;};

    void setMessage(const std::string& value) {message = value;};
    const std::string& getMessage() const {return message;};

    void setOnClose(std::function<void()> value) {onClose = value;};

    private:
    bool pointerOnCloseButton() const;
    void centerPopup();

    std::string title;
    std::string message;
    bool opened = false;
    int titleHeight = 24;
    int closeButtonSize = 18;
    std::function<void()> onClose = nullptr;
};


// Prompt popup: editable string with a submit callback.
// Default character mapping intentionally mirrors item_textbox. A custom mapper
// can be supplied if the application wants its own alphabet/SHIFT policy.
class item_prompt_popup : public item_popup{
    public:
    item_prompt_popup(
        std::string title = "Input",
        std::string message = "Enter value:",
        std::string value = "",
        unsigned int maxLength = 32,
        std::function<void(const std::string&)> callback = nullptr,
        bool opened = false,
        int zOrder = 1000010
    );

    ~item_prompt_popup() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;

    const std::string& getValue() const {return value;};
    void setValue(const std::string& text);
    void clear() {value.clear();};

    void setMaxLength(unsigned int length) {maxLength = length > 0 ? length : 1;};
    void setOnSubmit(std::function<void(const std::string&)> value) {onSubmit = value;};
    void setKeyMapper(std::function<char(int)> value) {keyMapper = value;};

    void setExtendedKeyMapper(
        std::function<char(
            int,
            const item_text_input_state&
        )> value)
    {
        extendedKeyMapper = value;
    };

    const item_text_input_state& getInputState() const {return inputState;};
    void resetInputModifiers() {inputState.reset();};

    void setAlphaLock(bool value)
    {
        inputState.alpha =
            value
                ? item_alpha_mode::LOCKED
                : item_alpha_mode::OFF;
    };

    void submit();

    private:
    static char defaultKeyToChar(int key);

    std::string value;
    unsigned int maxLength = 32;
    std::function<void(const std::string&)> onSubmit = nullptr;
    std::function<char(int)> keyMapper = nullptr;

    std::function<char(
        int,
        const item_text_input_state&
    )> extendedKeyMapper = nullptr;

    item_text_input_state inputState;
};


// Choice/bool popup. By default it shows two buttons: Yes / No.
// For more than two buttons, use the index callback.
class item_bool_popup : public item_popup{
    public:
    item_bool_popup(
        std::string title = "Confirm",
        std::string message = "Continue?",
        std::vector<std::string> buttons = std::vector<std::string>{"Yes", "No"},
        std::function<void(int, const std::string&)> callback = nullptr,
        bool opened = false,
        int zOrder = 1000010
    );

    ~item_bool_popup() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;

    void setButtons(const std::vector<std::string>& value);
    const std::vector<std::string>& getButtons() const {return buttons;};

    void setSelectedIndex(int index);
    int getSelectedIndex() const {return selectedIndex;};

    void setOnChoice(std::function<void(int, const std::string&)> value) {onChoice = value;};
    void setOnBool(std::function<void(bool)> value) {onBool = value;};

    void choose(int index);

    private:
    int pointerButtonIndex() const;

    std::vector<std::string> buttons;
    int selectedIndex = 0;
    std::function<void(int, const std::string&)> onChoice = nullptr;
    std::function<void(bool)> onBool = nullptr;
};

using item_choice_popup = item_bool_popup;


// Numeric popup: decimal numeric entry with optional min/max clamp.
class item_numeric_popup : public item_popup{
    public:
    item_numeric_popup(
        std::string title = "Numeric",
        std::string message = "Enter number:",
        double value = 0.0,
        double minValue = -99999999.0,
        double maxValue = 99999999.0,
        int precision = 2,
        double step = 1.0,
        std::function<void(double)> callback = nullptr,
        bool opened = false,
        int zOrder = 1000010
    );

    ~item_numeric_popup() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;

    double getValue() const {return value;};
    void setValue(double value);
    void setRange(double minValue, double maxValue);
    void setPrecision(int value);
    void setStep(double value) {step = value > 0.0 ? value : 1.0;};

    void setOnSubmit(std::function<void(double)> value) {onSubmit = value;};
    void submit();

    private:
    void syncBuffer();
    bool parseBuffer(double& output) const;

    double value = 0.0;
    double minValue = -99999999.0;
    double maxValue = 99999999.0;
    int precision = 2;
    double step = 1.0;
    std::string buffer;
    std::function<void(double)> onSubmit = nullptr;
};


class item_options_popup : public item_popup{
    public:
    item_options_popup(
        std::string title = "Options",
        std::string message = "Configure options:",
        bool opened = false,
        int zOrder = 1000020
    );

    ~item_options_popup() override = default;

    void draw() override;
    void handleEvent(int eventType, int eventKey) override;
    bool handleSystemExit() override;

    void open();
    void apply();
    void cancel();

    int addCheckbox(
        const std::string& label,
        bool value = false
    );

    int addToggle(
        const std::string& label,
        bool value = false
    );

    int addRadio(
        const std::string& label,
        int radioGroup = 0,
        bool selected = false
    );

    bool removeOption(int id);
    void clearOptions();

    bool hasOption(int id) const;
    bool getValue(int id) const;
    bool setValue(int id, bool value);

    int getSelectedRadio(int radioGroup) const;

    const std::vector<option_popup_entry>& getOptions() const {return options;};

    void setApplyLabel(const std::string& value) {applyLabel = value;};
    void setCancelLabel(const std::string& value) {cancelLabel = value;};

    void setOnApply(std::function<void(item_options_popup&)> callback)
    {
        onApply = callback;
    };

    void setOnCancel(std::function<void()> callback)
    {
        onCancel = callback;
    };

    private:
    int optionIndexById(int id) const;
    int pointerOptionIndex() const;
    int pointerButtonIndex() const;

    void toggleOption(int index);
    void selectRadio(int index);
    void captureSnapshot();
    void restoreSnapshot();

    int nextOptionId = 1;
    int selectedIndex = 0;
    int scrollOffset = 0;
    int rowHeight = 22;

    bool applied = false;

    std::string applyLabel = "Apply";
    std::string cancelLabel = "Cancel";

    std::vector<option_popup_entry> options;
    std::vector<bool> snapshotValues;

    std::function<void(item_options_popup&)> onApply = nullptr;
    std::function<void()> onCancel = nullptr;
};

using item_checkbox_popup = item_options_popup;
using item_toggle_popup = item_options_popup;


//******************************** List classe *********************************
class liste_item{
public:
    liste_item() = default;
    ~liste_item() = default;

    int addItem(item* i);
    void removeItem(item* i);
    bool removeItemById(int id);

    void draw();
    void update(uint32_t nowTicks);
    void applyTheme(const item_theme& theme);

    item* getItems(unsigned int idx);
    item* getItemsById(int id);
    item* getTopItemAt(int x, int y);
    item* getKeyboardCapture();

    bool setZOrderById(int id, int zOrder);

    // Page filtering
    void setActivePageId(int pageId);
    int getActivePageId() const {return activePageId;};
    bool moveItemToPage(int itemId, int pageId);
    bool isItemOnActivePage(const item* currentItem) const;

    // Give modal/top-level controls a chance to consume EXIT before the app quits.
    bool handleSystemExit();

    // Common focus manager
    bool setFocusById(int id);
    void clearFocus();
    item* getFocusedItem();
    int getFocusedItemId() const {return focusedItemId;};
    bool focusNext();
    bool focusPrevious();

    // Dispatch to items which listen to the keyboard without taking focus
    void dispatchGlobalEvent(int eventType, int eventKey);

    unsigned int getItemsCount() const {return listItem.size();};

    private:
    std::map<int, item*> listItem;

    int nextId = 1;
    int focusedItemId = -1;
    int activePageId = 0;
};
//==============================================================================


#endif // __ITEM_H__