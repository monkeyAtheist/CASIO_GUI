#include "item.hpp"
#include <math.h>
#include <algorithm>



//******************************** Theme *********************************

item_theme make_item_theme(
    item_theme_preset preset)
{
    item_theme theme;

    switch(preset)
    {
        case item_theme_preset::DARK:
            theme.background = Class_color(22, 24, 28);
            theme.surface = Class_color(38, 41, 46);
            theme.surfaceAlt = Class_color(53, 57, 63);

            theme.border = Class_color(175, 180, 188);
            theme.text = Class_color(242, 244, 247);
            theme.textMuted = Class_color(165, 170, 178);

            theme.accent = Class_color(90, 145, 255);
            theme.accentText = Class_color(10, 15, 25);
            theme.selection = Class_color(65, 82, 118);

            theme.disabled = Class_color(105, 110, 118);
            theme.success = Class_color(55, 190, 105);
            theme.warning = Class_color(245, 175, 55);
            theme.error = Class_color(235, 80, 80);
            break;

        case item_theme_preset::HIGH_CONTRAST:
            theme.background = Class_color(0, 0, 0);
            theme.surface = Class_color(0, 0, 0);
            theme.surfaceAlt = Class_color(30, 30, 30);

            theme.border = Class_color(255, 255, 255);
            theme.text = Class_color(255, 255, 255);
            theme.textMuted = Class_color(210, 210, 210);

            theme.accent = Class_color(255, 220, 0);
            theme.accentText = Class_color(0, 0, 0);
            theme.selection = Class_color(60, 60, 60);

            theme.disabled = Class_color(145, 145, 145);
            theme.success = Class_color(0, 255, 80);
            theme.warning = Class_color(255, 220, 0);
            theme.error = Class_color(255, 70, 70);

            theme.borderSize = 2;
            theme.focusBorderSize = 3;
            break;

        case item_theme_preset::LIGHT:
        default:
            break;
    }

    return theme;
}

item_theme_preset default_item_theme_preset()
{
#if CASIO_GUI_DEFAULT_THEME == CASIO_GUI_THEME_DARK
    return item_theme_preset::DARK;
#elif CASIO_GUI_DEFAULT_THEME == CASIO_GUI_THEME_HIGH_CONTRAST
    return item_theme_preset::HIGH_CONTRAST;
#else
    return item_theme_preset::LIGHT;
#endif
}

item_theme& current_item_theme()
{
    static item_theme theme =
        make_item_theme(
            default_item_theme_preset()
        );

    return theme;
}

void set_item_theme(
    const item_theme& theme)
{
    current_item_theme() = theme;
}

void set_item_theme_preset(
    item_theme_preset preset)
{
    set_item_theme(
        make_item_theme(preset)
    );
}


//******************************** Text input mapping *********************************

static char graphAlphaLetterForKey(int key)
{
    switch(key)
    {
        case KEY_XOT:    return 'A';
        case KEY_LOG:    return 'B';
        case KEY_LN:     return 'C';
        case KEY_SIN:    return 'D';
        case KEY_COS:    return 'E';
        case KEY_TAN:    return 'F';

        case KEY_FRAC:   return 'G';
        case KEY_FD:     return 'H';
        case KEY_LEFTP:  return 'I';
        case KEY_RIGHTP: return 'J';
        case KEY_COMMA:  return 'K';
        case KEY_ARROW:  return 'L';

        case KEY_7:      return 'M';
        case KEY_8:      return 'N';
        case KEY_9:      return 'O';

        case KEY_4:      return 'P';
        case KEY_5:      return 'Q';
        case KEY_6:      return 'R';

        case KEY_MUL:    return 'S';
        case KEY_DIV:    return 'T';

        case KEY_1:      return 'U';
        case KEY_2:      return 'V';
        case KEY_3:      return 'W';

        case KEY_ADD:    return 'X';
        case KEY_SUB:    return 'Y';

        case KEY_0:      return 'Z';

        default:
            return 0;
    }
}

char item_text_key_to_char(
    int key,
    const item_text_input_state& state)
{
    if(state.isAlpha())
    {
        char letter =
            graphAlphaLetterForKey(key);

        if(letter == 0)
            return 0;

        // SHIFT while ALPHA is active gives a one-shot lowercase character.
        if(state.shift)
            letter =
                static_cast<char>(
                    letter - 'A' + 'a'
                );

        return letter;
    }

    if(state.shift)
    {
        // ASCII-oriented punctuation layer. Mathematical SHIFT functions from
        // the native calculator UI are intentionally not inserted into text.
        switch(key)
        {
            case KEY_0:      return ' ';
            case KEY_DOT:    return ':';
            case KEY_COMMA:  return ';';
            case KEY_LEFTP:  return '[';
            case KEY_RIGHTP: return ']';

            case KEY_ADD:    return '=';
            case KEY_SUB:    return '_';
            case KEY_MUL:    return '<';
            case KEY_DIV:    return '>';

            case KEY_EXP:    return '^';
            case KEY_NEG:    return '~';
            case KEY_ARROW:  return '\\';

            case KEY_FRAC:   return '%';
            case KEY_FD:     return '#';

            case KEY_7:      return '{';
            case KEY_8:      return '|';
            case KEY_9:      return '}';

            default:
                return 0;
        }
    }

    switch(key)
    {
        case KEY_0: return '0';
        case KEY_1: return '1';
        case KEY_2: return '2';
        case KEY_3: return '3';
        case KEY_4: return '4';
        case KEY_5: return '5';
        case KEY_6: return '6';
        case KEY_7: return '7';
        case KEY_8: return '8';
        case KEY_9: return '9';

        case KEY_DOT:    return '.';
        case KEY_ADD:    return '+';
        case KEY_SUB:    return '-';
        case KEY_NEG:    return '-';
        case KEY_MUL:    return '*';
        case KEY_DIV:    return '/';
        case KEY_LEFTP:  return '(';
        case KEY_RIGHTP: return ')';
        case KEY_COMMA:  return ',';

        default:
            return 0;
    }
}

bool item_text_handle_modifier(
    int key,
    item_text_input_state& state)
{
    if(key == KEY_SHIFT)
    {
        state.shift = !state.shift;
        return true;
    }

    if(key != KEY_ALPHA)
        return false;

    if(state.shift)
    {
        // SHIFT+ALPHA toggles persistent ALPHA lock.
        state.alpha =
            state.alpha == item_alpha_mode::LOCKED
                ? item_alpha_mode::OFF
                : item_alpha_mode::LOCKED;

        state.shift = false;
        return true;
    }

    switch(state.alpha)
    {
        case item_alpha_mode::OFF:
            state.alpha =
                item_alpha_mode::ONCE;
            break;

        case item_alpha_mode::ONCE:
            state.alpha =
                item_alpha_mode::LOCKED;
            break;

        case item_alpha_mode::LOCKED:
            state.alpha =
                item_alpha_mode::OFF;
            break;
    }

    return true;
}

void item_text_consume_modifier(
    item_text_input_state& state)
{
    state.shift = false;

    if(state.alpha == item_alpha_mode::ONCE)
        state.alpha = item_alpha_mode::OFF;
}

const char* item_text_modifier_label(
    const item_text_input_state& state)
{
    if(state.alpha == item_alpha_mode::LOCKED)
        return state.shift ? "a*" : "A*";

    if(state.alpha == item_alpha_mode::ONCE)
        return state.shift ? "a" : "A";

    if(state.shift)
        return "S";

    return "";
}


item::item()
{
    param.status = {false, false, false, false, false, true , false};
    param.pos = {0, 0, 100, 50};
    param.label = {"", "", "", "", "", ""};
    id = 0;

    applyTheme(
        current_item_theme()
    );
}

void item::applyTheme(
    const item_theme& theme)
{
    param.color.offColor =
        RawitemColor(
            theme.surface,
            theme.border,
            theme.text
        );

    param.color.onColor =
        RawitemColor(
            theme.accent,
            theme.border,
            theme.accentText
        );

    param.color.hoverOffColor =
        RawitemColor(
            theme.surfaceAlt,
            theme.accent,
            theme.text
        );

    param.color.hoverOnColor =
        RawitemColor(
            theme.accent,
            theme.accent,
            theme.accentText
        );

    param.color.clickColor =
        RawitemColor(
            theme.selection,
            theme.accent,
            theme.text
        );

    param.color.dragColor =
        RawitemColor(
            theme.selection,
            theme.accent,
            theme.text
        );

    param.color.dimmedColor =
        RawitemColor(
            theme.surfaceAlt,
            theme.disabled,
            theme.disabled
        );
}

void item::chckEvent(int eventType, int eventKey)
{
    if(!event.callback)
        return;

    switch(event.type)
    {
        case itemEvent::eventType::HOVER :
            if(param.status.hover)
                event.callback();
            break;
        case itemEvent::eventType::KEY_UP :
            if(eventType != KEYEV_UP || param.status.clicked == false || event.keyEvent != eventKey)
                break;
            event.callback();
            break;
        case itemEvent::eventType::KEY_DOWN :
            if(eventType != KEYEV_DOWN || param.status.clicked == false || event.keyEvent != eventKey)
                break;
            event.callback();
            break;
        case itemEvent::eventType::KEY_PRESS :
            if(((eventType != KEYEV_DOWN) && (eventType != KEYEV_HOLD)) || param.status.clicked == false || event.keyEvent != eventKey)
                break;
            event.callback();
            break;
        case itemEvent::eventType::DRAG :
            // if(eventType != KEYEV_DOWN || param.status.dragged == true || event.keyEvent != eventKey)
            break;
    }
}

bool item::contains(int x, int y) const
{
    return (
        x >= getX() &&
        x < getX() + getW() &&
        y >= getY() &&
        y < getY() + getH()
    );
}

bool item::isHover(int x, int y)
{
    pointerX = x;
    pointerY = y;
    param.status.hover = contains(x, y);
    return param.status.hover;
}

//******************************** Transform2D *********************************

void Transform2D::prepend(
    double a,
    double b,
    double c,
    double d,
    double tx,
    double ty)
{
    double n00 = a * m00 + b * m10;
    double n01 = a * m01 + b * m11;
    double n02 = a * m02 + b * m12 + tx;

    double n10 = c * m00 + d * m10;
    double n11 = c * m01 + d * m11;
    double n12 = c * m02 + d * m12 + ty;

    m00 = n00;
    m01 = n01;
    m02 = n02;
    m10 = n10;
    m11 = n11;
    m12 = n12;
}

Transform2D& Transform2D::reset()
{
    m00 = 1.0;
    m01 = 0.0;
    m02 = 0.0;
    m10 = 0.0;
    m11 = 1.0;
    m12 = 0.0;
    return *this;
}

Transform2D& Transform2D::translate(double dx, double dy)
{
    prepend(1.0, 0.0, 0.0, 1.0, dx, dy);
    return *this;
}

Transform2D& Transform2D::rotate(double degrees)
{
    return rotate(degrees, 0.0, 0.0);
}

Transform2D& Transform2D::rotate(
    double degrees,
    double centerX,
    double centerY)
{
    const double pi = 3.14159265358979323846;
    double angle = degrees * pi / 180.0;
    double c = cos(angle);
    double s = sin(angle);

    double tx = centerX - c * centerX + s * centerY;
    double ty = centerY - s * centerX - c * centerY;

    prepend(c, -s, s, c, tx, ty);
    return *this;
}

Transform2D& Transform2D::scale(double sx, double sy)
{
    return scale(sx, sy, 0.0, 0.0);
}

Transform2D& Transform2D::scale(
    double sx,
    double sy,
    double centerX,
    double centerY)
{
    double tx = centerX - sx * centerX;
    double ty = centerY - sy * centerY;

    prepend(sx, 0.0, 0.0, sy, tx, ty);
    return *this;
}

Transform2D& Transform2D::mirrorX(double axisX)
{
    return scale(-1.0, 1.0, axisX, 0.0);
}

Transform2D& Transform2D::mirrorY(double axisY)
{
    return scale(1.0, -1.0, 0.0, axisY);
}

STRUCT_point Transform2D::apply(STRUCT_point point) const
{
    double x =
        m00 * static_cast<double>(point.x) +
        m01 * static_cast<double>(point.y) +
        m02;

    double y =
        m10 * static_cast<double>(point.x) +
        m11 * static_cast<double>(point.y) +
        m12;

    return STRUCT_point{
        static_cast<int>(x >= 0.0 ? x + 0.5 : x - 0.5),
        static_cast<int>(y >= 0.0 ? y + 0.5 : y - 0.5)
    };
}

item_button::item_button(
    STRUCT_pos _pos,
    itemEvent _event,
    item_status _status,
    itemColor _color,
    item_label _label)
{
    param.pos = _pos;
    param.status = _status;
    param.color = _color;
    param.label = _label;
    event = _event;
}

void item_button::draw()
{
    if(!isVisible())
        return;


    int x1 = getX();
    int y1 = getY();

    int x2 = x1 + getW() - 1;
    int y2 = y1 + getH() - 1;


    int borderColor;
    int fillColor;
    int textColor;


    if(isDimmed())
    {
        borderColor = param.color.dimmedColor.borderColor.getRGB();
        fillColor   = param.color.dimmedColor.fillColor.getRGB();
        textColor   = param.color.dimmedColor.TextColor.getRGB();
    }

    else if(param.status.clicked)
    {
        borderColor = getBorderColorClick().getRGB();
        fillColor   = getFillColorClick().getRGB();
        textColor   = getTextColorClick().getRGB();
    }

    else if(param.status.hover && param.status.ON)
    {
        borderColor = getBorderColorHoverOn().getRGB();
        fillColor   = getFillColorHoverOn().getRGB();
        textColor   = getTextColorHoverOn().getRGB();
    }

    else if(param.status.hover)
    {
        borderColor = getBorderColorHoverOff().getRGB();
        fillColor   = getFillColorHoverOff().getRGB();
        textColor   = getTextColorHoverOff().getRGB();
    }

    else if(param.status.ON)
    {
        borderColor = getBorderColorOn().getRGB();
        fillColor   = getFillColorOn().getRGB();
        textColor   = getTextColorOn().getRGB();
    }

    else
    {
        borderColor = getBorderColorOff().getRGB();
        fillColor   = getFillColorOff().getRGB();
        textColor   = getTextColorOff().getRGB();
    }


    drect(
        x1,
        y1,
        x2,
        y2,
        borderColor
    );


    drect(
        x1 + 2,
        y1 + 2,
        x2 - 2,
        y2 - 2,
        fillColor
    );


    dtext(
        x1 + 10,
        y1 + 10,
        textColor,
        getLabelOn().c_str()
    );
}

bool item_button::isHover(int x, int y)
{
    return item::isHover(x, y);
}


//******************************** Drawing helpers *********************************

static void drawOutlineRect(
    int x1,
    int y1,
    int x2,
    int y2,
    int color,
    int borderSize)
{
    if(borderSize < 1)
        borderSize = 1;

    for(int i = 0; i < borderSize; ++i)
    {
        if(x1 + i > x2 - i || y1 + i > y2 - i)
            break;

        dline(x1 + i, y1 + i, x2 - i, y1 + i, color);
        dline(x1 + i, y2 - i, x2 - i, y2 - i, color);
        dline(x1 + i, y1 + i, x1 + i, y2 - i, color);
        dline(x2 - i, y1 + i, x2 - i, y2 - i, color);
    }
}

static void drawCircleOutline(
    int centerX,
    int centerY,
    int radius,
    int color)
{
    if(radius < 0)
        return;

    int x = radius;
    int y = 0;
    int error = 1 - radius;

    while(x >= y)
    {
        dpixel(centerX + x, centerY + y, color);
        dpixel(centerX + y, centerY + x, color);
        dpixel(centerX - y, centerY + x, color);
        dpixel(centerX - x, centerY + y, color);
        dpixel(centerX - x, centerY - y, color);
        dpixel(centerX - y, centerY - x, color);
        dpixel(centerX + y, centerY - x, color);
        dpixel(centerX + x, centerY - y, color);

        ++y;

        if(error < 0)
        {
            error += 2 * y + 1;
        }
        else
        {
            --x;
            error += 2 * (y - x) + 1;
        }
    }
}

static void drawCircleFilled(
    int centerX,
    int centerY,
    int radius,
    int color)
{
    if(radius < 0)
        return;

    int x = radius;
    int y = 0;
    int error = 1 - radius;

    while(x >= y)
    {
        dline(centerX - x, centerY + y, centerX + x, centerY + y, color);
        dline(centerX - x, centerY - y, centerX + x, centerY - y, color);
        dline(centerX - y, centerY + x, centerX + y, centerY + x, color);
        dline(centerX - y, centerY - x, centerX + y, centerY - x, color);

        ++y;

        if(error < 0)
        {
            error += 2 * y + 1;
        }
        else
        {
            --x;
            error += 2 * (y - x) + 1;
        }
    }
}

static long triangleSign(
    int px,
    int py,
    const STRUCT_point& a,
    const STRUCT_point& b)
{
    return static_cast<long>(px - b.x) * static_cast<long>(a.y - b.y)
         - static_cast<long>(a.x - b.x) * static_cast<long>(py - b.y);
}

static bool pointInTriangle(
    int x,
    int y,
    const STRUCT_point& p1,
    const STRUCT_point& p2,
    const STRUCT_point& p3)
{
    long d1 = triangleSign(x, y, p1, p2);
    long d2 = triangleSign(x, y, p2, p3);
    long d3 = triangleSign(x, y, p3, p1);

    bool hasNegative = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool hasPositive = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(hasNegative && hasPositive);
}

//******************************** Rectangle *********************************

item_rectangle::item_rectangle(
    STRUCT_pos _pos,
    Class_color fillColor,
    Class_color borderColor,
    item_draw_mode mode,
    int _borderSize,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;

    param.color.offColor.fillColor = fillColor;
    param.color.offColor.borderColor = borderColor;

    drawMode = mode;
    borderSize = (_borderSize > 0) ? _borderSize : 1;
    zOrder = _zOrder;
}

void item_rectangle::draw()
{
    if(!isVisible())
        return;

    int x1 = getX();
    int y1 = getY();
    int x2 = x1 + getW() - 1;
    int y2 = y1 + getH() - 1;

    if(drawMode == item_draw_mode::FILLED)
    {
        drect(
            x1,
            y1,
            x2,
            y2,
            getFillColorOff().getRGB()
        );
    }

    drawOutlineRect(
        x1,
        y1,
        x2,
        y2,
        getBorderColorOff().getRGB(),
        borderSize
    );
}

//******************************** Circle *********************************

item_circle::item_circle(
    int _centerX,
    int _centerY,
    int _radius,
    Class_color fillColor,
    Class_color borderColor,
    item_draw_mode mode,
    int _borderSize,
    int _zOrder)
{
    centerX = _centerX;
    centerY = _centerY;
    radius = (_radius > 0) ? _radius : 1;
    borderSize = (_borderSize > 0) ? _borderSize : 1;
    drawMode = mode;

    param.pos = {
        centerX - radius,
        centerY - radius,
        radius * 2 + 1,
        radius * 2 + 1
    };

    param.status.visible = true;

    param.color.offColor.fillColor = fillColor;
    param.color.offColor.borderColor = borderColor;

    zOrder = _zOrder;
}

bool item_circle::contains(int x, int y) const
{
    long dx = static_cast<long>(x - centerX);
    long dy = static_cast<long>(y - centerY);
    long r = static_cast<long>(radius);

    return dx * dx + dy * dy <= r * r;
}

void item_circle::draw()
{
    if(!isVisible())
        return;

    if(drawMode == item_draw_mode::FILLED)
    {
        drawCircleFilled(
            centerX,
            centerY,
            radius,
            getFillColorOff().getRGB()
        );
    }

    for(int i = 0; i < borderSize; ++i)
    {
        int currentRadius = radius - i;

        if(currentRadius < 0)
            break;

        drawCircleOutline(
            centerX,
            centerY,
            currentRadius,
            getBorderColorOff().getRGB()
        );
    }
}

void item_circle::setCenter(int x, int y)
{
    centerX = x;
    centerY = y;

    param.pos.x = centerX - radius;
    param.pos.y = centerY - radius;
}

void item_circle::setRadius(int _radius)
{
    radius = (_radius > 0) ? _radius : 1;

    param.pos = {
        centerX - radius,
        centerY - radius,
        radius * 2 + 1,
        radius * 2 + 1
    };
}

//******************************** Triangle *********************************

item_triangle::item_triangle(
    STRUCT_point _p1,
    STRUCT_point _p2,
    STRUCT_point _p3,
    Class_color fillColor,
    Class_color borderColor,
    item_draw_mode mode,
    int _zOrder)
{
    p1 = _p1;
    p2 = _p2;
    p3 = _p3;
    drawMode = mode;

    param.status.visible = true;

    param.color.offColor.fillColor = fillColor;
    param.color.offColor.borderColor = borderColor;

    zOrder = _zOrder;

    updateBoundingBox();
}

void item_triangle::updateBoundingBox()
{
    int minX = p1.x;
    int minY = p1.y;
    int maxX = p1.x;
    int maxY = p1.y;

    if(p2.x < minX) minX = p2.x;
    if(p3.x < minX) minX = p3.x;
    if(p2.y < minY) minY = p2.y;
    if(p3.y < minY) minY = p3.y;

    if(p2.x > maxX) maxX = p2.x;
    if(p3.x > maxX) maxX = p3.x;
    if(p2.y > maxY) maxY = p2.y;
    if(p3.y > maxY) maxY = p3.y;

    param.pos = {
        minX,
        minY,
        maxX - minX + 1,
        maxY - minY + 1
    };
}

void item_triangle::setPoints(
    STRUCT_point _p1,
    STRUCT_point _p2,
    STRUCT_point _p3)
{
    p1 = _p1;
    p2 = _p2;
    p3 = _p3;
    updateBoundingBox();
}

bool item_triangle::contains(int x, int y) const
{
    return pointInTriangle(x, y, p1, p2, p3);
}

void item_triangle::draw()
{
    if(!isVisible())
        return;

    int fillColor = getFillColorOff().getRGB();
    int borderColor = getBorderColorOff().getRGB();

    if(drawMode == item_draw_mode::FILLED)
    {
        int minX = getX();
        int minY = getY();
        int maxX = getX() + getW() - 1;
        int maxY = getY() + getH() - 1;

        for(int y = minY; y <= maxY; ++y)
        {
            int firstX = maxX + 1;
            int lastX = minX - 1;

            for(int x = minX; x <= maxX; ++x)
            {
                if(pointInTriangle(x, y, p1, p2, p3))
                {
                    if(x < firstX) firstX = x;
                    if(x > lastX) lastX = x;
                }
            }

            if(firstX <= lastX)
                dline(firstX, y, lastX, y, fillColor);
        }
    }

    dline(p1.x, p1.y, p2.x, p2.y, borderColor);
    dline(p2.x, p2.y, p3.x, p3.y, borderColor);
    dline(p3.x, p3.y, p1.x, p1.y, borderColor);
}


//******************************** Polygon helpers / advanced shapes *********************************

static STRUCT_point rotatePointAround(
    STRUCT_point point,
    STRUCT_point center,
    float degrees)
{
    const float pi = 3.14159265358979323846f;
    float angle = degrees * pi / 180.0f;
    float cs = cosf(angle);
    float sn = sinf(angle);

    float dx = static_cast<float>(point.x - center.x);
    float dy = static_cast<float>(point.y - center.y);

    return STRUCT_point{
        center.x + static_cast<int>(dx * cs - dy * sn + (dx * cs - dy * sn >= 0.0f ? 0.5f : -0.5f)),
        center.y + static_cast<int>(dx * sn + dy * cs + (dx * sn + dy * cs >= 0.0f ? 0.5f : -0.5f))
    };
}

void item_triangle::rotate(float degrees)
{
    STRUCT_point center{
        getX() + getW() / 2,
        getY() + getH() / 2
    };

    rotate(degrees, center);
}

void item_triangle::rotate(float degrees, STRUCT_point center)
{
    p1 = rotatePointAround(p1, center, degrees);
    p2 = rotatePointAround(p2, center, degrees);
    p3 = rotatePointAround(p3, center, degrees);
    updateBoundingBox();
}

void item_triangle::mirrorX()
{
    mirrorX(getX() + getW() / 2);
}

void item_triangle::mirrorX(int axisX)
{
    p1.x = axisX * 2 - p1.x;
    p2.x = axisX * 2 - p2.x;
    p3.x = axisX * 2 - p3.x;
    updateBoundingBox();
}

void item_triangle::mirrorY()
{
    mirrorY(getY() + getH() / 2);
}

void item_triangle::mirrorY(int axisY)
{
    p1.y = axisY * 2 - p1.y;
    p2.y = axisY * 2 - p2.y;
    p3.y = axisY * 2 - p3.y;
    updateBoundingBox();
}

void item_triangle::translate(int dx, int dy)
{
    Transform2D transform;
    transform.translate(dx, dy);
    applyTransform(transform);
}

void item_triangle::scale(float sx, float sy)
{
    scale(
        sx,
        sy,
        STRUCT_point{
            getX() + getW() / 2,
            getY() + getH() / 2
        }
    );
}

void item_triangle::scale(
    float sx,
    float sy,
    STRUCT_point center)
{
    Transform2D transform;
    transform.scale(sx, sy, center.x, center.y);
    applyTransform(transform);
}

void item_triangle::applyTransform(const Transform2D& transform)
{
    p1 = transform.apply(p1);
    p2 = transform.apply(p2);
    p3 = transform.apply(p3);
    updateBoundingBox();
}

static void drawThickLine(
    int x1,
    int y1,
    int x2,
    int y2,
    int color,
    int thickness)
{
    if(thickness < 1)
        thickness = 1;

    int half = thickness / 2;

    if(abs(x2 - x1) >= abs(y2 - y1))
    {
        for(int offset = -half; offset <= half; ++offset)
            dline(x1, y1 + offset, x2, y2 + offset, color);
    }
    else
    {
        for(int offset = -half; offset <= half; ++offset)
            dline(x1 + offset, y1, x2 + offset, y2, color);
    }
}

item_polygon::item_polygon(
    std::vector<STRUCT_point> _points,
    Class_color fillColor,
    Class_color borderColor,
    item_draw_mode mode,
    int _borderSize,
    int _zOrder)
{
    points = _points;
    drawMode = mode;
    borderSize = (_borderSize > 0) ? _borderSize : 1;

    param.status.visible = true;
    param.color.offColor.fillColor = fillColor;
    param.color.offColor.borderColor = borderColor;

    zOrder = _zOrder;

    scanlineScratch.reserve(
        points.size()
    );

    updateBoundingBox();
}

void item_polygon::updateBoundingBox()
{
    if(points.empty())
    {
        param.pos = {0, 0, 0, 0};
        return;
    }

    int minX = points[0].x;
    int minY = points[0].y;
    int maxX = points[0].x;
    int maxY = points[0].y;

    for(const auto& point : points)
    {
        if(point.x < minX) minX = point.x;
        if(point.x > maxX) maxX = point.x;
        if(point.y < minY) minY = point.y;
        if(point.y > maxY) maxY = point.y;
    }

    param.pos = {
        minX,
        minY,
        maxX - minX + 1,
        maxY - minY + 1
    };
}

void item_polygon::setPoints(
    const std::vector<STRUCT_point>& value)
{
    points = value;

    if(scanlineScratch.capacity() < points.size())
        scanlineScratch.reserve(points.size());

    updateBoundingBox();
}

void item_polygon::addPoint(STRUCT_point point)
{
    points.push_back(point);

    if(scanlineScratch.capacity() < points.size())
        scanlineScratch.reserve(points.size());

    updateBoundingBox();
}

bool item_polygon::contains(int x, int y) const
{
    if(points.size() < 3)
        return false;

    bool inside = false;
    size_t j = points.size() - 1;

    for(size_t i = 0; i < points.size(); ++i)
    {
        const STRUCT_point& pi = points[i];
        const STRUCT_point& pj = points[j];

        bool crosses =
            ((pi.y > y) != (pj.y > y));

        if(crosses)
        {
            double intersectionX =
                static_cast<double>(pj.x - pi.x) *
                static_cast<double>(y - pi.y) /
                static_cast<double>(pj.y - pi.y) +
                static_cast<double>(pi.x);

            if(static_cast<double>(x) < intersectionX)
                inside = !inside;
        }

        j = i;
    }

    return inside;
}

void item_polygon::draw()
{
    if(!isVisible() || points.size() < 2)
        return;

    int fillColor = getFillColorOff().getRGB();
    int borderColor = getBorderColorOff().getRGB();

    if(drawMode == item_draw_mode::FILLED && points.size() >= 3)
    {
        if(scanlineScratch.capacity() < points.size())
            scanlineScratch.reserve(points.size());

        int minY = getY();
        int maxY = getY() + getH() - 1;

        for(int y = minY; y <= maxY; ++y)
        {
            scanlineScratch.clear();

            for(size_t i = 0; i < points.size(); ++i)
            {
                const STRUCT_point& a = points[i];
                const STRUCT_point& b = points[(i + 1) % points.size()];

                if(a.y == b.y)
                    continue;

                int lowY = (a.y < b.y) ? a.y : b.y;
                int highY = (a.y > b.y) ? a.y : b.y;

                // Half-open interval prevents double-counting polygon vertices.
                if(y < lowY || y >= highY)
                    continue;

                double ratio =
                    static_cast<double>(y - a.y) /
                    static_cast<double>(b.y - a.y);

                int x = static_cast<int>(
                    static_cast<double>(a.x) +
                    ratio * static_cast<double>(b.x - a.x)
                );

                scanlineScratch.push_back(x);
            }

            std::sort(scanlineScratch.begin(), scanlineScratch.end());

            for(size_t i = 0; i + 1 < scanlineScratch.size(); i += 2)
            {
                dline(
                    scanlineScratch[i],
                    y,
                    scanlineScratch[i + 1],
                    y,
                    fillColor
                );
            }
        }
    }

    if(points.size() >= 2)
    {
        for(size_t i = 0; i < points.size(); ++i)
        {
            const STRUCT_point& a = points[i];
            const STRUCT_point& b = points[(i + 1) % points.size()];

            drawThickLine(
                a.x,
                a.y,
                b.x,
                b.y,
                borderColor,
                borderSize
            );
        }
    }
}

void item_polygon::rotate(float degrees)
{
    STRUCT_point center{
        getX() + getW() / 2,
        getY() + getH() / 2
    };

    rotate(degrees, center);
}

void item_polygon::rotate(float degrees, STRUCT_point center)
{
    for(auto& point : points)
        point = rotatePointAround(point, center, degrees);

    updateBoundingBox();
}

void item_polygon::mirrorX()
{
    mirrorX(getX() + getW() / 2);
}

void item_polygon::mirrorX(int axisX)
{
    for(auto& point : points)
        point.x = axisX * 2 - point.x;

    updateBoundingBox();
}

void item_polygon::mirrorY()
{
    mirrorY(getY() + getH() / 2);
}

void item_polygon::mirrorY(int axisY)
{
    for(auto& point : points)
        point.y = axisY * 2 - point.y;

    updateBoundingBox();
}

void item_polygon::translate(int dx, int dy)
{
    Transform2D transform;
    transform.translate(dx, dy);
    applyTransform(transform);
}

void item_polygon::scale(float sx, float sy)
{
    scale(
        sx,
        sy,
        STRUCT_point{
            getX() + getW() / 2,
            getY() + getH() / 2
        }
    );
}

void item_polygon::scale(
    float sx,
    float sy,
    STRUCT_point center)
{
    Transform2D transform;
    transform.scale(sx, sy, center.x, center.y);
    applyTransform(transform);
}

void item_polygon::applyTransform(const Transform2D& transform)
{
    for(auto& point : points)
        point = transform.apply(point);

    updateBoundingBox();
}

static std::vector<STRUCT_point> rightTrianglePoints(
    STRUCT_pos bounds,
    item_right_triangle_corner corner)
{
    int x1 = bounds.x;
    int y1 = bounds.y;
    int x2 = bounds.x + bounds.w - 1;
    int y2 = bounds.y + bounds.h - 1;

    switch(corner)
    {
        case item_right_triangle_corner::TOP_RIGHT:
            return {
                STRUCT_point{x2, y1},
                STRUCT_point{x1, y1},
                STRUCT_point{x2, y2}
            };

        case item_right_triangle_corner::BOTTOM_LEFT:
            return {
                STRUCT_point{x1, y2},
                STRUCT_point{x1, y1},
                STRUCT_point{x2, y2}
            };

        case item_right_triangle_corner::BOTTOM_RIGHT:
            return {
                STRUCT_point{x2, y2},
                STRUCT_point{x2, y1},
                STRUCT_point{x1, y2}
            };

        case item_right_triangle_corner::TOP_LEFT:
        default:
            return {
                STRUCT_point{x1, y1},
                STRUCT_point{x2, y1},
                STRUCT_point{x1, y2}
            };
    }
}

item_right_triangle::item_right_triangle(
    STRUCT_pos bounds,
    item_right_triangle_corner corner,
    Class_color fillColor,
    Class_color borderColor,
    item_draw_mode mode,
    int _borderSize,
    int _zOrder)
    : item_polygon(
        rightTrianglePoints(bounds, corner),
        fillColor,
        borderColor,
        mode,
        _borderSize,
        _zOrder)
{
}

static std::vector<STRUCT_point> trapezoidPoints(
    STRUCT_pos bounds,
    int topWidth,
    int bottomWidth)
{
    int width = (bounds.w > 0) ? bounds.w : 1;
    int height = (bounds.h > 0) ? bounds.h : 1;

    if(topWidth < 1) topWidth = 1;
    if(bottomWidth < 1) bottomWidth = 1;
    if(topWidth > width) topWidth = width;
    if(bottomWidth > width) bottomWidth = width;

    int topLeft = bounds.x + (width - topWidth) / 2;
    int bottomLeft = bounds.x + (width - bottomWidth) / 2;
    int yTop = bounds.y;
    int yBottom = bounds.y + height - 1;

    return {
        STRUCT_point{topLeft, yTop},
        STRUCT_point{topLeft + topWidth - 1, yTop},
        STRUCT_point{bottomLeft + bottomWidth - 1, yBottom},
        STRUCT_point{bottomLeft, yBottom}
    };
}

item_trapezoid::item_trapezoid(
    STRUCT_pos bounds,
    int topWidth,
    int bottomWidth,
    Class_color fillColor,
    Class_color borderColor,
    item_draw_mode mode,
    int _borderSize,
    int _zOrder)
    : item_polygon(
        trapezoidPoints(bounds, topWidth, bottomWidth),
        fillColor,
        borderColor,
        mode,
        _borderSize,
        _zOrder)
{
}

static std::vector<STRUCT_point> regularPolygonPoints(
    int centerX,
    int centerY,
    int radius,
    unsigned int sides,
    float rotationDegrees)
{
    if(radius < 1)
        radius = 1;

    if(sides < 3)
        sides = 3;

    const float pi = 3.14159265358979323846f;
    std::vector<STRUCT_point> result;
    result.reserve(sides);

    for(unsigned int i = 0; i < sides; ++i)
    {
        float degrees =
            rotationDegrees +
            360.0f * static_cast<float>(i) /
            static_cast<float>(sides);

        float angle = degrees * pi / 180.0f;

        result.push_back(
            STRUCT_point{
                centerX + static_cast<int>(cosf(angle) * radius),
                centerY + static_cast<int>(sinf(angle) * radius)
            }
        );
    }

    return result;
}

item_regular_polygon::item_regular_polygon(
    int centerX,
    int centerY,
    int radius,
    unsigned int sides,
    float rotationDegrees,
    Class_color fillColor,
    Class_color borderColor,
    item_draw_mode mode,
    int _borderSize,
    int _zOrder)
    : item_polygon(
        regularPolygonPoints(
            centerX,
            centerY,
            radius,
            sides,
            rotationDegrees),
        fillColor,
        borderColor,
        mode,
        _borderSize,
        _zOrder)
{
}

item_ellipse::item_ellipse(
    int _centerX,
    int _centerY,
    int _radiusX,
    int _radiusY,
    Class_color fillColor,
    Class_color borderColor,
    item_draw_mode mode,
    int _borderSize,
    int _zOrder)
{
    centerX = _centerX;
    centerY = _centerY;
    radiusX = (_radiusX > 0) ? _radiusX : 1;
    radiusY = (_radiusY > 0) ? _radiusY : 1;
    drawMode = mode;
    borderSize = (_borderSize > 0) ? _borderSize : 1;

    param.pos = {
        centerX - radiusX,
        centerY - radiusY,
        radiusX * 2 + 1,
        radiusY * 2 + 1
    };

    param.status.visible = true;
    param.color.offColor.fillColor = fillColor;
    param.color.offColor.borderColor = borderColor;
    zOrder = _zOrder;

    rebuildExtentCache();
}

void item_ellipse::rebuildExtentCache()
{
    extentCache.clear();
    extentCache.resize(
        static_cast<unsigned int>(
            radiusY * 2 + 1
        )
    );

    for(int dy = -radiusY;
        dy <= radiusY;
        ++dy)
    {
        double normalized =
            static_cast<double>(dy) /
            static_cast<double>(radiusY);

        double remaining =
            1.0 -
            normalized *
            normalized;

        if(remaining < 0.0)
            remaining = 0.0;

        extentCache[
            static_cast<unsigned int>(
                dy + radiusY
            )
        ] =
            static_cast<int>(
                static_cast<double>(radiusX) *
                sqrt(remaining)
            );
    }
}

bool item_ellipse::contains(int x, int y) const
{
    long long dx = static_cast<long long>(x - centerX);
    long long dy = static_cast<long long>(y - centerY);
    long long rx = radiusX;
    long long ry = radiusY;

    return
        dx * dx * ry * ry +
        dy * dy * rx * rx <=
        rx * rx * ry * ry;
}

void item_ellipse::draw()
{
    if(!isVisible())
        return;

    int fillColor = getFillColorOff().getRGB();
    int borderColor = getBorderColorOff().getRGB();

    for(int dy = -radiusY; dy <= radiusY; ++dy)
    {
        int extent =
            extentCache[
                static_cast<unsigned int>(
                    dy + radiusY
                )
            ];

        if(drawMode == item_draw_mode::FILLED)
        {
            dline(
                centerX - extent,
                centerY + dy,
                centerX + extent,
                centerY + dy,
                fillColor
            );
        }

        for(int b = 0; b < borderSize; ++b)
        {
            int bx = extent - b;

            if(bx < 0)
                break;

            dpixel(centerX - bx, centerY + dy, borderColor);
            dpixel(centerX + bx, centerY + dy, borderColor);
        }
    }

    // Horizontal extremities are reinforced to keep the outline continuous.
    for(int b = 0; b < borderSize; ++b)
    {
        dpixel(centerX, centerY - radiusY + b, borderColor);
        dpixel(centerX, centerY + radiusY - b, borderColor);
    }
}

item_line::item_line(
    STRUCT_point _p1,
    STRUCT_point _p2,
    Class_color color,
    int _thickness,
    int _zOrder)
{
    p1 = _p1;
    p2 = _p2;
    lineColor = color;
    thickness = (_thickness > 0) ? _thickness : 1;
    param.status.visible = true;
    zOrder = _zOrder;
    updateBoundingBox();
}

void item_line::updateBoundingBox()
{
    int half = thickness / 2 + 1;

    int minX = (p1.x < p2.x) ? p1.x : p2.x;
    int maxX = (p1.x > p2.x) ? p1.x : p2.x;
    int minY = (p1.y < p2.y) ? p1.y : p2.y;
    int maxY = (p1.y > p2.y) ? p1.y : p2.y;

    param.pos = {
        minX - half,
        minY - half,
        maxX - minX + 1 + half * 2,
        maxY - minY + 1 + half * 2
    };
}

void item_line::setPoints(
    STRUCT_point _p1,
    STRUCT_point _p2)
{
    p1 = _p1;
    p2 = _p2;
    updateBoundingBox();
}

bool item_line::contains(int x, int y) const
{
    double vx = static_cast<double>(p2.x - p1.x);
    double vy = static_cast<double>(p2.y - p1.y);
    double wx = static_cast<double>(x - p1.x);
    double wy = static_cast<double>(y - p1.y);

    double lengthSq = vx * vx + vy * vy;

    if(lengthSq <= 0.0)
    {
        double dx = static_cast<double>(x - p1.x);
        double dy = static_cast<double>(y - p1.y);
        return dx * dx + dy * dy <= thickness * thickness;
    }

    double t = (wx * vx + wy * vy) / lengthSq;

    if(t < 0.0) t = 0.0;
    if(t > 1.0) t = 1.0;

    double px = static_cast<double>(p1.x) + t * vx;
    double py = static_cast<double>(p1.y) + t * vy;

    double dx = static_cast<double>(x) - px;
    double dy = static_cast<double>(y) - py;
    double tolerance = static_cast<double>(thickness + 2);

    return dx * dx + dy * dy <= tolerance * tolerance;
}

void item_line::draw()
{
    if(!isVisible())
        return;

    drawThickLine(
        p1.x,
        p1.y,
        p2.x,
        p2.y,
        lineColor.getRGB(),
        thickness
    );
}

void item_line::translate(int dx, int dy)
{
    Transform2D transform;
    transform.translate(dx, dy);
    applyTransform(transform);
}

void item_line::rotate(float degrees)
{
    rotate(
        degrees,
        STRUCT_point{
            (p1.x + p2.x) / 2,
            (p1.y + p2.y) / 2
        }
    );
}

void item_line::rotate(float degrees, STRUCT_point center)
{
    Transform2D transform;
    transform.rotate(degrees, center.x, center.y);
    applyTransform(transform);
}

void item_line::scale(float sx, float sy)
{
    scale(
        sx,
        sy,
        STRUCT_point{
            (p1.x + p2.x) / 2,
            (p1.y + p2.y) / 2
        }
    );
}

void item_line::scale(
    float sx,
    float sy,
    STRUCT_point center)
{
    Transform2D transform;
    transform.scale(sx, sy, center.x, center.y);
    applyTransform(transform);
}

void item_line::mirrorX()
{
    mirrorX((p1.x + p2.x) / 2);
}

void item_line::mirrorX(int axisX)
{
    Transform2D transform;
    transform.mirrorX(axisX);
    applyTransform(transform);
}

void item_line::mirrorY()
{
    mirrorY((p1.y + p2.y) / 2);
}

void item_line::mirrorY(int axisY)
{
    Transform2D transform;
    transform.mirrorY(axisY);
    applyTransform(transform);
}

void item_line::applyTransform(const Transform2D& transform)
{
    p1 = transform.apply(p1);
    p2 = transform.apply(p2);
    updateBoundingBox();
}


//******************************** Polyline / arc / sector *********************************

item_polyline::item_polyline(
    std::vector<STRUCT_point> _points,
    Class_color color,
    int _thickness,
    int _zOrder)
{
    points = _points;
    lineColor = color;
    thickness = (_thickness > 0) ? _thickness : 1;
    param.status.visible = true;
    zOrder = _zOrder;
    updateBoundingBox();
}

void item_polyline::updateBoundingBox()
{
    if(points.empty())
    {
        param.pos = {0, 0, 0, 0};
        return;
    }

    int minX = points[0].x;
    int maxX = points[0].x;
    int minY = points[0].y;
    int maxY = points[0].y;

    for(const auto& point : points)
    {
        if(point.x < minX) minX = point.x;
        if(point.x > maxX) maxX = point.x;
        if(point.y < minY) minY = point.y;
        if(point.y > maxY) maxY = point.y;
    }

    int margin = thickness / 2 + 2;

    param.pos = {
        minX - margin,
        minY - margin,
        maxX - minX + 1 + margin * 2,
        maxY - minY + 1 + margin * 2
    };
}

void item_polyline::setPoints(
    const std::vector<STRUCT_point>& value)
{
    points = value;
    updateBoundingBox();
}

void item_polyline::addPoint(STRUCT_point point)
{
    points.push_back(point);
    updateBoundingBox();
}

static bool pointNearSegment(
    int x,
    int y,
    STRUCT_point a,
    STRUCT_point b,
    double tolerance)
{
    double vx = static_cast<double>(b.x - a.x);
    double vy = static_cast<double>(b.y - a.y);
    double wx = static_cast<double>(x - a.x);
    double wy = static_cast<double>(y - a.y);

    double lengthSq = vx * vx + vy * vy;

    if(lengthSq <= 0.0)
    {
        double dx = static_cast<double>(x - a.x);
        double dy = static_cast<double>(y - a.y);
        return dx * dx + dy * dy <= tolerance * tolerance;
    }

    double t = (wx * vx + wy * vy) / lengthSq;

    if(t < 0.0) t = 0.0;
    if(t > 1.0) t = 1.0;

    double px = static_cast<double>(a.x) + t * vx;
    double py = static_cast<double>(a.y) + t * vy;

    double dx = static_cast<double>(x) - px;
    double dy = static_cast<double>(y) - py;

    return dx * dx + dy * dy <= tolerance * tolerance;
}

bool item_polyline::contains(int x, int y) const
{
    if(points.size() < 2)
        return false;

    double tolerance = static_cast<double>(thickness + 2);

    for(size_t i = 0; i + 1 < points.size(); ++i)
    {
        if(pointNearSegment(
            x,
            y,
            points[i],
            points[i + 1],
            tolerance))
        {
            return true;
        }
    }

    return false;
}

void item_polyline::draw()
{
    if(!isVisible() || points.size() < 2)
        return;

    int color = lineColor.getRGB();

    for(size_t i = 0; i + 1 < points.size(); ++i)
    {
        drawThickLine(
            points[i].x,
            points[i].y,
            points[i + 1].x,
            points[i + 1].y,
            color,
            thickness
        );
    }
}

void item_polyline::translate(int dx, int dy)
{
    Transform2D transform;
    transform.translate(dx, dy);
    applyTransform(transform);
}

void item_polyline::rotate(float degrees)
{
    rotate(
        degrees,
        STRUCT_point{
            getX() + getW() / 2,
            getY() + getH() / 2
        }
    );
}

void item_polyline::rotate(float degrees, STRUCT_point center)
{
    Transform2D transform;
    transform.rotate(degrees, center.x, center.y);
    applyTransform(transform);
}

void item_polyline::scale(float sx, float sy)
{
    scale(
        sx,
        sy,
        STRUCT_point{
            getX() + getW() / 2,
            getY() + getH() / 2
        }
    );
}

void item_polyline::scale(
    float sx,
    float sy,
    STRUCT_point center)
{
    Transform2D transform;
    transform.scale(sx, sy, center.x, center.y);
    applyTransform(transform);
}

void item_polyline::mirrorX()
{
    mirrorX(getX() + getW() / 2);
}

void item_polyline::mirrorX(int axisX)
{
    Transform2D transform;
    transform.mirrorX(axisX);
    applyTransform(transform);
}

void item_polyline::mirrorY()
{
    mirrorY(getY() + getH() / 2);
}

void item_polyline::mirrorY(int axisY)
{
    Transform2D transform;
    transform.mirrorY(axisY);
    applyTransform(transform);
}

void item_polyline::applyTransform(const Transform2D& transform)
{
    for(auto& point : points)
        point = transform.apply(point);

    updateBoundingBox();
}

static std::vector<STRUCT_point> buildArcPointsInternal(
    int centerX,
    int centerY,
    int radius,
    float startDegrees,
    float endDegrees,
    unsigned int segments)
{
    if(radius < 1)
        radius = 1;

    if(segments < 2)
        segments = 2;

    float span = endDegrees - startDegrees;

    if(span == 0.0f)
        span = 360.0f;

    const float pi = 3.14159265358979323846f;
    std::vector<STRUCT_point> result;
    result.reserve(segments + 1);

    for(unsigned int i = 0; i <= segments; ++i)
    {
        float ratio =
            static_cast<float>(i) /
            static_cast<float>(segments);

        float degrees = startDegrees + span * ratio;
        float radians = degrees * pi / 180.0f;

        result.push_back(
            STRUCT_point{
                centerX + static_cast<int>(cosf(radians) * radius),
                centerY + static_cast<int>(sinf(radians) * radius)
            }
        );
    }

    return result;
}

std::vector<STRUCT_point> item_arc::buildPoints(
    int centerX,
    int centerY,
    int radius,
    float startDegrees,
    float endDegrees,
    unsigned int segments)
{
    return buildArcPointsInternal(
        centerX,
        centerY,
        radius,
        startDegrees,
        endDegrees,
        segments
    );
}

item_arc::item_arc(
    int _centerX,
    int _centerY,
    int _radius,
    float _startDegrees,
    float _endDegrees,
    Class_color color,
    int thickness,
    unsigned int _segments,
    int zOrder)
    : item_polyline(
        buildPoints(
            _centerX,
            _centerY,
            _radius,
            _startDegrees,
            _endDegrees,
            _segments),
        color,
        thickness,
        zOrder)
{
    centerX = _centerX;
    centerY = _centerY;
    radius = (_radius > 0) ? _radius : 1;
    startDegrees = _startDegrees;
    endDegrees = _endDegrees;
    segments = (_segments >= 2) ? _segments : 2;
}

void item_arc::setGeometry(
    int _centerX,
    int _centerY,
    int _radius,
    float _startDegrees,
    float _endDegrees)
{
    centerX = _centerX;
    centerY = _centerY;
    radius = (_radius > 0) ? _radius : 1;
    startDegrees = _startDegrees;
    endDegrees = _endDegrees;

    setPoints(
        buildPoints(
            centerX,
            centerY,
            radius,
            startDegrees,
            endDegrees,
            segments)
    );
}

std::vector<STRUCT_point> item_sector::buildPoints(
    int centerX,
    int centerY,
    int radius,
    float startDegrees,
    float endDegrees,
    unsigned int segments)
{
    std::vector<STRUCT_point> arc =
        buildArcPointsInternal(
            centerX,
            centerY,
            radius,
            startDegrees,
            endDegrees,
            segments
        );

    std::vector<STRUCT_point> result;
    result.reserve(arc.size() + 1);
    result.push_back(STRUCT_point{centerX, centerY});

    for(const auto& point : arc)
        result.push_back(point);

    return result;
}

item_sector::item_sector(
    int _centerX,
    int _centerY,
    int _radius,
    float _startDegrees,
    float _endDegrees,
    Class_color fillColor,
    Class_color borderColor,
    item_draw_mode mode,
    int borderSize,
    unsigned int _segments,
    int zOrder)
    : item_polygon(
        buildPoints(
            _centerX,
            _centerY,
            _radius,
            _startDegrees,
            _endDegrees,
            _segments),
        fillColor,
        borderColor,
        mode,
        borderSize,
        zOrder)
{
    centerX = _centerX;
    centerY = _centerY;
    radius = (_radius > 0) ? _radius : 1;
    startDegrees = _startDegrees;
    endDegrees = _endDegrees;
    segments = (_segments >= 2) ? _segments : 2;
}

void item_sector::setGeometry(
    int _centerX,
    int _centerY,
    int _radius,
    float _startDegrees,
    float _endDegrees)
{
    centerX = _centerX;
    centerY = _centerY;
    radius = (_radius > 0) ? _radius : 1;
    startDegrees = _startDegrees;
    endDegrees = _endDegrees;

    setPoints(
        buildPoints(
            centerX,
            centerY,
            radius,
            startDegrees,
            endDegrees,
            segments)
    );
}

static std::vector<STRUCT_point> roundedRectanglePoints(
    STRUCT_pos bounds,
    int radius,
    unsigned int segmentsPerCorner)
{
    int width = (bounds.w > 0) ? bounds.w : 1;
    int height = (bounds.h > 0) ? bounds.h : 1;
    int maxRadius = ((width < height) ? width : height) / 2;

    if(radius < 0)
        radius = 0;

    if(radius > maxRadius)
        radius = maxRadius;

    if(segmentsPerCorner < 1)
        segmentsPerCorner = 1;

    if(radius == 0)
    {
        return {
            STRUCT_point{bounds.x, bounds.y},
            STRUCT_point{bounds.x + width - 1, bounds.y},
            STRUCT_point{bounds.x + width - 1, bounds.y + height - 1},
            STRUCT_point{bounds.x, bounds.y + height - 1}
        };
    }

    const float pi = 3.14159265358979323846f;
    std::vector<STRUCT_point> result;
    result.reserve(segmentsPerCorner * 4 + 4);

    const int cx[4] = {
        bounds.x + width - 1 - radius,
        bounds.x + width - 1 - radius,
        bounds.x + radius,
        bounds.x + radius
    };

    const int cy[4] = {
        bounds.y + radius,
        bounds.y + height - 1 - radius,
        bounds.y + height - 1 - radius,
        bounds.y + radius
    };

    const float start[4] = {-90.0f, 0.0f, 90.0f, 180.0f};

    for(int corner = 0; corner < 4; ++corner)
    {
        for(unsigned int i = 0; i <= segmentsPerCorner; ++i)
        {
            float ratio =
                static_cast<float>(i) /
                static_cast<float>(segmentsPerCorner);

            float degrees = start[corner] + 90.0f * ratio;
            float radians = degrees * pi / 180.0f;

            result.push_back(
                STRUCT_point{
                    cx[corner] + static_cast<int>(cosf(radians) * radius),
                    cy[corner] + static_cast<int>(sinf(radians) * radius)
                }
            );
        }
    }

    return result;
}

item_rounded_rectangle::item_rounded_rectangle(
    STRUCT_pos _bounds,
    int _radius,
    Class_color fillColor,
    Class_color borderColor,
    item_draw_mode mode,
    int borderSize,
    unsigned int _segmentsPerCorner,
    int zOrder)
    : item_polygon(
        roundedRectanglePoints(
            _bounds,
            _radius,
            _segmentsPerCorner),
        fillColor,
        borderColor,
        mode,
        borderSize,
        zOrder)
{
    bounds = _bounds;
    radius = _radius;
    segmentsPerCorner =
        (_segmentsPerCorner > 0)
        ? _segmentsPerCorner
        : 1;
}

void item_rounded_rectangle::rebuild()
{
    setPoints(
        roundedRectanglePoints(
            bounds,
            radius,
            segmentsPerCorner)
    );
}

void item_rounded_rectangle::setBounds(STRUCT_pos value)
{
    bounds = value;
    rebuild();
}

void item_rounded_rectangle::setRadius(int value)
{
    radius = value;
    rebuild();
}


//******************************** Toggle button *********************************

item_toggle_button::item_toggle_button(
    STRUCT_pos _pos,
    itemEvent _event,
    item_status _status,
    itemColor _colors,
    item_label _label)
    : item_button(
        _pos,
        _event,
        _status,
        _colors,
        _label)
{
    param.status.toggle = true;
}

void item_toggle_button::handleEvent(int eventType, int eventKey)
{
    if(
        eventType == KEYEV_DOWN &&
        eventKey == event.keyEvent &&
        param.status.hover &&
        !param.status.dimmed &&
        param.status.visible)
    {
        param.status.ON = !param.status.ON;

        // Do not let the transient click palette mask the newly selected
        // ON/OFF palette until another cursor/key event occurs.
        setClicked(false);
    }
}

//******************************** Textbox *********************************

item_textbox::item_textbox(
    STRUCT_pos _pos,
    std::string _text,
    std::string _placeholder,
    unsigned int _maxLength,
    int _zOrder,
    item_textbox_input_mode _inputMode)
{
    param.pos = _pos;
    param.status.visible = true;

    text = _text;
    placeholder = _placeholder;
    maxLength = (_maxLength > 0) ? _maxLength : 1;
    zOrder = _zOrder;
    inputMode = _inputMode;

    applyTheme(
        current_item_theme()
    );
}

void item_textbox::applyTheme(
    const item_theme& theme)
{
    item::applyTheme(theme);
}

void item_textbox::setInputMode(
    item_textbox_input_mode value)
{
    inputMode = value;

    if(!focused)
        return;

    inputState.shift = false;

    if(inputMode == item_textbox_input_mode::TEXT_ONLY)
        inputState.alpha = item_alpha_mode::LOCKED;
    else if(inputMode == item_textbox_input_mode::NUMERIC_ONLY)
        inputState.alpha = item_alpha_mode::OFF;
    else
        inputState.alpha = autoAlphaOnFocus
            ? item_alpha_mode::LOCKED
            : item_alpha_mode::OFF;
}

void item_textbox::resetInputModifiers()
{
    inputState.reset();
    capsLock = false;

    if(!focused)
        return;

    if(inputMode == item_textbox_input_mode::TEXT_ONLY)
        inputState.alpha = item_alpha_mode::LOCKED;
    else if(
        inputMode == item_textbox_input_mode::MIXED &&
        autoAlphaOnFocus)
    {
        inputState.alpha = item_alpha_mode::LOCKED;
    }
}

void item_textbox::setAlphaLock(bool value)
{
    if(inputMode == item_textbox_input_mode::NUMERIC_ONLY)
    {
        inputState.alpha = item_alpha_mode::OFF;
        return;
    }

    if(inputMode == item_textbox_input_mode::TEXT_ONLY)
    {
        inputState.alpha = item_alpha_mode::LOCKED;
        return;
    }

    inputState.alpha =
        value
            ? item_alpha_mode::LOCKED
            : item_alpha_mode::OFF;
}

void item_textbox::setFocus(bool value)
{
    bool entering =
        value &&
        !focused;

    focused = value;

    if(!focused)
    {
        inputState.reset();
        capsLock = false;
        return;
    }

    if(!entering)
        return;

    inputState.shift = false;
    capsLock = false;

    if(inputMode == item_textbox_input_mode::TEXT_ONLY)
        inputState.alpha = item_alpha_mode::LOCKED;
    else if(inputMode == item_textbox_input_mode::NUMERIC_ONLY)
        inputState.alpha = item_alpha_mode::OFF;
    else
        inputState.alpha = autoAlphaOnFocus
            ? item_alpha_mode::LOCKED
            : item_alpha_mode::OFF;
}

void item_textbox::setText(
    const std::string& value)
{
    text = value;

    if(text.size() > maxLength)
        text.resize(maxLength);

    notifyTextChanged();
}

void item_textbox::clear()
{
    if(text.empty())
        return;

    text.clear();
    notifyTextChanged();
}

void item_textbox::notifyTextChanged()
{
    if(onTextChanged)
        onTextChanged(text);
}

char item_textbox::keyToChar(int key) const
{
    if(keyMapper)
    {
        char mapped =
            keyMapper(
                key,
                inputState
            );

        if(mapped != 0)
            return mapped;
    }

    if(isTextEntryMode())
    {
        item_text_input_state mappingState =
            inputState;

        mappingState.alpha =
            item_alpha_mode::LOCKED;

        mappingState.shift = false;

        char c =
            item_text_key_to_char(
                key,
                mappingState
            );

        if(c >= 'A' && c <= 'Z')
        {
            if(!capsLock)
            {
                c =
                    static_cast<char>(
                        c - 'A' + 'a'
                    );
            }

            return c;
        }

        return 0;
    }

    item_text_input_state numericState;
    numericState.alpha = item_alpha_mode::OFF;
    numericState.shift = false;

    char c =
        item_text_key_to_char(
            key,
            numericState
        );

    if(
        (c >= '0' && c <= '9') ||
        c == '.' ||
        c == '-' ||
        c == '+')
    {
        return c;
    }

    return 0;
}

void item_textbox::handleEvent(
    int eventType,
    int eventKey)
{
    if(eventType != KEYEV_DOWN)
        return;

    if(eventKey == KEY_EXE && param.status.hover)
    {
        setFocus(true);
        return;
    }

    if(!focused)
        return;

    if(eventKey == KEY_EXIT)
    {
        setFocus(false);
        return;
    }

    if(eventKey == KEY_ALPHA)
    {
        if(inputMode == item_textbox_input_mode::MIXED)
        {
            inputState.alpha =
                inputState.isAlpha()
                    ? item_alpha_mode::OFF
                    : item_alpha_mode::LOCKED;

            inputState.shift = false;
        }

        return;
    }

    if(eventKey == KEY_SHIFT)
    {
        if(isTextEntryMode())
            capsLock = !capsLock;

        return;
    }

    if(eventKey == KEY_DEL)
    {
        if(!text.empty())
        {
            text.pop_back();
            notifyTextChanged();
        }

        return;
    }

    if(eventKey == KEY_ACON)
    {
        clear();
        return;
    }

    char c =
        keyToChar(eventKey);

    if(c != 0 && text.size() < maxLength)
    {
        text += c;
        notifyTextChanged();
    }
}

void item_textbox::draw()
{
    if(!isVisible())
        return;

    const item_theme& theme =
        current_item_theme();

    int x1 = getX();
    int y1 = getY();
    int x2 = x1 + getW() - 1;
    int y2 = y1 + getH() - 1;

    int borderColor =
        focused
            ? theme.accent.getRGB()
            : (
                param.status.hover
                    ? theme.accent.getRGB()
                    : theme.border.getRGB()
            );

    drect(
        x1,
        y1,
        x2,
        y2,
        isDimmed()
            ? theme.surfaceAlt.getRGB()
            : theme.surface.getRGB()
    );

    drawOutlineRect(
        x1,
        y1,
        x2,
        y2,
        borderColor,
        focused
            ? theme.focusBorderSize
            : theme.borderSize
    );

    bool showingPlaceholder =
        text.empty() &&
        !placeholder.empty();

    const char* value =
        showingPlaceholder
            ? placeholder.c_str()
            : text.c_str();

    dtext(
        x1 + 4,
        y1 + 6,
        showingPlaceholder
            ? theme.textMuted.getRGB()
            : (
                isDimmed()
                    ? theme.disabled.getRGB()
                    : theme.text.getRGB()
            ),
        value
    );

    if(focused)
    {
        const char* mode =
            isNumericEntryMode()
                ? "123"
                : (
                    capsLock
                        ? "ABC"
                        : "abc"
                );

        dtext(
            x2 - 28,
            y1 + 6,
            theme.accent.getRGB(),
            mode
        );
    }
}

//******************************** Numeric *********************************

item_numeric::item_numeric(
    STRUCT_pos _pos,
    int _value,
    int _minValue,
    int _maxValue,
    int _step,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;

    minValue = _minValue;
    maxValue = _maxValue;

    if(maxValue < minValue)
    {
        int swap = minValue;
        minValue = maxValue;
        maxValue = swap;
    }

    step = (_step > 0) ? _step : 1;
    value = _value;
    setValue(value);

    zOrder = _zOrder;
}

void item_numeric::setValue(int _value)
{
    if(_value < minValue)
        _value = minValue;

    if(_value > maxValue)
        _value = maxValue;

    if(value == _value)
        return;

    value = _value;

    if(onChanged)
        onChanged(value);
}

void item_numeric::setRange(int _minValue, int _maxValue)
{
    minValue = _minValue;
    maxValue = _maxValue;

    if(maxValue < minValue)
    {
        int swap = minValue;
        minValue = maxValue;
        maxValue = swap;
    }

    setValue(value);
}

void item_numeric::handleEvent(int eventType, int eventKey)
{
    if(eventType != KEYEV_DOWN && eventType != KEYEV_HOLD)
        return;

    if(eventType == KEYEV_DOWN &&
       eventKey == KEY_EXE &&
       param.status.hover)
    {
        active = true;
        return;
    }

    if(!active)
        return;

    if(eventKey == KEY_UP || eventKey == KEY_RIGHT)
        setValue(value + step);

    if(eventKey == KEY_DOWN || eventKey == KEY_LEFT)
        setValue(value - step);
}

void item_numeric::draw()
{
    if(!isVisible())
        return;

    int x1 = getX();
    int y1 = getY();
    int x2 = x1 + getW() - 1;
    int y2 = y1 + getH() - 1;

    const item_theme& theme =
        current_item_theme();

    drect(
        x1,
        y1,
        x2,
        y2,
        isDimmed()
            ? theme.surfaceAlt.getRGB()
            : theme.surface.getRGB()
    );

    drawOutlineRect(
        x1,
        y1,
        x2,
        y2,
        active
            ? theme.accent.getRGB()
            : theme.border.getRGB(),
        active
            ? theme.focusBorderSize
            : theme.borderSize
    );

    int numericText =
        isDimmed()
            ? theme.disabled.getRGB()
            : theme.text.getRGB();

    dtext(x1 + 5, y1 + 6, numericText, "-");
    dtext(x2 - 10, y1 + 6, numericText, "+");

    dprint(
        x1 + getW() / 2 - 10,
        y1 + 6,
        numericText,
        "%d",
        value
    );
}

//******************************** Slider *********************************

item_slider::item_slider(
    STRUCT_pos _pos,
    int _value,
    int _minValue,
    int _maxValue,
    int _step,
    int _zOrder,
    item_orientation _orientation)
{
    param.pos = _pos;
    param.status.visible = true;

    minValue = _minValue;
    maxValue = _maxValue;

    if(maxValue < minValue)
    {
        int swap = minValue;
        minValue = maxValue;
        maxValue = swap;
    }

    step = (_step > 0) ? _step : 1;
    orientation = _orientation;
    value = minValue;
    setValue(_value);

    zOrder = _zOrder;
}

void item_slider::setValue(int _value)
{
    if(_value < minValue)
        _value = minValue;

    if(_value > maxValue)
        _value = maxValue;

    if(value == _value)
        return;

    value = _value;

    if(onChanged)
        onChanged(value);
}

void item_slider::setRange(int _minValue, int _maxValue)
{
    minValue = _minValue;
    maxValue = _maxValue;

    if(maxValue < minValue)
    {
        int swap = minValue;
        minValue = maxValue;
        maxValue = swap;
    }

    setValue(value);
}

void item_slider::handleEvent(int eventType, int eventKey)
{
    if(eventType != KEYEV_DOWN && eventType != KEYEV_HOLD)
        return;

    if(eventType == KEYEV_DOWN &&
       eventKey == KEY_EXE &&
       param.status.hover)
    {
        active = true;
        return;
    }

    if(!active)
        return;

    if(eventKey == KEY_RIGHT || eventKey == KEY_UP)
        setValue(value + step);

    if(eventKey == KEY_LEFT || eventKey == KEY_DOWN)
        setValue(value - step);
}

void item_slider::draw()
{
    if(!isVisible())
        return;

    int x1 = getX();
    int y1 = getY();
    int x2 = x1 + getW() - 1;
    int y2 = y1 + getH() - 1;

    const item_theme& theme =
        current_item_theme();

    int trackColor =
        isDimmed()
            ? theme.disabled.getRGB()
            : theme.border.getRGB();

    int handleColor =
        isDimmed()
            ? theme.disabled.getRGB()
            : (
                active
                    ? theme.accent.getRGB()
                    : theme.text.getRGB()
            );

    if(orientation == item_orientation::VERTICAL)
    {
        int centerX = x1 + getW() / 2;
        int top = y1 + 6;
        int bottom = y2 - 6;

        dline(
            centerX,
            top,
            centerX,
            bottom,
            trackColor
        );

        int handleY = bottom;

        if(maxValue != minValue)
        {
            handleY =
                bottom -
                (value - minValue) * (bottom - top) /
                (maxValue - minValue);
        }

        drect(
            centerX - 6,
            handleY - 3,
            centerX + 6,
            handleY + 3,
            handleColor
        );
    }
    else
    {
        int centerY = y1 + getH() / 2;
        int left = x1 + 6;
        int right = x2 - 6;

        dline(
            left,
            centerY,
            right,
            centerY,
            trackColor
        );

        int handleX = left;

        if(maxValue != minValue)
        {
            handleX =
                left +
                (value - minValue) * (right - left) /
                (maxValue - minValue);
        }

        drect(
            handleX - 3,
            centerY - 6,
            handleX + 3,
            centerY + 6,
            handleColor
        );
    }

    drawOutlineRect(
        x1,
        y1,
        x2,
        y2,
        active || param.status.hover
            ? theme.accent.getRGB()
            : theme.surfaceAlt.getRGB(),
        active
            ? theme.focusBorderSize
            : theme.borderSize
    );
}


//******************************** Extended controls *********************************

item_checkbox::item_checkbox(
    STRUCT_pos _pos,
    std::string _label,
    bool _checked,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;
    label = _label;
    checked = _checked;
    zOrder = _zOrder;
}

void item_checkbox::setChecked(bool value)
{
    if(checked == value)
        return;

    checked = value;

    if(onChanged)
        onChanged(checked);
}

void item_checkbox::toggle()
{
    setChecked(!checked);
}

void item_checkbox::handleEvent(int eventType, int eventKey)
{
    if(
        eventType == KEYEV_DOWN &&
        eventKey == KEY_EXE &&
        param.status.hover &&
        !isDimmed())
    {
        toggle();
        setClicked(false);
    }
}

void item_checkbox::draw()
{
    if(!isVisible())
        return;

    const item_theme& theme =
        current_item_theme();

    int x1 = getX();
    int y1 = getY();
    int boxSize = getH() - 6;

    if(boxSize > 18)
        boxSize = 18;

    if(boxSize < 10)
        boxSize = 10;

    int boxX1 = x1 + 3;
    int boxY1 = y1 + (getH() - boxSize) / 2;
    int boxX2 = boxX1 + boxSize - 1;
    int boxY2 = boxY1 + boxSize - 1;

    drect(
        boxX1,
        boxY1,
        boxX2,
        boxY2,
        theme.surface.getRGB()
    );

    drawOutlineRect(
        boxX1,
        boxY1,
        boxX2,
        boxY2,
        param.status.hover
            ? theme.accent.getRGB()
            : theme.border.getRGB(),
        theme.borderSize
    );

    if(checked)
    {
        dline(
            boxX1 + 3,
            boxY1 + boxSize / 2,
            boxX1 + boxSize / 2 - 1,
            boxY2 - 3,
            theme.text.getRGB()
        );

        dline(
            boxX1 + boxSize / 2 - 1,
            boxY2 - 3,
            boxX2 - 3,
            boxY1 + 3,
            theme.text.getRGB()
        );
    }

    if(!label.empty())
    {
        dtext(
            boxX2 + 6,
            y1 + (getH() - 12) / 2,
            isDimmed()
                ? theme.disabled.getRGB()
                : theme.text.getRGB(),
            label.c_str()
        );
    }

    if(param.status.hover)
    {
        drawOutlineRect(
            x1,
            y1,
            getX() + getW() - 1,
            getY() + getH() - 1,
            theme.accent.getRGB(),
            theme.borderSize
        );
    }
}


//******************************** Progress bar *********************************

item_progress_bar::item_progress_bar(
    STRUCT_pos _pos,
    int _value,
    int _minValue,
    int _maxValue,
    item_orientation _orientation,
    bool _showPercent,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;

    minValue = _minValue;
    maxValue = _maxValue;

    if(maxValue < minValue)
    {
        int swap = minValue;
        minValue = maxValue;
        maxValue = swap;
    }

    orientation = _orientation;
    showPercent = _showPercent;
    value = minValue;
    setValue(_value);
    zOrder = _zOrder;

    applyTheme(
        current_item_theme()
    );
}

void item_progress_bar::applyTheme(
    const item_theme& theme)
{
    item::applyTheme(theme);

    progressColor = theme.accent;
    backgroundColor = theme.surfaceAlt;
    borderColor = theme.border;
}

void item_progress_bar::setValue(int _value)
{
    if(_value < minValue)
        _value = minValue;

    if(_value > maxValue)
        _value = maxValue;

    value = _value;
}

void item_progress_bar::setRange(int _minValue, int _maxValue)
{
    minValue = _minValue;
    maxValue = _maxValue;

    if(maxValue < minValue)
    {
        int swap = minValue;
        minValue = maxValue;
        maxValue = swap;
    }

    setValue(value);
}

void item_progress_bar::draw()
{
    if(!isVisible())
        return;

    int x1 = getX();
    int y1 = getY();
    int x2 = getX() + getW() - 1;
    int y2 = getY() + getH() - 1;

    drect(
        x1,
        y1,
        x2,
        y2,
        backgroundColor.getRGB()
    );

    int ratio = 0;

    if(maxValue != minValue)
    {
        ratio =
            (value - minValue) * 1000 /
            (maxValue - minValue);
    }

    if(ratio < 0) ratio = 0;
    if(ratio > 1000) ratio = 1000;

    if(orientation == item_orientation::VERTICAL)
    {
        int fillHeight =
            (getH() - 2) * ratio / 1000;

        if(fillHeight > 0)
        {
            drect(
                x1 + 1,
                y2 - fillHeight,
                x2 - 1,
                y2 - 1,
                progressColor.getRGB()
            );
        }
    }
    else
    {
        int fillWidth =
            (getW() - 2) * ratio / 1000;

        if(fillWidth > 0)
        {
            drect(
                x1 + 1,
                y1 + 1,
                x1 + fillWidth,
                y2 - 1,
                progressColor.getRGB()
            );
        }
    }

    drawOutlineRect(
        x1,
        y1,
        x2,
        y2,
        borderColor.getRGB(),
        1
    );

    if(showPercent)
    {
        int percent = ratio / 10;

        dprint(
            x1 + getW() / 2 - 12,
            y1 + getH() / 2 - 6,
            current_item_theme().text.getRGB(),
            "%d%%",
            percent
        );
    }
}


//******************************** Scrollbar *********************************

item_scrollbar::item_scrollbar(
    STRUCT_pos _pos,
    int _position,
    int _minValue,
    int _maxValue,
    int _pageSize,
    int _step,
    item_orientation _orientation,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;

    minValue = _minValue;
    maxValue = _maxValue;

    if(maxValue < minValue)
    {
        int swap = minValue;
        minValue = maxValue;
        maxValue = swap;
    }

    pageSize = (_pageSize > 0) ? _pageSize : 1;
    step = (_step > 0) ? _step : 1;
    orientation = _orientation;
    position = minValue;
    setPosition(_position);
    zOrder = _zOrder;
}

void item_scrollbar::setPosition(int value)
{
    if(value < minValue)
        value = minValue;

    if(value > maxValue)
        value = maxValue;

    if(position == value)
        return;

    position = value;

    if(onScroll)
        onScroll(position);
}

void item_scrollbar::setRange(int _minValue, int _maxValue)
{
    minValue = _minValue;
    maxValue = _maxValue;

    if(maxValue < minValue)
    {
        int swap = minValue;
        minValue = maxValue;
        maxValue = swap;
    }

    setPosition(position);
}

int item_scrollbar::thumbLength() const
{
    int trackLength =
        orientation == item_orientation::VERTICAL
            ? getH() - 4
            : getW() - 4;

    if(trackLength < 1)
        return 1;

    int range = maxValue - minValue;

    if(range <= 0)
        return trackLength;

    int length =
        trackLength * pageSize /
        (range + pageSize);

    if(length < 8)
        length = 8;

    if(length > trackLength)
        length = trackLength;

    return length;
}

int item_scrollbar::thumbOffset() const
{
    int trackLength =
        orientation == item_orientation::VERTICAL
            ? getH() - 4
            : getW() - 4;

    int length = thumbLength();
    int available = trackLength - length;

    if(available <= 0 || maxValue == minValue)
        return 0;

    return
        (position - minValue) *
        available /
        (maxValue - minValue);
}

void item_scrollbar::setPositionFromPointer()
{
    int trackStart;
    int coordinate;
    int trackLength;

    if(orientation == item_orientation::VERTICAL)
    {
        trackStart = getY() + 2;
        coordinate = getPointerY();
        trackLength = getH() - 4;
    }
    else
    {
        trackStart = getX() + 2;
        coordinate = getPointerX();
        trackLength = getW() - 4;
    }

    int length = thumbLength();
    int available = trackLength - length;

    if(available <= 0 || maxValue == minValue)
    {
        setPosition(minValue);
        return;
    }

    int local =
        coordinate -
        trackStart -
        length / 2;

    if(local < 0)
        local = 0;

    if(local > available)
        local = available;

    int newPosition =
        minValue +
        local * (maxValue - minValue) /
        available;

    setPosition(newPosition);
}

void item_scrollbar::handleEvent(int eventType, int eventKey)
{
    if(eventType != KEYEV_DOWN && eventType != KEYEV_HOLD)
        return;

    if(
        eventType == KEYEV_DOWN &&
        eventKey == KEY_EXE &&
        param.status.hover)
    {
        active = true;
        setPositionFromPointer();
        return;
    }

    if(!active)
        return;

    if(orientation == item_orientation::VERTICAL)
    {
        if(eventKey == KEY_UP)
            setPosition(position - step);

        if(eventKey == KEY_DOWN)
            setPosition(position + step);
    }
    else
    {
        if(eventKey == KEY_LEFT)
            setPosition(position - step);

        if(eventKey == KEY_RIGHT)
            setPosition(position + step);
    }
}

void item_scrollbar::draw()
{
    if(!isVisible())
        return;

    const item_theme& theme =
        current_item_theme();

    int x1 = getX();
    int y1 = getY();
    int x2 = getX() + getW() - 1;
    int y2 = getY() + getH() - 1;

    drect(
        x1,
        y1,
        x2,
        y2,
        theme.surfaceAlt.getRGB()
    );

    drawOutlineRect(
        x1,
        y1,
        x2,
        y2,
        active
            ? theme.accent.getRGB()
            : theme.border.getRGB(),
        active
            ? theme.focusBorderSize
            : theme.borderSize
    );

    int offset = thumbOffset();
    int length = thumbLength();

    if(orientation == item_orientation::VERTICAL)
    {
        int thumbY = y1 + 2 + offset;

        drect(
            x1 + 3,
            thumbY,
            x2 - 3,
            thumbY + length - 1,
            active ? theme.accent.getRGB() : theme.textMuted.getRGB()
        );
    }
    else
    {
        int thumbX = x1 + 2 + offset;

        drect(
            thumbX,
            y1 + 3,
            thumbX + length - 1,
            y2 - 3,
            active ? theme.accent.getRGB() : theme.textMuted.getRGB()
        );
    }
}


//******************************** LED indicator *********************************

item_led::item_led(
    STRUCT_pos _pos,
    item_led_state _state,
    std::string _label,
    item_led_shape _shape,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;
    state = _state;
    label = _label;
    shape = _shape;
    zOrder = _zOrder;
    lastBlinkTicks = rtc_ticks();
}

Class_color item_led::currentColor() const
{
    switch(state)
    {
        case item_led_state::ON:
            return onColor;

        case item_led_state::WARNING:
            return warningColor;

        case item_led_state::ERROR:
            return errorColor;

        case item_led_state::OFF:
        default:
            return offColor;
    }
}

void item_led::setState(item_led_state value)
{
    if(state == value)
        return;

    state = value;

    if(onChanged)
        onChanged(state);
}

void item_led::setBlink(bool enabled, uint32_t intervalMs)
{
    blinkEnabled = enabled;
    blinkIntervalMs = (intervalMs > 0) ? intervalMs : 1;
    blinkVisible = true;
    lastBlinkTicks = rtc_ticks();
}

void item_led::update(uint32_t nowTicks)
{
    if(!blinkEnabled || state == item_led_state::OFF)
    {
        blinkVisible = true;
        lastBlinkTicks = nowTicks;
        return;
    }

    uint32_t intervalTicks =
        static_cast<uint32_t>(
            (
                static_cast<uint64_t>(blinkIntervalMs) *
                128u +
                999u
            ) /
            1000u
        );

    if(intervalTicks < 1)
        intervalTicks = 1;

    if(static_cast<uint32_t>(nowTicks - lastBlinkTicks) >= intervalTicks)
    {
        blinkVisible = !blinkVisible;
        lastBlinkTicks = nowTicks;
    }
}

void item_led::draw()
{
    if(!isVisible())
        return;

    const item_theme& theme =
        current_item_theme();

    int x1 = getX();
    int y1 = getY();
    int indicatorSize = getH();

    if(indicatorSize > getW())
        indicatorSize = getW();

    if(!label.empty() && indicatorSize > 20)
        indicatorSize = 20;

    if(indicatorSize < 6)
        indicatorSize = 6;

    int cx = x1 + indicatorSize / 2;
    int cy = y1 + getH() / 2;
    int color =
        blinkVisible
            ? currentColor().getRGB()
            : offColor.getRGB();

    if(shape == item_led_shape::RECTANGLE)
    {
        drect(
            x1 + 2,
            cy - indicatorSize / 2 + 2,
            x1 + indicatorSize - 3,
            cy + indicatorSize / 2 - 3,
            color
        );

        drawOutlineRect(
            x1 + 1,
            cy - indicatorSize / 2 + 1,
            x1 + indicatorSize - 2,
            cy + indicatorSize / 2 - 2,
            theme.border.getRGB(),
            theme.borderSize
        );
    }
    else
    {
        int radius = indicatorSize / 2 - 2;

        if(radius < 2)
            radius = 2;

        for(int y = -radius; y <= radius; ++y)
        {
            int extent = static_cast<int>(
                sqrt(
                    static_cast<double>(
                        radius * radius - y * y
                    )
                )
            );

            dline(
                cx - extent,
                cy + y,
                cx + extent,
                cy + y,
                color
            );
        }

        // Small black outline approximation.
        for(int i = 0; i < 48; ++i)
        {
            float angle =
                6.28318530717958647692f *
                static_cast<float>(i) /
                48.0f;

            dpixel(
                cx + static_cast<int>(cosf(angle) * radius),
                cy + static_cast<int>(sinf(angle) * radius),
                theme.border.getRGB()
            );
        }
    }

    if(!label.empty())
    {
        dtext(
            x1 + indicatorSize + 5,
            y1 + (getH() - 12) / 2,
            isDimmed()
                ? theme.disabled.getRGB()
                : theme.text.getRGB(),
            label.c_str()
        );
    }
}


//******************************** Color wheel *********************************

static Class_color itemColorWheelHueToColor(float hue)
{
    while(hue < 0.0f)
        hue += 360.0f;

    while(hue >= 360.0f)
        hue -= 360.0f;

    float h = hue / 60.0f;
    int sector = static_cast<int>(h);
    float fraction = h - static_cast<float>(sector);

    int rising =
        static_cast<int>(
            fraction * 255.0f + 0.5f
        );

    int falling = 255 - rising;

    switch(sector)
    {
        case 0:
            return Class_color(255, rising, 0);

        case 1:
            return Class_color(falling, 255, 0);

        case 2:
            return Class_color(0, 255, rising);

        case 3:
            return Class_color(0, falling, 255);

        case 4:
            return Class_color(rising, 0, 255);

        case 5:
        default:
            return Class_color(255, 0, falling);
    }
}

item_color_wheel::item_color_wheel(
    STRUCT_pos _pos,
    float _hue,
    float _step,
    int _zOrder,
    unsigned int _segments)
{
    param.pos = _pos;
    param.status.visible = true;

    hue = normalizeHue(_hue);
    step = (_step > 0.0f) ? _step : 1.0f;
    segments = (_segments >= 24) ? _segments : 24;
    zOrder = _zOrder;

    rebuildColorCache();
}

float item_color_wheel::normalizeHue(float value)
{
    while(value < 0.0f)
        value += 360.0f;

    while(value >= 360.0f)
        value -= 360.0f;

    return value;
}

Class_color item_color_wheel::hueToColor(float value)
{
    return itemColorWheelHueToColor(value);
}

void item_color_wheel::rebuildColorCache()
{
    wheelColors.clear();
    wheelCos.clear();
    wheelSin.clear();

    wheelColors.reserve(segments);
    wheelCos.reserve(segments);
    wheelSin.reserve(segments);

    for(unsigned int i = 0; i < segments; ++i)
    {
        float ratio =
            static_cast<float>(i) /
            static_cast<float>(segments);

        float currentHue =
            360.0f *
            ratio;

        float angle =
            6.28318530717958647692f *
            ratio;

        wheelColors.push_back(
            hueToColor(currentHue).getRGB()
        );

        wheelCos.push_back(
            cosf(angle)
        );

        wheelSin.push_back(
            sinf(angle)
        );
    }
}

void item_color_wheel::setHue(float value)
{
    float normalized = normalizeHue(value);

    if(hue == normalized)
        return;

    hue = normalized;

    if(onChanged)
        onChanged(hue, getColor());
}

Class_color item_color_wheel::getColor() const
{
    return hueToColor(hue);
}

void item_color_wheel::setRingRatio(float value)
{
    if(value < 0.15f)
        value = 0.15f;

    if(value > 0.90f)
        value = 0.90f;

    ringRatio = value;
}

bool item_color_wheel::contains(int x, int y) const
{
    int width = getW();
    int height = getH();

    if(width <= 0 || height <= 0)
        return false;

    int radius =
        ((width < height) ? width : height) / 2 - 2;

    if(radius < 3)
        return false;

    int innerRadius =
        static_cast<int>(
            static_cast<float>(radius) *
            ringRatio
        );

    int cx = getX() + width / 2;
    int cy = getY() + height / 2;

    long dx = x - cx;
    long dy = y - cy;
    long distance2 = dx * dx + dy * dy;

    return
        distance2 <= radius * radius &&
        distance2 >= innerRadius * innerRadius;
}

bool item_color_wheel::isHover(int x, int y)
{
    pointerX = x;
    pointerY = y;
    param.status.hover = contains(x, y);
    return param.status.hover;
}

void item_color_wheel::setHueFromPointer()
{
    int cx = getX() + getW() / 2;
    int cy = getY() + getH() / 2;

    float angle =
        atan2f(
            static_cast<float>(getPointerY() - cy),
            static_cast<float>(getPointerX() - cx)
        ) *
        180.0f /
        3.14159265358979323846f;

    setHue(angle);
}

void item_color_wheel::handleEvent(int eventType, int eventKey)
{
    if(eventType != KEYEV_DOWN && eventType != KEYEV_HOLD)
        return;

    if(
        eventType == KEYEV_DOWN &&
        eventKey == KEY_EXE &&
        param.status.hover)
    {
        focused = true;
        setHueFromPointer();
        return;
    }

    if(!focused)
        return;

    if(eventKey == KEY_LEFT || eventKey == KEY_DOWN)
        setHue(hue - step);

    if(eventKey == KEY_RIGHT || eventKey == KEY_UP)
        setHue(hue + step);
}

void item_color_wheel::draw()
{
    if(!isVisible())
        return;

    int width = getW();
    int height = getH();

    if(width <= 0 || height <= 0)
        return;

    int radius =
        ((width < height) ? width : height) / 2 - 2;

    if(radius < 3)
        return;

    int innerRadius =
        static_cast<int>(
            static_cast<float>(radius) *
            ringRatio
        );

    int cx = getX() + width / 2;
    int cy = getY() + height / 2;

    if(
        wheelColors.size() != segments ||
        wheelCos.size() != segments ||
        wheelSin.size() != segments)
    {
        rebuildColorCache();
    }

    for(unsigned int i = 0; i < segments; ++i)
    {
        float cs = wheelCos[i];
        float sn = wheelSin[i];

        dline(
            cx + static_cast<int>(cs * innerRadius),
            cy + static_cast<int>(sn * innerRadius),
            cx + static_cast<int>(cs * radius),
            cy + static_cast<int>(sn * radius),
            wheelColors[i]
        );
    }

    float markerAngle =
        hue *
        3.14159265358979323846f /
        180.0f;

    float cs = cosf(markerAngle);
    float sn = sinf(markerAngle);

    int markerInner = innerRadius - 2;
    int markerOuter = radius + 1;

    int nx = static_cast<int>(-sn);
    int ny = static_cast<int>(cs);

    dline(
        cx + static_cast<int>(cs * markerInner) + nx,
        cy + static_cast<int>(sn * markerInner) + ny,
        cx + static_cast<int>(cs * markerOuter) + nx,
        cy + static_cast<int>(sn * markerOuter) + ny,
        C_WHITE
    );

    dline(
        cx + static_cast<int>(cs * markerInner) - nx,
        cy + static_cast<int>(sn * markerInner) - ny,
        cx + static_cast<int>(cs * markerOuter) - nx,
        cy + static_cast<int>(sn * markerOuter) - ny,
        C_WHITE
    );

    dline(
        cx + static_cast<int>(cs * markerInner),
        cy + static_cast<int>(sn * markerInner),
        cx + static_cast<int>(cs * markerOuter),
        cy + static_cast<int>(sn * markerOuter),
        C_BLACK
    );

    if(focused)
    {
        drawOutlineRect(
            getX(),
            getY(),
            getX() + getW() - 1,
            getY() + getH() - 1,
            C_BLUE,
            1
        );
    }
}


//******************************** Instruments *********************************

static STRUCT_point gaugePointAtInternal(
    int centerX,
    int centerY,
    int radius,
    float degrees)
{
    const float pi = 3.14159265358979323846f;
    float radians = degrees * pi / 180.0f;

    return STRUCT_point{
        centerX + static_cast<int>(cosf(radians) * radius),
        centerY + static_cast<int>(sinf(radians) * radius)
    };
}

static void drawGaugeArcInternal(
    int centerX,
    int centerY,
    int radius,
    float startAngle,
    float endAngle,
    int color,
    int thickness = 1)
{
    if(radius < 1)
        return;

    if(thickness < 1)
        thickness = 1;

    float span = endAngle - startAngle;

    if(span < 0.0f)
        span = -span;

    unsigned int segments =
        static_cast<unsigned int>(span / 4.0f);

    if(segments < 8)
        segments = 8;

    if(segments > 120)
        segments = 120;

    for(int layer = 0; layer < thickness; ++layer)
    {
        int currentRadius = radius - layer;

        if(currentRadius < 1)
            break;

        STRUCT_point previous =
            gaugePointAtInternal(
                centerX,
                centerY,
                currentRadius,
                startAngle
            );

        for(unsigned int i = 1; i <= segments; ++i)
        {
            float ratio =
                static_cast<float>(i) /
                static_cast<float>(segments);

            float angle =
                startAngle +
                (endAngle - startAngle) * ratio;

            STRUCT_point current =
                gaugePointAtInternal(
                    centerX,
                    centerY,
                    currentRadius,
                    angle
                );

            dline(
                previous.x,
                previous.y,
                current.x,
                current.y,
                color
            );

            previous = current;
        }
    }
}


item_gauge::item_gauge(
    STRUCT_pos _pos,
    int _value,
    int _minValue,
    int _maxValue,
    float _startAngle,
    float _endAngle,
    unsigned int _majorDivisions,
    unsigned int _minorDivisions,
    std::string _unit,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;

    minValue = _minValue;
    maxValue = _maxValue;

    if(maxValue < minValue)
    {
        int swap = minValue;
        minValue = maxValue;
        maxValue = swap;
    }

    startAngle = _startAngle;
    endAngle = _endAngle;

    if(endAngle <= startAngle)
        endAngle = startAngle + 270.0f;

    majorDivisions =
        (_majorDivisions > 0)
            ? _majorDivisions
            : 1;

    minorDivisions = _minorDivisions;
    unit = _unit;
    zOrder = _zOrder;

    value = clampValue(_value);

    applyTheme(
        current_item_theme()
    );
}

void item_gauge::applyTheme(
    const item_theme& theme)
{
    item::applyTheme(theme);

    faceColor = theme.surface;
    scaleColor = theme.text;
    needleColor = theme.accent;
    hubColor = theme.text;
    textColor = theme.text;
}

int item_gauge::clampValue(int _value) const
{
    if(_value < minValue)
        return minValue;

    if(_value > maxValue)
        return maxValue;

    return _value;
}

void item_gauge::setValue(int _value)
{
    value = clampValue(_value);
}

void item_gauge::setRange(
    int _minValue,
    int _maxValue)
{
    minValue = _minValue;
    maxValue = _maxValue;

    if(maxValue < minValue)
    {
        int swap = minValue;
        minValue = maxValue;
        maxValue = swap;
    }

    value = clampValue(value);
}

void item_gauge::setAngles(
    float _startAngle,
    float _endAngle)
{
    startAngle = _startAngle;
    endAngle = _endAngle;

    if(endAngle <= startAngle)
        endAngle = startAngle + 270.0f;
}

void item_gauge::setGraduations(
    unsigned int _majorDivisions,
    unsigned int _minorDivisions)
{
    majorDivisions =
        (_majorDivisions > 0)
            ? _majorDivisions
            : 1;

    minorDivisions = _minorDivisions;
}

void item_gauge::addZone(
    int _minValue,
    int _maxValue,
    Class_color color)
{
    if(_maxValue < _minValue)
    {
        int swap = _minValue;
        _minValue = _maxValue;
        _maxValue = swap;
    }

    gauge_zone zone(
        _minValue,
        _maxValue,
        color
    );

    zones.push_back(zone);
}

float item_gauge::valueToAngle(int _value) const
{
    if(maxValue == minValue)
        return startAngle;

    float ratio =
        static_cast<float>(
            clampValue(_value) - minValue
        ) /
        static_cast<float>(
            maxValue - minValue
        );

    return
        startAngle +
        (endAngle - startAngle) * ratio;
}

int item_gauge::gaugeCenterX() const
{
    return getX() + getW() / 2;
}

int item_gauge::gaugeCenterY() const
{
    return getY() + getH() / 2;
}

int item_gauge::gaugeRadius() const
{
    int radius =
        ((getW() < getH()) ? getW() : getH()) /
        2 - 7;

    return (radius > 8) ? radius : 8;
}

STRUCT_point item_gauge::pointOnGauge(
    float angle,
    int radius) const
{
    return gaugePointAtInternal(
        gaugeCenterX(),
        gaugeCenterY(),
        radius,
        angle
    );
}

void item_gauge::draw()
{
    if(!isVisible())
        return;

    int cx = gaugeCenterX();
    int cy = gaugeCenterY();
    int radius = gaugeRadius();

    int face =
        isDimmed()
            ? C_LIGHT
            : faceColor.getRGB();

    int scale =
        isDimmed()
            ? C_DARK
            : scaleColor.getRGB();

    int needle =
        isDimmed()
            ? C_DARK
            : needleColor.getRGB();

    int hub =
        isDimmed()
            ? C_DARK
            : hubColor.getRGB();

    int text =
        isDimmed()
            ? C_DARK
            : textColor.getRGB();

    drawCircleFilled(
        cx,
        cy,
        radius + 4,
        face
    );

    drawCircleOutline(
        cx,
        cy,
        radius + 4,
        scale
    );

    drawGaugeArcInternal(
        cx,
        cy,
        radius,
        startAngle,
        endAngle,
        C_DARK,
        2
    );

    for(const gauge_zone& zone : zones)
    {
        int zoneMin = zone.minValue;
        int zoneMax = zone.maxValue;

        if(zoneMax < minValue ||
           zoneMin > maxValue)
        {
            continue;
        }

        if(zoneMin < minValue)
            zoneMin = minValue;

        if(zoneMax > maxValue)
            zoneMax = maxValue;

        drawGaugeArcInternal(
            cx,
            cy,
            radius - 2,
            valueToAngle(zoneMin),
            valueToAngle(zoneMax),
            zone.color.getRGB(),
            5
        );
    }

    for(unsigned int major = 0;
        major <= majorDivisions;
        ++major)
    {
        float ratio =
            static_cast<float>(major) /
            static_cast<float>(majorDivisions);

        float angle =
            startAngle +
            (endAngle - startAngle) * ratio;

        STRUCT_point outer =
            pointOnGauge(
                angle,
                radius
            );

        STRUCT_point inner =
            pointOnGauge(
                angle,
                radius - 9
            );

        dline(
            outer.x,
            outer.y,
            inner.x,
            inner.y,
            scale
        );

        if(major >= majorDivisions)
            continue;

        for(unsigned int minor = 1;
            minor <= minorDivisions;
            ++minor)
        {
            float minorRatio =
                (
                    static_cast<float>(major) +
                    static_cast<float>(minor) /
                    static_cast<float>(
                        minorDivisions + 1
                    )
                ) /
                static_cast<float>(
                    majorDivisions
                );

            float minorAngle =
                startAngle +
                (endAngle - startAngle) *
                minorRatio;

            STRUCT_point minorOuter =
                pointOnGauge(
                    minorAngle,
                    radius
                );

            STRUCT_point minorInner =
                pointOnGauge(
                    minorAngle,
                    radius - 5
                );

            dline(
                minorOuter.x,
                minorOuter.y,
                minorInner.x,
                minorInner.y,
                scale
            );
        }
    }

    if(showMinMax)
    {
        STRUCT_point minPoint =
            pointOnGauge(
                startAngle,
                radius - 18
            );

        STRUCT_point maxPoint =
            pointOnGauge(
                endAngle,
                radius - 18
            );

        dprint(
            minPoint.x - 8,
            minPoint.y - 5,
            text,
            "%d",
            minValue
        );

        dprint(
            maxPoint.x - 8,
            maxPoint.y - 5,
            text,
            "%d",
            maxValue
        );
    }

    float needleAngle =
        valueToAngle(value);

    STRUCT_point needlePoint =
        pointOnGauge(
            needleAngle,
            radius - 13
        );

    drawThickLine(
        cx,
        cy,
        needlePoint.x,
        needlePoint.y,
        needle,
        2
    );

    drawCircleFilled(
        cx,
        cy,
        4,
        hub
    );

    drawCircleOutline(
        cx,
        cy,
        4,
        scale
    );

    if(showValue)
    {
        int textY =
            getY() + getH() - 17;

        dprint(
            getX() + 5,
            textY,
            text,
            "%d",
            value
        );

        if(!unit.empty())
        {
            dtext(
                getX() + 42,
                textY,
                text,
                unit.c_str()
            );
        }
    }
}


item_dial::item_dial(
    STRUCT_pos _pos,
    int _value,
    int _minValue,
    int _maxValue,
    int _step,
    float _startAngle,
    float _endAngle,
    unsigned int _majorDivisions,
    unsigned int _minorDivisions,
    std::string _unit,
    int _zOrder)
    : item_gauge(
        _pos,
        _value,
        _minValue,
        _maxValue,
        _startAngle,
        _endAngle,
        _majorDivisions,
        _minorDivisions,
        _unit,
        _zOrder)
{
    step = (_step > 0) ? _step : 1;
}

void item_dial::setValue(int _value)
{
    int previous = getValue();

    item_gauge::setValue(_value);

    if(previous != getValue() &&
       onChanged)
    {
        onChanged(getValue());
    }
}

void item_dial::setValueFromPointer()
{
    int cx = gaugeCenterX();
    int cy = gaugeCenterY();

    float angle =
        atan2f(
            static_cast<float>(
                getPointerY() - cy
            ),
            static_cast<float>(
                getPointerX() - cx
            )
        ) *
        180.0f /
        3.14159265358979323846f;

    while(angle < startAngle)
        angle += 360.0f;

    while(
        angle > endAngle &&
        angle - 360.0f >= startAngle)
    {
        angle -= 360.0f;
    }

    if(angle < startAngle)
        angle = startAngle;

    if(angle > endAngle)
        angle = endAngle;

    float ratio =
        (angle - startAngle) /
        (endAngle - startAngle);

    int rawValue =
        minValue +
        static_cast<int>(
            ratio *
            static_cast<float>(
                maxValue - minValue
            ) +
            0.5f
        );

    int offset =
        rawValue - minValue;

    int snapped =
        minValue +
        ((offset + step / 2) / step) *
        step;

    setValue(snapped);
}

void item_dial::handleEvent(
    int eventType,
    int eventKey)
{
    if(eventType != KEYEV_DOWN &&
       eventType != KEYEV_HOLD)
    {
        return;
    }

    if(eventType == KEYEV_DOWN &&
       eventKey == KEY_EXE &&
       param.status.hover)
    {
        active = true;
        setValueFromPointer();
        return;
    }

    if(!active)
        return;

    if(eventKey == KEY_RIGHT ||
       eventKey == KEY_UP)
    {
        setValue(
            getValue() + step
        );
    }

    if(eventKey == KEY_LEFT ||
       eventKey == KEY_DOWN)
    {
        setValue(
            getValue() - step
        );
    }
}

void item_dial::draw()
{
    item_gauge::draw();

    if(!isVisible())
        return;

    if(active)
    {
        drawOutlineRect(
            getX(),
            getY(),
            getX() + getW() - 1,
            getY() + getH() - 1,
            C_BLUE,
            1
        );
    }
}


//******************************** Tab *********************************

item_tab::item_tab(
    STRUCT_pos _pos,
    std::vector<std::string> _tabs,
    int _selectedTab,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;

    tabs = _tabs;

    if(tabs.empty())
        tabs.push_back("Tab");

    selectedTab = _selectedTab;
    setSelectedTab(selectedTab);

    zOrder = _zOrder;
}

int item_tab::pointerTabIndex() const
{
    if(!param.status.hover || getW() <= 0 || getPointerX() < getX() || getPointerX() >= getX() + getW())
        return -1;
    int count = static_cast<int>(tabs.size());
    if(count <= 0) return -1;
    int index = (getPointerX() - getX()) * count / getW();
    if(index < 0) index = 0;
    if(index >= count) index = count - 1;
    return index;
}

void item_tab::setFocus(bool value)
{
    bool entering = value && !focused;
    focused = value;
    if(!focused || !entering) return;
    int index = pointerTabIndex();
    if(index >= 0)
    {
        bool changed = selectedTab != index;
        selectedTab = index;
        if(changed && onTabChanged) onTabChanged(selectedTab);
    }
}

void item_tab::setSelectedTab(int index)
{
    if(tabs.empty())
    {
        selectedTab = 0;
        return;
    }

    if(index < 0)
        index = 0;

    if(index >= static_cast<int>(tabs.size()))
        index = static_cast<int>(tabs.size()) - 1;

    selectedTab = index;
}

const std::string& item_tab::getTab(unsigned int index) const
{
    static const std::string empty = "";

    if(index >= tabs.size())
        return empty;

    return tabs[index];
}

void item_tab::handleEvent(int eventType, int eventKey)
{
    if(eventType != KEYEV_DOWN || tabs.empty()) return;
    if(focused)
    {
        if(eventKey == KEY_LEFT || eventKey == KEY_RIGHT)
        {
            int next = selectedTab + (eventKey == KEY_LEFT ? -1 : 1);
            if(next < 0) next = static_cast<int>(tabs.size()) - 1;
            if(next >= static_cast<int>(tabs.size())) next = 0;
            if(next != selectedTab)
            {
                selectedTab = next;
                if(onTabChanged) onTabChanged(selectedTab);
            }
            return;
        }
        if(eventKey == KEY_EXE) return;
    }
    if(eventKey != KEY_EXE || !param.status.hover) return;
    int index = pointerTabIndex();
    if(index < 0) return;
    if(index != selectedTab)
    {
        selectedTab = index;
        if(onTabChanged) onTabChanged(selectedTab);
    }
}

void item_tab::draw()
{
    if(!isVisible() || tabs.empty())
        return;

    int count = static_cast<int>(tabs.size());
    int tabWidth = getW() / count;

    for(int i = 0; i < count; ++i)
    {
        int x1 = getX() + i * tabWidth;
        int x2 = (i == count - 1)
            ? getX() + getW() - 1
            : x1 + tabWidth - 1;

        int y1 = getY();
        int y2 = getY() + getH() - 1;

        int fillColor = (i == selectedTab)
            ? C_LIGHT
            : C_WHITE;

        drect(
            x1,
            y1,
            x2,
            y2,
            fillColor
        );

        drawOutlineRect(
            x1,
            y1,
            x2,
            y2,
            (focused && i == selectedTab) ? C_BLUE : C_BLACK,
            (focused && i == selectedTab) ? 2 : 1
        );

        dtext(
            x1 + 4,
            y1 + 6,
            C_BLACK,
            tabs[i].c_str()
        );
    }
}


//******************************** Menu bar *********************************

item_menu_bar::item_menu_bar(
    STRUCT_pos _pos,
    std::vector<std::string> _menus,
    int _selectedMenu,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;

    menus = _menus;

    if(menus.empty())
        menus.push_back("Menu");

    callbacks.resize(menus.size());

    selectedMenu = _selectedMenu;
    setSelectedMenu(selectedMenu);

    zOrder = _zOrder;
}

void item_menu_bar::setSelectedMenu(int index)
{
    if(menus.empty())
    {
        selectedMenu = 0;
        return;
    }

    if(index < 0)
        index = 0;

    if(index >= static_cast<int>(menus.size()))
        index = static_cast<int>(menus.size()) - 1;

    selectedMenu = index;
}

const std::string& item_menu_bar::getMenu(unsigned int index) const
{
    static const std::string empty = "";

    if(index >= menus.size())
        return empty;

    return menus[index];
}

int item_menu_bar::pointerMenuIndex() const
{
    if(!param.status.hover || getW() <= 0 || getPointerX() < getX() || getPointerX() >= getX() + getW())
        return -1;
    int count = static_cast<int>(menus.size());
    if(count <= 0) return -1;
    int index = (getPointerX() - getX()) * count / getW();
    if(index < 0) index = 0;
    if(index >= count) index = count - 1;
    return index;
}

void item_menu_bar::setFocus(bool value)
{
    bool entering = value && !focused;
    focused = value;
    if(!focused || !entering) return;
    int index = pointerMenuIndex();
    if(index >= 0) selectedMenu = index;
}

void item_menu_bar::setCallback(
    unsigned int index,
    std::function<void()> callback)
{
    if(index >= callbacks.size())
        return;

    callbacks[index] = callback;
}

void item_menu_bar::invokeSelected()
{
    if(selectedMenu < 0 ||
       selectedMenu >= static_cast<int>(callbacks.size()))
    {
        return;
    }

    if(callbacks[selectedMenu])
        callbacks[selectedMenu]();
}

void item_menu_bar::handleEvent(int eventType, int eventKey)
{
    if(eventType != KEYEV_DOWN || menus.empty()) return;
    if(focused)
    {
        if(eventKey == KEY_LEFT || eventKey == KEY_RIGHT)
        {
            int next = selectedMenu + (eventKey == KEY_LEFT ? -1 : 1);
            if(next < 0) next = static_cast<int>(menus.size()) - 1;
            if(next >= static_cast<int>(menus.size())) next = 0;
            selectedMenu = next;
            return;
        }
        if(eventKey == KEY_EXE)
        {
            invokeSelected();
            return;
        }
    }
    if(eventKey != KEY_EXE || !param.status.hover) return;
    int index = pointerMenuIndex();
    if(index < 0) return;
    selectedMenu = index;
    invokeSelected();
}

void item_menu_bar::draw()
{
    if(!isVisible() || menus.empty())
        return;

    int count = static_cast<int>(menus.size());
    int menuWidth = getW() / count;

    for(int i = 0; i < count; ++i)
    {
        int x1 = getX() + i * menuWidth;
        int x2 = (i == count - 1)
            ? getX() + getW() - 1
            : x1 + menuWidth - 1;

        int y1 = getY();
        int y2 = getY() + getH() - 1;

        bool pointerInside =
            !focused &&
            param.status.hover &&
            getPointerX() >= x1 &&
            getPointerX() <= x2;

        int fillColor =
            (i == selectedMenu || pointerInside)
            ? C_LIGHT
            : C_WHITE;

        drect(x1, y1, x2, y2, fillColor);

        drawOutlineRect(
            x1,
            y1,
            x2,
            y2,
            (focused && i == selectedMenu) ? C_BLUE : C_BLACK,
            (focused && i == selectedMenu) ? 2 : 1
        );

        dtext(
            x1 + 4,
            y1 + 6,
            C_BLACK,
            menus[i].c_str()
        );
    }
}

//******************************** Bottom soft-key menu *********************************

item_softkey_bar::item_softkey_bar(
    std::vector<std::string> _labels,
    int height,
    int _zOrder)
{
    if(height < 16)
        height = 16;

    param.pos = STRUCT_pos{
        0,
        DHEIGHT - height,
        DWIDTH,
        height
    };

    param.status.visible = true;

    labels = _labels;

    if(labels.size() < 6)
        labels.resize(6);

    if(labels.size() > 6)
        labels.resize(6);

    callbacks.resize(6);
    zOrder = _zOrder;
}

void item_softkey_bar::setLabel(
    unsigned int index,
    const std::string& label)
{
    if(index >= labels.size())
        return;

    labels[index] = label;
}

const std::string& item_softkey_bar::getLabel(unsigned int index) const
{
    static const std::string empty = "";

    if(index >= labels.size())
        return empty;

    return labels[index];
}

void item_softkey_bar::setCallback(
    unsigned int index,
    std::function<void()> callback)
{
    if(index >= callbacks.size())
        return;

    callbacks[index] = callback;
}

bool item_softkey_bar::trigger(unsigned int index)
{
    if(index >= callbacks.size() || !callbacks[index])
        return false;

    callbacks[index]();
    return true;
}

int item_softkey_bar::keyToIndex(int key) const
{
    switch(key)
    {
        case KEY_F1: return 0;
        case KEY_F2: return 1;
        case KEY_F3: return 2;
        case KEY_F4: return 3;
        case KEY_F5: return 4;
        case KEY_F6: return 5;
        default:
            return -1;
    }
}

int item_softkey_bar::pointerToIndex() const
{
    int localX = getPointerX() - getX();

    if(localX < 0 || localX >= getW())
        return -1;

    int index = localX * 6 / getW();

    if(index < 0 || index >= 6)
        return -1;

    return index;
}

void item_softkey_bar::handleGlobalEvent(
    int eventType,
    int eventKey)
{
    if(eventType != KEYEV_DOWN)
        return;

    int index = keyToIndex(eventKey);

    if(index >= 0)
        trigger(static_cast<unsigned int>(index));
}

void item_softkey_bar::handleEvent(
    int eventType,
    int eventKey)
{
    if(
        eventType != KEYEV_DOWN ||
        eventKey != KEY_EXE ||
        !param.status.hover)
    {
        return;
    }

    int index = pointerToIndex();

    if(index >= 0)
        trigger(static_cast<unsigned int>(index));
}

void item_softkey_bar::draw()
{
    if(!isVisible())
        return;

    int cellWidth = getW() / 6;

    for(int i = 0; i < 6; ++i)
    {
        int x1 = getX() + i * cellWidth;
        int x2 = (i == 5)
            ? getX() + getW() - 1
            : x1 + cellWidth - 1;

        int y1 = getY();
        int y2 = getY() + getH() - 1;

        bool pointerInside =
            param.status.hover &&
            getPointerX() >= x1 &&
            getPointerX() <= x2;

        drect(
            x1,
            y1,
            x2,
            y2,
            pointerInside ? C_LIGHT : C_WHITE
        );

        drawOutlineRect(
            x1,
            y1,
            x2,
            y2,
            C_BLACK,
            1
        );

        dprint(
            x1 + 2,
            y1 + 2,
            C_DARK,
            "F%d",
            i + 1
        );

        if(i < static_cast<int>(labels.size()))
        {
            dtext(
                x1 + 2,
                y1 + getH() / 2,
                C_BLACK,
                labels[i].c_str()
            );
        }
    }
}


//******************************** Graph *********************************

static size_t graphTextLength(const char* text)
{
    if(text == nullptr)
        return 0;

    size_t len = 0;

    while(text[len] != '\0')
        ++len;

    return len;
}

static void graphAppendText(
    char* output,
    size_t outputSize,
    const char* text)
{
    if(output == nullptr || outputSize == 0 || text == nullptr)
        return;

    size_t len = graphTextLength(output);
    size_t i = 0;

    while(text[i] != '\0' && len + 1 < outputSize)
        output[len++] = text[i++];

    output[len] = '\0';
}

static void graphAppendChar(
    char* output,
    size_t outputSize,
    char c)
{
    if(output == nullptr || outputSize == 0)
        return;

    size_t len = graphTextLength(output);

    if(len + 1 >= outputSize)
        return;

    output[len] = c;
    output[len + 1] = '\0';
}

static void graphAppendUnsigned(
    char* output,
    size_t outputSize,
    unsigned long value,
    int minDigits = 1)
{
    char digits[24];
    int count = 0;

    do
    {
        digits[count++] = static_cast<char>('0' + (value % 10));
        value /= 10;
    }
    while(value > 0 && count < static_cast<int>(sizeof(digits)));

    while(count < minDigits && count < static_cast<int>(sizeof(digits)))
        digits[count++] = '0';

    for(int i = count - 1; i >= 0; --i)
        graphAppendChar(output, outputSize, digits[i]);
}

static void formatGraphNumber(
    double value,
    int precision,
    char* output,
    size_t outputSize)
{
    if(output == nullptr || outputSize == 0)
        return;

    output[0] = '\0';

    if(precision < 0)
        precision = 0;

    if(precision > 4)
        precision = 4;

    double absValue = (value < 0.0) ? -value : value;

    if(absValue > 99999999.0)
    {
        if(value < 0.0)
            graphAppendChar(output, outputSize, '-');

        graphAppendText(output, outputSize, ">1e8");
        return;
    }

    int factor = 1;

    for(int i = 0; i < precision; ++i)
        factor *= 10;

    long scaled = static_cast<long>(
        value * static_cast<double>(factor) +
        (value >= 0.0 ? 0.5 : -0.5)
    );

    bool negative = scaled < 0;
    unsigned long magnitude =
        static_cast<unsigned long>(negative ? -scaled : scaled);

    unsigned long whole =
        magnitude / static_cast<unsigned long>(factor);

    unsigned long fraction =
        magnitude % static_cast<unsigned long>(factor);

    if(negative)
        graphAppendChar(output, outputSize, '-');

    graphAppendUnsigned(
        output,
        outputSize,
        whole
    );

    if(precision > 0)
    {
        graphAppendChar(output, outputSize, '.');

        graphAppendUnsigned(
            output,
            outputSize,
            fraction,
            precision
        );

        // Remove trailing zeroes and a possible trailing decimal point.
        size_t len = graphTextLength(output);

        while(len > 0 && output[len - 1] == '0')
            output[--len] = '\0';

        if(len > 0 && output[len - 1] == '.')
            output[--len] = '\0';
    }
}

item_graph::item_graph(
    STRUCT_pos _pos,
    graph_axis _xAxis,
    graph_axis _yAxis,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;

    xAxis = _xAxis;
    yAxis = _yAxis;

    normalizeAxis(xAxis);
    normalizeAxis(yAxis);

    initialXAxis = xAxis;
    initialYAxis = yAxis;

    zOrder = _zOrder;
}

bool item_graph::normalizeAxis(graph_axis& axis)
{
    if(axis.divisions < 1)
        axis.divisions = 1;

    if(axis.scale == graph_scale::LOG10)
    {
        if(axis.minValue <= 0.0)
            axis.minValue = 0.1;

        if(axis.maxValue <= axis.minValue)
            axis.maxValue = axis.minValue * 10.0;
    }
    else
    {
        if(axis.maxValue <= axis.minValue)
            axis.maxValue = axis.minValue + 1.0;
    }

    return true;
}

double item_graph::transformValue(
    double value,
    graph_scale scale,
    bool& valid)
{
    valid = true;

    if(scale == graph_scale::LOG10)
    {
        if(value <= 0.0)
        {
            valid = false;
            return 0.0;
        }

        return log10(value);
    }

    return value;
}

double item_graph::inverseValue(
    double value,
    graph_scale scale)
{
    if(scale == graph_scale::LOG10)
        return pow(10.0, value);

    return value;
}

int item_graph::plotLeft() const
{
    return getX() + leftMargin;
}

int item_graph::plotRight() const
{
    return getX() + getW() - rightMargin - 1;
}

int item_graph::plotTop() const
{
    return getY() + topMargin;
}

int item_graph::plotBottom() const
{
    return getY() + getH() - bottomMargin - 1;
}

bool item_graph::mapX(double value, int& screenX) const
{
    bool validValue = false;
    bool validMin = false;
    bool validMax = false;

    double v = transformValue(value, xAxis.scale, validValue);
    double minValue = transformValue(xAxis.minValue, xAxis.scale, validMin);
    double maxValue = transformValue(xAxis.maxValue, xAxis.scale, validMax);

    if(!validValue || !validMin || !validMax || maxValue <= minValue)
        return false;

    double ratio = (v - minValue) / (maxValue - minValue);

    screenX = plotLeft() +
        static_cast<int>(
            ratio * static_cast<double>(plotRight() - plotLeft())
        );

    return true;
}

bool item_graph::mapY(double value, int& screenY) const
{
    bool validValue = false;
    bool validMin = false;
    bool validMax = false;

    double v = transformValue(value, yAxis.scale, validValue);
    double minValue = transformValue(yAxis.minValue, yAxis.scale, validMin);
    double maxValue = transformValue(yAxis.maxValue, yAxis.scale, validMax);

    if(!validValue || !validMin || !validMax || maxValue <= minValue)
        return false;

    double ratio = (v - minValue) / (maxValue - minValue);

    screenY = plotBottom() -
        static_cast<int>(
            ratio * static_cast<double>(plotBottom() - plotTop())
        );

    return true;
}

int item_graph::addSeries(
    Class_color color,
    bool connected,
    bool showPoints)
{
    series.emplace_back(color, connected, showPoints, true);
    return static_cast<int>(series.size()) - 1;
}

bool item_graph::addPoint(
    unsigned int seriesIndex,
    double x,
    double y)
{
    if(seriesIndex >= series.size())
        return false;

    series[seriesIndex].points.emplace_back(x, y);
    return true;
}

bool item_graph::clearSeries(unsigned int seriesIndex)
{
    if(seriesIndex >= series.size())
        return false;

    series[seriesIndex].points.clear();
    return true;
}

void item_graph::clearAllSeries()
{
    series.clear();
}

graph_series* item_graph::getSeries(unsigned int index)
{
    if(index >= series.size())
        return nullptr;

    return &series[index];
}

const graph_series* item_graph::getSeries(unsigned int index) const
{
    if(index >= series.size())
        return nullptr;

    return &series[index];
}

void item_graph::setXAxis(const graph_axis& axis)
{
    xAxis = axis;
    normalizeAxis(xAxis);
}

void item_graph::setYAxis(const graph_axis& axis)
{
    yAxis = axis;
    normalizeAxis(yAxis);
}

void item_graph::setXRange(
    double minValue,
    double maxValue)
{
    xAxis.minValue = minValue;
    xAxis.maxValue = maxValue;
    normalizeAxis(xAxis);
}

void item_graph::setYRange(
    double minValue,
    double maxValue)
{
    yAxis.minValue = minValue;
    yAxis.maxValue = maxValue;
    normalizeAxis(yAxis);
}

void item_graph::setLogX(bool enabled)
{
    xAxis.scale =
        enabled
        ? graph_scale::LOG10
        : graph_scale::LINEAR;

    normalizeAxis(xAxis);
}

void item_graph::setLogY(bool enabled)
{
    yAxis.scale =
        enabled
        ? graph_scale::LOG10
        : graph_scale::LINEAR;

    normalizeAxis(yAxis);
}

void item_graph::panAxis(
    graph_axis& axis,
    int direction)
{
    bool validMin = false;
    bool validMax = false;

    double minValue =
        transformValue(axis.minValue, axis.scale, validMin);

    double maxValue =
        transformValue(axis.maxValue, axis.scale, validMax);

    if(!validMin || !validMax || maxValue <= minValue)
        return;

    double shift =
        (maxValue - minValue) *
        panStep *
        static_cast<double>(direction);

    minValue += shift;
    maxValue += shift;

    axis.minValue = inverseValue(minValue, axis.scale);
    axis.maxValue = inverseValue(maxValue, axis.scale);
}

void item_graph::zoomAxis(
    graph_axis& axis,
    double factor)
{
    if(factor <= 0.0)
        return;

    bool validMin = false;
    bool validMax = false;

    double minValue =
        transformValue(axis.minValue, axis.scale, validMin);

    double maxValue =
        transformValue(axis.maxValue, axis.scale, validMax);

    if(!validMin || !validMax || maxValue <= minValue)
        return;

    double center = (minValue + maxValue) * 0.5;
    double halfSpan = (maxValue - minValue) * 0.5 * factor;

    if(halfSpan <= 0.000001)
        return;

    axis.minValue =
        inverseValue(center - halfSpan, axis.scale);

    axis.maxValue =
        inverseValue(center + halfSpan, axis.scale);
}

void item_graph::panX(int direction)
{
    panAxis(xAxis, direction);
}

void item_graph::panY(int direction)
{
    panAxis(yAxis, direction);
}

void item_graph::zoomIn()
{
    zoomAxis(xAxis, zoomFactor);
    zoomAxis(yAxis, zoomFactor);
}

void item_graph::zoomOut()
{
    zoomAxis(xAxis, 1.0 / zoomFactor);
    zoomAxis(yAxis, 1.0 / zoomFactor);
}

void item_graph::resetView()
{
    xAxis = initialXAxis;
    yAxis = initialYAxis;
}

double item_graph::axisGraduationValue(
    const graph_axis& axis,
    int division) const
{
    if(axis.divisions <= 0)
        return axis.minValue;

    bool validMin = false;
    bool validMax = false;

    double minValue = transformValue(axis.minValue, axis.scale, validMin);
    double maxValue = transformValue(axis.maxValue, axis.scale, validMax);

    if(!validMin || !validMax || maxValue <= minValue)
        return axis.minValue;

    double ratio =
        static_cast<double>(division) /
        static_cast<double>(axis.divisions);

    return inverseValue(
        minValue + (maxValue - minValue) * ratio,
        axis.scale
    );
}

int item_graph::addCursor(
    double x,
    double y,
    Class_color color,
    std::string label,
    graph_cursor_style style)
{
    cursors.emplace_back(
        x,
        y,
        color,
        true,
        true,
        label,
        style
    );

    if(activeCursor < 0)
        activeCursor = 0;

    return static_cast<int>(cursors.size()) - 1;
}

bool item_graph::removeCursor(unsigned int index)
{
    if(index >= cursors.size())
        return false;

    cursors.erase(cursors.begin() + index);

    if(cursors.empty())
    {
        activeCursor = -1;
        cursorControl = false;
    }
    else if(activeCursor >= static_cast<int>(cursors.size()))
    {
        activeCursor = static_cast<int>(cursors.size()) - 1;
    }

    return true;
}

void item_graph::clearCursors()
{
    cursors.clear();
    activeCursor = -1;
    cursorControl = false;
}

graph_cursor* item_graph::getCursor(unsigned int index)
{
    if(index >= cursors.size())
        return nullptr;

    return &cursors[index];
}

const graph_cursor* item_graph::getCursor(unsigned int index) const
{
    if(index >= cursors.size())
        return nullptr;

    return &cursors[index];
}

bool item_graph::setActiveCursor(unsigned int index)
{
    if(index >= cursors.size())
        return false;

    activeCursor = static_cast<int>(index);
    return true;
}

void item_graph::moveCursorAxis(
    double& value,
    const graph_axis& axis,
    int direction)
{
    bool validMin = false;
    bool validMax = false;
    bool validValue = false;

    double minValue = transformValue(axis.minValue, axis.scale, validMin);
    double maxValue = transformValue(axis.maxValue, axis.scale, validMax);
    double current = transformValue(value, axis.scale, validValue);

    if(!validMin || !validMax || !validValue || maxValue <= minValue)
        return;

    current +=
        (maxValue - minValue) *
        cursorStep *
        static_cast<double>(direction);

    if(current < minValue)
        current = minValue;

    if(current > maxValue)
        current = maxValue;

    value = inverseValue(current, axis.scale);
}

void item_graph::moveActiveCursor(int dx, int dy)
{
    if(activeCursor < 0 || activeCursor >= static_cast<int>(cursors.size()))
        return;

    graph_cursor& cursor = cursors[activeCursor];

    if(dx != 0)
        moveCursorAxis(cursor.x, xAxis, dx);

    if(dy != 0)
        moveCursorAxis(cursor.y, yAxis, dy);

    if(onCursorMoved)
        onCursorMoved(activeCursor, cursor.x, cursor.y);
}

void item_graph::handleEvent(
    int eventType,
    int eventKey)
{
    if(!focused)
        return;

    if(eventType != KEYEV_DOWN && eventType != KEYEV_HOLD)
        return;

    if(eventType == KEYEV_DOWN && eventKey == KEY_OPTN && !cursors.empty())
    {
        cursorControl = !cursorControl;
        return;
    }

    if(cursorControl && activeCursor >= 0)
    {
        switch(eventKey)
        {
            case KEY_LEFT:
                moveActiveCursor(-1, 0);
                return;

            case KEY_RIGHT:
                moveActiveCursor(1, 0);
                return;

            case KEY_UP:
                moveActiveCursor(0, 1);
                return;

            case KEY_DOWN:
                moveActiveCursor(0, -1);
                return;

            case KEY_FD:
                if(eventType == KEYEV_DOWN && !cursors.empty())
                {
                    activeCursor =
                        (activeCursor + 1) %
                        static_cast<int>(cursors.size());
                }
                return;

            default:
                break;
        }
    }

    switch(eventKey)
    {
        case KEY_LEFT:
            panX(-1);
            break;

        case KEY_RIGHT:
            panX(1);
            break;

        case KEY_UP:
            panY(1);
            break;

        case KEY_DOWN:
            panY(-1);
            break;

        case KEY_ADD:
            zoomIn();
            break;

        case KEY_SUB:
            zoomOut();
            break;

        case KEY_0:
            resetView();
            break;

        default:
            break;
    }
}

void item_graph::draw()
{
    if(!isVisible())
        return;

    int x1 = getX();
    int y1 = getY();
    int x2 = getX() + getW() - 1;
    int y2 = getY() + getH() - 1;

    int pLeft = plotLeft();
    int pRight = plotRight();
    int pTop = plotTop();
    int pBottom = plotBottom();

    if(pRight <= pLeft || pBottom <= pTop)
        return;

    drect(x1, y1, x2, y2, C_WHITE);

    drawOutlineRect(
        x1,
        y1,
        x2,
        y2,
        focused ? C_BLUE : C_BLACK,
        focused ? 2 : 1
    );

    drawOutlineRect(
        pLeft,
        pTop,
        pRight,
        pBottom,
        C_BLACK,
        1
    );

    if(xAxis.showGrid)
    {
        for(int i = 1; i < xAxis.divisions; ++i)
        {
            int x =
                pLeft +
                (pRight - pLeft) * i /
                xAxis.divisions;

            dline(x, pTop, x, pBottom, C_LIGHT);
        }
    }

    if(yAxis.showGrid)
    {
        for(int i = 1; i < yAxis.divisions; ++i)
        {
            int y =
                pBottom -
                (pBottom - pTop) * i /
                yAxis.divisions;

            dline(pLeft, y, pRight, y, C_LIGHT);
        }
    }

    if(xAxis.showAxis)
    {
        int axisY = pBottom;

        if(
            yAxis.scale == graph_scale::LINEAR &&
            yAxis.minValue <= 0.0 &&
            yAxis.maxValue >= 0.0)
        {
            mapY(0.0, axisY);
        }

        dline(pLeft, axisY, pRight, axisY, C_BLACK);

        for(int i = 0; i <= xAxis.divisions; ++i)
        {
            int x =
                pLeft +
                (pRight - pLeft) * i /
                xAxis.divisions;

            dline(x, axisY - 2, x, axisY + 2, C_BLACK);
        }
    }

    if(yAxis.showAxis)
    {
        int axisX = pLeft;

        if(
            xAxis.scale == graph_scale::LINEAR &&
            xAxis.minValue <= 0.0 &&
            xAxis.maxValue >= 0.0)
        {
            mapX(0.0, axisX);
        }

        dline(axisX, pTop, axisX, pBottom, C_BLACK);

        for(int i = 0; i <= yAxis.divisions; ++i)
        {
            int y =
                pBottom -
                (pBottom - pTop) * i /
                yAxis.divisions;

            dline(axisX - 2, y, axisX + 2, y, C_BLACK);
        }
    }

    auto drawGraduationValue = [](
        int x,
        int y,
        double value,
        int precision)
    {
        char text[28];
        formatGraphNumber(value, precision, text, sizeof(text));
        dtext(x, y, C_DARK, text);
    };

    if(xAxis.showGraduations)
    {
        for(int i = 0; i <= xAxis.divisions; ++i)
        {
            int x =
                pLeft +
                (pRight - pLeft) * i /
                xAxis.divisions;

            double value = axisGraduationValue(xAxis, i);

            drawGraduationValue(
                x - 9,
                pBottom + 5,
                value,
                xAxis.graduationPrecision
            );
        }
    }

    if(yAxis.showGraduations)
    {
        for(int i = 0; i <= yAxis.divisions; ++i)
        {
            int y =
                pBottom -
                (pBottom - pTop) * i /
                yAxis.divisions;

            double value = axisGraduationValue(yAxis, i);

            drawGraduationValue(
                getX() + 2,
                y - 5,
                value,
                yAxis.graduationPrecision
            );
        }
    }

    if(!xAxis.label.empty())
    {
        dtext(
            pRight - 12,
            pBottom + 5,
            C_BLACK,
            xAxis.label.c_str()
        );
    }

    if(!yAxis.label.empty())
    {
        dtext(
            getX() + 3,
            pTop,
            C_BLACK,
            yAxis.label.c_str()
        );
    }

    if(xAxis.scale == graph_scale::LOG10)
    {
        dtext(
            pLeft + 3,
            pBottom + 5,
            C_DARK,
            "log X"
        );
    }

    if(yAxis.scale == graph_scale::LOG10)
    {
        dtext(
            getX() + 3,
            pTop + 12,
            C_DARK,
            "log Y"
        );
    }

    struct dwindow graphWindow = {
        pLeft,
        pTop,
        pRight + 1,
        pBottom + 1
    };

    struct dwindow oldWindow =
        dwindow_set(graphWindow);

    for(auto& currentSeries : series)
    {
        if(!currentSeries.visible)
            continue;

        bool previousValid = false;
        int previousX = 0;
        int previousY = 0;

        for(auto& point : currentSeries.points)
        {
            int pointX = 0;
            int pointY = 0;

            bool valid =
                mapX(point.x, pointX) &&
                mapY(point.y, pointY);

            if(valid)
            {
                if(currentSeries.connected && previousValid)
                {
                    dline(
                        previousX,
                        previousY,
                        pointX,
                        pointY,
                        currentSeries.color.getRGB()
                    );
                }

                if(currentSeries.showPoints)
                {
                    drect(
                        pointX - 1,
                        pointY - 1,
                        pointX + 1,
                        pointY + 1,
                        currentSeries.color.getRGB()
                    );
                }

                previousX = pointX;
                previousY = pointY;
                previousValid = true;
            }
            else
            {
                previousValid = false;
            }
        }
    }

    for(unsigned int i = 0; i < cursors.size(); ++i)
    {
        graph_cursor& cursor = cursors[i];

        if(!cursor.visible)
            continue;

        int cursorX = 0;
        int cursorY = 0;

        if(!mapX(cursor.x, cursorX) || !mapY(cursor.y, cursorY))
            continue;

        int color = cursor.color.getRGB();

        if(cursor.style == graph_cursor_style::CROSSHAIR ||
           cursor.style == graph_cursor_style::VERTICAL)
        {
            dline(cursorX, pTop, cursorX, pBottom, color);
        }

        if(cursor.style == graph_cursor_style::CROSSHAIR ||
           cursor.style == graph_cursor_style::HORIZONTAL)
        {
            dline(pLeft, cursorY, pRight, cursorY, color);
        }

        drect(
            cursorX - 2,
            cursorY - 2,
            cursorX + 2,
            cursorY + 2,
            color
        );
    }

    dwindow_set(oldWindow);

    int infoY = pTop + 3;

    for(unsigned int i = 0; i < cursors.size(); ++i)
    {
        const graph_cursor& cursor = cursors[i];

        if(!cursor.visible || !cursor.showValues)
            continue;

        char xText[24];
        char yText[24];
        char text[72];

        formatGraphNumber(cursor.x, 3, xText, sizeof(xText));
        formatGraphNumber(cursor.y, 3, yText, sizeof(yText));

        text[0] = '\0';

        if(static_cast<int>(i) == activeCursor && cursorControl)
            graphAppendText(text, sizeof(text), ">");

        graphAppendText(text, sizeof(text), cursor.label.c_str());
        graphAppendText(text, sizeof(text), " X=");
        graphAppendText(text, sizeof(text), xText);
        graphAppendText(text, sizeof(text), " Y=");
        graphAppendText(text, sizeof(text), yText);

        dtext(pLeft + 3, infoY, cursor.color.getRGB(), text);
        infoY += 12;
    }

    if(showCursorDelta && cursors.size() >= 2)
    {
        const graph_cursor& a = cursors[0];
        const graph_cursor& b = cursors[1];

        if(a.visible && b.visible)
        {
            char dxText[24];
            char dyText[24];
            char deltaText[64];

            formatGraphNumber(b.x - a.x, 3, dxText, sizeof(dxText));
            formatGraphNumber(b.y - a.y, 3, dyText, sizeof(dyText));

            deltaText[0] = '\0';
            graphAppendText(deltaText, sizeof(deltaText), "dX=");
            graphAppendText(deltaText, sizeof(deltaText), dxText);
            graphAppendText(deltaText, sizeof(deltaText), " dY=");
            graphAppendText(deltaText, sizeof(deltaText), dyText);

            dtext(pLeft + 3, infoY, C_DARK, deltaText);
        }
    }
}


//******************************** Table / spreadsheet *********************************






//******************************** Canvas *********************************

item_canvas::item_canvas(
    STRUCT_pos _pos,
    bool _drawBackground,
    bool _drawBorder,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;

    backgroundEnabled = _drawBackground;
    borderEnabled = _drawBorder;

    zOrder = _zOrder;

    applyTheme(
        current_item_theme()
    );
}

void item_canvas::applyTheme(
    const item_theme& theme)
{
    item::applyTheme(theme);

    backgroundColor =
        theme.surface;

    canvasBorderColor =
        theme.border;
}

int item_canvas::resolvedColor(
    const Class_color& value) const
{
    if(isDimmed())
        return current_item_theme().disabled.getRGB();

    return value.getRGB();
}

void item_canvas::sortCommands()
{
    std::stable_sort(
        commands.begin(),
        commands.end(),
        [](const canvas_command& a,
           const canvas_command& b)
        {
            if(a.zOrder != b.zOrder)
                return a.zOrder < b.zOrder;

            return a.id < b.id;
        }
    );
}

void item_canvas::ensurePolygonScratch(
    unsigned int count)
{
    if(polygonScratch.capacity() < count)
        polygonScratch.reserve(count);
}

int item_canvas::appendCommand(
    canvas_command command)
{
    command.id =
        nextCommandId++;

    if(command.thickness < 1)
        command.thickness = 1;

    if(command.borderSize < 1)
        command.borderSize = 1;

    ensurePolygonScratch(
        static_cast<unsigned int>(
            command.points.size()
        )
    );

    commands.push_back(command);
    sortCommands();

    return command.id;
}

int item_canvas::addPixel(
    int x,
    int y,
    Class_color color,
    int _zOrder)
{
    canvas_command command;
    command.type = canvas_command_type::PIXEL;
    command.p1 = STRUCT_point{x, y};
    command.color = color;
    command.zOrder = _zOrder;

    return appendCommand(command);
}

int item_canvas::addLine(
    STRUCT_point p1,
    STRUCT_point p2,
    Class_color color,
    int thickness,
    int _zOrder)
{
    canvas_command command;
    command.type = canvas_command_type::LINE;
    command.p1 = p1;
    command.p2 = p2;
    command.color = color;
    command.thickness = thickness;
    command.zOrder = _zOrder;

    return appendCommand(command);
}

int item_canvas::addRectangle(
    STRUCT_pos rect,
    Class_color fillColor,
    Class_color borderColor,
    item_draw_mode mode,
    int borderSize,
    int _zOrder)
{
    canvas_command command;
    command.type = canvas_command_type::RECTANGLE;
    command.rect = rect;
    command.fillColor = fillColor;
    command.borderColor = borderColor;
    command.drawMode = mode;
    command.borderSize = borderSize;
    command.zOrder = _zOrder;

    return appendCommand(command);
}

int item_canvas::addCircle(
    STRUCT_point center,
    int radius,
    Class_color fillColor,
    Class_color borderColor,
    item_draw_mode mode,
    int borderSize,
    int _zOrder)
{
    canvas_command command;
    command.type = canvas_command_type::CIRCLE;
    command.p1 = center;
    command.radius = (radius > 0) ? radius : 1;
    command.fillColor = fillColor;
    command.borderColor = borderColor;
    command.drawMode = mode;
    command.borderSize = borderSize;
    command.zOrder = _zOrder;

    return appendCommand(command);
}

int item_canvas::addEllipse(
    STRUCT_point center,
    int radiusX,
    int radiusY,
    Class_color fillColor,
    Class_color borderColor,
    item_draw_mode mode,
    int borderSize,
    int _zOrder)
{
    canvas_command command;
    command.type = canvas_command_type::ELLIPSE;
    command.p1 = center;
    command.radiusX = (radiusX > 0) ? radiusX : 1;
    command.radiusY = (radiusY > 0) ? radiusY : 1;
    command.fillColor = fillColor;
    command.borderColor = borderColor;
    command.drawMode = mode;
    command.borderSize = borderSize;
    command.zOrder = _zOrder;

    return appendCommand(command);
}

int item_canvas::addPolyline(
    const std::vector<STRUCT_point>& points,
    Class_color color,
    int thickness,
    int _zOrder)
{
    canvas_command command;
    command.type = canvas_command_type::POLYLINE;
    command.points = points;
    command.color = color;
    command.thickness = thickness;
    command.zOrder = _zOrder;

    return appendCommand(command);
}

int item_canvas::addPolygon(
    const std::vector<STRUCT_point>& points,
    Class_color fillColor,
    Class_color borderColor,
    item_draw_mode mode,
    int borderSize,
    int _zOrder)
{
    canvas_command command;
    command.type = canvas_command_type::POLYGON;
    command.points = points;
    command.fillColor = fillColor;
    command.borderColor = borderColor;
    command.drawMode = mode;
    command.borderSize = borderSize;
    command.zOrder = _zOrder;

    return appendCommand(command);
}

int item_canvas::addArc(
    STRUCT_point center,
    int radius,
    float startAngleDegrees,
    float endAngleDegrees,
    Class_color color,
    int thickness,
    unsigned int segments,
    int _zOrder)
{
    if(radius < 1)
        radius = 1;

    if(segments < 2)
        segments = 2;

    std::vector<STRUCT_point> points;
    points.reserve(segments + 1);

    const float pi =
        3.14159265358979323846f;

    for(unsigned int i = 0;
        i <= segments;
        ++i)
    {
        float t =
            static_cast<float>(i) /
            static_cast<float>(segments);

        float angle =
            startAngleDegrees +
            (
                endAngleDegrees -
                startAngleDegrees
            ) *
            t;

        float radians =
            angle *
            pi /
            180.0f;

        points.push_back(
            STRUCT_point{
                center.x +
                    static_cast<int>(
                        cosf(radians) *
                        radius
                    ),
                center.y +
                    static_cast<int>(
                        sinf(radians) *
                        radius
                    )
            }
        );
    }

    return addPolyline(
        points,
        color,
        thickness,
        _zOrder
    );
}

int item_canvas::addSector(
    STRUCT_point center,
    int radius,
    float startAngleDegrees,
    float endAngleDegrees,
    Class_color fillColor,
    Class_color borderColor,
    int borderSize,
    unsigned int segments,
    int _zOrder)
{
    if(radius < 1)
        radius = 1;

    if(segments < 2)
        segments = 2;

    std::vector<STRUCT_point> points;
    points.reserve(segments + 2);

    points.push_back(center);

    const float pi =
        3.14159265358979323846f;

    for(unsigned int i = 0;
        i <= segments;
        ++i)
    {
        float t =
            static_cast<float>(i) /
            static_cast<float>(segments);

        float angle =
            startAngleDegrees +
            (
                endAngleDegrees -
                startAngleDegrees
            ) *
            t;

        float radians =
            angle *
            pi /
            180.0f;

        points.push_back(
            STRUCT_point{
                center.x +
                    static_cast<int>(
                        cosf(radians) *
                        radius
                    ),
                center.y +
                    static_cast<int>(
                        sinf(radians) *
                        radius
                    )
            }
        );
    }

    return addPolygon(
        points,
        fillColor,
        borderColor,
        item_draw_mode::FILLED,
        borderSize,
        _zOrder
    );
}

int item_canvas::addText(
    STRUCT_point position,
    const std::string& text,
    Class_color color,
    int _zOrder)
{
    canvas_command command;
    command.type = canvas_command_type::TEXT;
    command.p1 = position;
    command.text = text;
    command.color = color;
    command.zOrder = _zOrder;

    return appendCommand(command);
}

bool item_canvas::removeCommand(
    int commandId)
{
    for(auto it = commands.begin();
        it != commands.end();
        ++it)
    {
        if(it->id != commandId)
            continue;

        commands.erase(it);
        return true;
    }

    return false;
}

void item_canvas::clearCommands()
{
    commands.clear();
    polygonScratch.clear();
    nextCommandId = 1;
}

canvas_command* item_canvas::getCommand(
    int commandId)
{
    for(canvas_command& command : commands)
    {
        if(command.id == commandId)
            return &command;
    }

    return nullptr;
}

const canvas_command* item_canvas::getCommand(
    int commandId) const
{
    for(const canvas_command& command : commands)
    {
        if(command.id == commandId)
            return &command;
    }

    return nullptr;
}

bool item_canvas::setCommandVisible(
    int commandId,
    bool visible)
{
    canvas_command* command =
        getCommand(commandId);

    if(command == nullptr)
        return false;

    command->visible = visible;
    return true;
}

bool item_canvas::setCommandZOrder(
    int commandId,
    int _zOrder)
{
    canvas_command* command =
        getCommand(commandId);

    if(command == nullptr)
        return false;

    command->zOrder = _zOrder;
    sortCommands();

    return true;
}

void item_canvas::drawPolygonCommand(
    canvas_command& command,
    bool closePath)
{
    if(command.points.size() < 2)
        return;

    if(
        command.drawMode == item_draw_mode::FILLED &&
        closePath &&
        command.points.size() >= 3)
    {
        ensurePolygonScratch(
            static_cast<unsigned int>(
                command.points.size()
            )
        );

        int minY =
            command.points[0].y;

        int maxY =
            command.points[0].y;

        for(const STRUCT_point& point :
            command.points)
        {
            if(point.y < minY)
                minY = point.y;

            if(point.y > maxY)
                maxY = point.y;
        }

        int fill =
            resolvedColor(
                command.fillColor
            );

        for(int localY = minY;
            localY <= maxY;
            ++localY)
        {
            polygonScratch.clear();

            for(unsigned int i = 0;
                i < command.points.size();
                ++i)
            {
                const STRUCT_point& a =
                    command.points[i];

                const STRUCT_point& b =
                    command.points[
                        (i + 1) %
                        command.points.size()
                    ];

                if(a.y == b.y)
                    continue;

                int lowY =
                    (a.y < b.y)
                        ? a.y
                        : b.y;

                int highY =
                    (a.y > b.y)
                        ? a.y
                        : b.y;

                if(
                    localY < lowY ||
                    localY >= highY)
                {
                    continue;
                }

                double ratio =
                    static_cast<double>(
                        localY - a.y
                    ) /
                    static_cast<double>(
                        b.y - a.y
                    );

                int localX =
                    static_cast<int>(
                        static_cast<double>(a.x) +
                        ratio *
                        static_cast<double>(
                            b.x - a.x
                        )
                    );

                polygonScratch.push_back(
                    localX
                );
            }

            std::sort(
                polygonScratch.begin(),
                polygonScratch.end()
            );

            for(unsigned int i = 0;
                i + 1 < polygonScratch.size();
                i += 2)
            {
                dline(
                    getX() +
                        polygonScratch[i],
                    getY() +
                        localY,
                    getX() +
                        polygonScratch[i + 1],
                    getY() +
                        localY,
                    fill
                );
            }
        }
    }

    int lineColor =
        closePath
            ? resolvedColor(
                command.borderColor
            )
            : resolvedColor(
                command.color
            );

    int thickness =
        closePath
            ? command.borderSize
            : command.thickness;

    unsigned int segmentCount =
        closePath
            ? static_cast<unsigned int>(
                command.points.size()
            )
            : static_cast<unsigned int>(
                command.points.size() - 1
            );

    for(unsigned int i = 0;
        i < segmentCount;
        ++i)
    {
        const STRUCT_point& a =
            command.points[i];

        const STRUCT_point& b =
            closePath
                ? command.points[
                    (i + 1) %
                    command.points.size()
                ]
                : command.points[i + 1];

        drawThickLine(
            getX() + a.x,
            getY() + a.y,
            getX() + b.x,
            getY() + b.y,
            lineColor,
            thickness
        );
    }
}

void item_canvas::drawCommand(
    canvas_command& command)
{
    if(!command.visible)
        return;

    STRUCT_point p1 =
        localToScreen(command.p1);

    STRUCT_point p2 =
        localToScreen(command.p2);

    switch(command.type)
    {
        case canvas_command_type::PIXEL:
            dpixel(
                p1.x,
                p1.y,
                resolvedColor(
                    command.color
                )
            );
            break;

        case canvas_command_type::LINE:
            drawThickLine(
                p1.x,
                p1.y,
                p2.x,
                p2.y,
                resolvedColor(
                    command.color
                ),
                command.thickness
            );
            break;

        case canvas_command_type::RECTANGLE:
        {
            int x1 =
                getX() +
                command.rect.x;

            int y1 =
                getY() +
                command.rect.y;

            int x2 =
                x1 +
                command.rect.w -
                1;

            int y2 =
                y1 +
                command.rect.h -
                1;

            if(command.drawMode == item_draw_mode::FILLED)
            {
                drect(
                    x1,
                    y1,
                    x2,
                    y2,
                    resolvedColor(
                        command.fillColor
                    )
                );
            }

            drawOutlineRect(
                x1,
                y1,
                x2,
                y2,
                resolvedColor(
                    command.borderColor
                ),
                command.borderSize
            );

            break;
        }

        case canvas_command_type::CIRCLE:
        {
            int fill =
                resolvedColor(
                    command.fillColor
                );

            int border =
                resolvedColor(
                    command.borderColor
                );

            if(command.drawMode == item_draw_mode::FILLED)
            {
                drawCircleFilled(
                    p1.x,
                    p1.y,
                    command.radius,
                    fill
                );
            }

            for(int b = 0;
                b < command.borderSize;
                ++b)
            {
                int radius =
                    command.radius -
                    b;

                if(radius < 0)
                    break;

                drawCircleOutline(
                    p1.x,
                    p1.y,
                    radius,
                    border
                );
            }

            break;
        }

        case canvas_command_type::ELLIPSE:
        {
            int fill =
                resolvedColor(
                    command.fillColor
                );

            int border =
                resolvedColor(
                    command.borderColor
                );

            for(int dy =
                    -command.radiusY;
                dy <= command.radiusY;
                ++dy)
            {
                double normalized =
                    static_cast<double>(dy) /
                    static_cast<double>(
                        command.radiusY
                    );

                double remaining =
                    1.0 -
                    normalized *
                    normalized;

                if(remaining < 0.0)
                    remaining = 0.0;

                int extent =
                    static_cast<int>(
                        static_cast<double>(
                            command.radiusX
                        ) *
                        sqrt(remaining)
                    );

                if(
                    command.drawMode ==
                    item_draw_mode::FILLED)
                {
                    dline(
                        p1.x - extent,
                        p1.y + dy,
                        p1.x + extent,
                        p1.y + dy,
                        fill
                    );
                }

                for(int b = 0;
                    b < command.borderSize;
                    ++b)
                {
                    int bx =
                        extent - b;

                    if(bx < 0)
                        break;

                    dpixel(
                        p1.x - bx,
                        p1.y + dy,
                        border
                    );

                    dpixel(
                        p1.x + bx,
                        p1.y + dy,
                        border
                    );
                }
            }

            break;
        }

        case canvas_command_type::POLYLINE:
            drawPolygonCommand(
                command,
                false
            );
            break;

        case canvas_command_type::POLYGON:
            drawPolygonCommand(
                command,
                true
            );
            break;

        case canvas_command_type::TEXT:
            dtext(
                p1.x,
                p1.y,
                resolvedColor(
                    command.color
                ),
                command.text.c_str()
            );
            break;
    }
}

void item_canvas::handleEvent(
    int eventType,
    int eventKey)
{
    if(
        eventType == KEYEV_DOWN &&
        eventKey == KEY_EXE &&
        param.status.hover &&
        onCanvasClick)
    {
        onCanvasClick(
            getPointerX() -
                getX(),
            getPointerY() -
                getY()
        );
    }
}

void item_canvas::draw()
{
    if(!isVisible())
        return;

    int x1 = getX();
    int y1 = getY();
    int x2 =
        getX() +
        getW() -
        1;

    int y2 =
        getY() +
        getH() -
        1;

    if(backgroundEnabled)
    {
        drect(
            x1,
            y1,
            x2,
            y2,
            isDimmed()
                ? current_item_theme().surfaceAlt.getRGB()
                : backgroundColor.getRGB()
        );
    }

    struct dwindow window = {
        x1,
        y1,
        x2 + 1,
        y2 + 1
    };

    struct dwindow oldWindow =
        dwindow_set(window);

    for(canvas_command& command : commands)
        drawCommand(command);

    dwindow_set(oldWindow);

    if(borderEnabled)
    {
        drawOutlineRect(
            x1,
            y1,
            x2,
            y2,
            isDimmed()
                ? current_item_theme().disabled.getRGB()
                : canvasBorderColor.getRGB(),
            canvasBorderSize
        );
    }
}


//******************************** Lightweight utility controls *********************************


item_text_label::item_text_label(
    STRUCT_pos _pos,
    std::string _text,
    item_text_align _align,
    bool _opaque,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;

    text = _text;
    alignment = _align;
    opaque = _opaque;
    zOrder = _zOrder;

    applyTheme(
        current_item_theme()
    );
}

void item_text_label::applyTheme(
    const item_theme& theme)
{
    item::applyTheme(theme);
    textColor = theme.text;
    backgroundColor = theme.surface;
}

void item_text_label::draw()
{
    if(!isVisible())
        return;

    if(opaque)
    {
        drect(
            getX(),
            getY(),
            getX() + getW() - 1,
            getY() + getH() - 1,
            backgroundColor.getRGB()
        );
    }

    int textW = 0;
    int textH = 0;

    dsize(
        text.c_str(),
        nullptr,
        &textW,
        &textH
    );

    int x = getX();

    if(alignment == item_text_align::CENTER)
        x = getX() + (getW() - textW) / 2;
    else if(alignment == item_text_align::RIGHT)
        x = getX() + getW() - textW;

    int y =
        getY() +
        (getH() - textH) / 2;

    dtext(
        x,
        y,
        textColor.getRGB(),
        text.c_str()
    );
}


item_separator::item_separator(
    STRUCT_pos _pos,
    item_orientation _orientation,
    int _thickness,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;

    orientation = _orientation;
    thickness =
        (_thickness > 0)
            ? _thickness
            : 1;

    zOrder = _zOrder;

    applyTheme(
        current_item_theme()
    );
}

void item_separator::applyTheme(
    const item_theme& theme)
{
    item::applyTheme(theme);
    color = theme.border;
}

void item_separator::draw()
{
    if(!isVisible())
        return;

    if(orientation == item_orientation::HORIZONTAL)
    {
        int y =
            getY() +
            getH() / 2;

        for(int i = 0; i < thickness; ++i)
        {
            dline(
                getX(),
                y + i,
                getX() + getW() - 1,
                y + i,
                color.getRGB()
            );
        }
    }
    else
    {
        int x =
            getX() +
            getW() / 2;

        for(int i = 0; i < thickness; ++i)
        {
            dline(
                x + i,
                getY(),
                x + i,
                getY() + getH() - 1,
                color.getRGB()
            );
        }
    }
}


item_toolbar::item_toolbar(
    STRUCT_pos _pos,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;
    zOrder = _zOrder;

    applyTheme(
        current_item_theme()
    );
}

void item_toolbar::applyTheme(
    const item_theme& theme)
{
    item::applyTheme(theme);

    backgroundColor = theme.surfaceAlt;
    borderColor = theme.border;
    textColor = theme.text;
    disabledColor = theme.disabled;
    hoverColor = theme.selection;
}

int item_toolbar::actionIndexById(int id) const
{
    for(unsigned int i = 0;
        i < actions.size();
        ++i)
    {
        if(actions[i].id == id)
            return static_cast<int>(i);
    }

    return -1;
}

int item_toolbar::addAction(
    const std::string& label,
    std::function<void()> callback,
    int shortcutKey,
    bool enabled)
{
    int id = nextActionId++;

    actions.emplace_back(
        id,
        label,
        shortcutKey,
        enabled,
        callback
    );

    return id;
}

bool item_toolbar::removeAction(int id)
{
    int index =
        actionIndexById(id);

    if(index < 0)
        return false;

    actions.erase(
        actions.begin() + index
    );

    return true;
}

void item_toolbar::clearActions()
{
    actions.clear();
    nextActionId = 1;
}

bool item_toolbar::setActionEnabled(
    int id,
    bool enabled)
{
    int index =
        actionIndexById(id);

    if(index < 0)
        return false;

    actions[index].enabled =
        enabled;

    return true;
}

bool item_toolbar::setActionLabel(
    int id,
    const std::string& label)
{
    int index =
        actionIndexById(id);

    if(index < 0)
        return false;

    actions[index].label =
        label;

    return true;
}

bool item_toolbar::setActionShortcut(
    int id,
    int shortcutKey)
{
    int index =
        actionIndexById(id);

    if(index < 0)
        return false;

    actions[index].shortcutKey =
        shortcutKey;

    return true;
}

bool item_toolbar::setActionCallback(
    int id,
    std::function<void()> callback)
{
    int index =
        actionIndexById(id);

    if(index < 0)
        return false;

    actions[index].callback =
        callback;

    return true;
}

void item_toolbar::triggerAction(int index)
{
    if(
        index < 0 ||
        index >= static_cast<int>(
            actions.size()
        ) ||
        !actions[index].enabled)
    {
        return;
    }

    if(actions[index].callback)
        actions[index].callback();
}

int item_toolbar::pointerActionIndex() const
{
    if(
        actions.empty() ||
        !contains(
            getPointerX(),
            getPointerY()
        ))
    {
        return -1;
    }

    int width =
        getW() /
        static_cast<int>(
            actions.size()
        );

    if(width < 1)
        width = 1;

    int index =
        (
            getPointerX() -
            getX()
        ) /
        width;

    if(index >= static_cast<int>(actions.size()))
        index = static_cast<int>(actions.size()) - 1;

    return index;
}

void item_toolbar::handleEvent(
    int eventType,
    int eventKey)
{
    if(
        eventType == KEYEV_DOWN &&
        eventKey == KEY_EXE &&
        param.status.hover)
    {
        triggerAction(
            pointerActionIndex()
        );
    }
}

void item_toolbar::handleGlobalEvent(
    int eventType,
    int eventKey)
{
    if(eventType != KEYEV_DOWN)
        return;

    for(unsigned int i = 0;
        i < actions.size();
        ++i)
    {
        if(
            actions[i].enabled &&
            actions[i].shortcutKey != 0 &&
            actions[i].shortcutKey == eventKey)
        {
            triggerAction(
                static_cast<int>(i)
            );
            return;
        }
    }
}

void item_toolbar::draw()
{
    if(!isVisible())
        return;

    drect(
        getX(),
        getY(),
        getX() + getW() - 1,
        getY() + getH() - 1,
        backgroundColor.getRGB()
    );

    drawOutlineRect(
        getX(),
        getY(),
        getX() + getW() - 1,
        getY() + getH() - 1,
        borderColor.getRGB(),
        1
    );

    if(actions.empty())
        return;

    int width =
        getW() /
        static_cast<int>(
            actions.size()
        );

    if(width < 1)
        width = 1;

    int hovered =
        param.status.hover
            ? pointerActionIndex()
            : -1;

    for(unsigned int i = 0;
        i < actions.size();
        ++i)
    {
        int x1 =
            getX() +
            static_cast<int>(i) *
            width;

        int x2 =
            (i == actions.size() - 1)
                ? getX() +
                  getW() -
                  1
                : x1 +
                  width -
                  1;

        if(static_cast<int>(i) == hovered)
        {
            drect(
                x1 + 1,
                getY() + 1,
                x2 - 1,
                getY() + getH() - 2,
                hoverColor.getRGB()
            );
        }

        if(i > 0)
        {
            dline(
                x1,
                getY() + 2,
                x1,
                getY() + getH() - 3,
                borderColor.getRGB()
            );
        }

        int tw = 0;
        int th = 0;

        dsize(
            actions[i].label.c_str(),
            nullptr,
            &tw,
            &th
        );

        dtext(
            x1 +
                (x2 - x1 + 1 - tw) /
                2,
            getY() +
                (getH() - th) /
                2,
            actions[i].enabled
                ? textColor.getRGB()
                : disabledColor.getRGB(),
            actions[i].label.c_str()
        );
    }
}


item_status_bar::item_status_bar(
    STRUCT_pos _pos,
    std::string _leftText,
    std::string _centerText,
    std::string _rightText,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;

    leftText = _leftText;
    centerText = _centerText;
    rightText = _rightText;

    zOrder = _zOrder;

    applyTheme(
        current_item_theme()
    );
}

void item_status_bar::applyTheme(
    const item_theme& theme)
{
    item::applyTheme(theme);

    backgroundColor = theme.surfaceAlt;
    borderColor = theme.border;
    textColor = theme.text;
}

void item_status_bar::draw()
{
    if(!isVisible())
        return;

    const item_theme& theme =
        current_item_theme();

    drect(
        getX(),
        getY(),
        getX() + getW() - 1,
        getY() + getH() - 1,
        backgroundColor.getRGB()
    );

    dline(
        getX(),
        getY(),
        getX() + getW() - 1,
        getY(),
        borderColor.getRGB()
    );

    int indicatorSpace =
        indicatorVisible
            ? 14
            : 0;

    if(indicatorVisible)
    {
        Class_color statusColor =
            theme.disabled;

        switch(status)
        {
            case item_led_state::ON:
                statusColor =
                    theme.success;
                break;

            case item_led_state::WARNING:
                statusColor =
                    theme.warning;
                break;

            case item_led_state::ERROR:
                statusColor =
                    theme.error;
                break;

            case item_led_state::OFF:
            default:
                break;
        }

        drawCircleFilled(
            getX() + 7,
            getY() + getH() / 2,
            4,
            statusColor.getRGB()
        );
    }

    dtext(
        getX() + 4 + indicatorSpace,
        getY() + (getH() - 12) / 2,
        textColor.getRGB(),
        leftText.c_str()
    );

    int centerW = 0;
    int centerH = 0;

    dsize(
        centerText.c_str(),
        nullptr,
        &centerW,
        &centerH
    );

    dtext(
        getX() +
            (getW() - centerW) /
            2,
        getY() +
            (getH() - centerH) /
            2,
        textColor.getRGB(),
        centerText.c_str()
    );

    int rightW = 0;
    int rightH = 0;

    dsize(
        rightText.c_str(),
        nullptr,
        &rightW,
        &rightH
    );

    dtext(
        getX() +
            getW() -
            rightW -
            4,
        getY() +
            (getH() - rightH) /
            2,
        textColor.getRGB(),
        rightText.c_str()
    );
}


//******************************** Containers / layouts *********************************


item_container::item_container(
    STRUCT_pos _pos,
    item_layout_mode _layout,
    int _padding,
    int _gap,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;

    layoutMode = _layout;
    padding = (_padding >= 0) ? _padding : 0;
    gap = (_gap >= 0) ? _gap : 0;

    zOrder = _zOrder;
}

void item_container::applyTheme(
    const item_theme& theme)
{
    item::applyTheme(theme);

    backgroundColor =
        theme.surface;

    borderColor =
        theme.border;

    titleColor =
        theme.text;

    for(container_child& child : children)
    {
        if(child.control != nullptr)
            child.control->applyTheme(theme);
    }
}

bool item_container::handleSystemExit()
{
    item* child =
        getFocusedChild();

    if(child != nullptr &&
       child->handleSystemExit())
    {
        if(!child->hasFocus())
        {
            focusedChildId = -1;
            focused = false;
        }

        return true;
    }

    return item::handleSystemExit();
}

bool item_container::isFocusable() const
{
    for(const container_child& child : children)
    {
        if(
            child.control != nullptr &&
            childIsActive(child) &&
            child.control->isVisible() &&
            child.control->isFocusable())
        {
            return true;
        }
    }

    return false;
}

bool item_container::wantsPointerFocus(
    int x,
    int y) const
{
    const item* child =
        getTopChildAt(
            x,
            y
        );

    if(child == nullptr)
        return false;

    return child->wantsPointerFocus(
        x,
        y
    );
}

void item_container::setFocus(bool value)
{
    focused = value;

    if(!focused)
        clearChildFocus();
}

void item_container::setLayoutMode(
    item_layout_mode value)
{
    layoutMode = value;
    performLayout();
}

void item_container::setPadding(int value)
{
    padding = (value >= 0) ? value : 0;
    performLayout();
}

void item_container::setGap(int value)
{
    gap = (value >= 0) ? value : 0;
    performLayout();
}

void item_container::setGridColumns(int value)
{
    gridColumns = (value > 0) ? value : 1;
    performLayout();
}

void item_container::setContainerTitle(
    const std::string& value)
{
    title = value;
    performLayout();
}

void item_container::setContainerColors(
    Class_color background,
    Class_color border,
    Class_color _titleColor)
{
    backgroundColor = background;
    borderColor = border;
    titleColor = _titleColor;
}

STRUCT_pos item_container::getContentRect() const
{
    int titleSpace =
        title.empty()
            ? 0
            : 18;

    int x =
        getX() +
        padding;

    int y =
        getY() +
        padding +
        titleSpace;

    int w =
        getW() -
        padding * 2;

    int h =
        getH() -
        padding * 2 -
        titleSpace;

    if(w < 1)
        w = 1;

    if(h < 1)
        h = 1;

    return STRUCT_pos{x, y, w, h};
}

void item_container::drawChrome()
{
    drect(
        getX(),
        getY(),
        getX() + getW() - 1,
        getY() + getH() - 1,
        backgroundColor.getRGB()
    );

    drawOutlineRect(
        getX(),
        getY(),
        getX() + getW() - 1,
        getY() + getH() - 1,
        focused
            ? current_item_theme().accent.getRGB()
            : borderColor.getRGB(),
        focused
            ? current_item_theme().focusBorderSize
            : current_item_theme().borderSize
    );

    if(!title.empty())
    {
        dtext(
            getX() + padding,
            getY() + 3,
            titleColor.getRGB(),
            title.c_str()
        );
    }
}

bool item_container::childIsActive(
    const container_child& child) const
{
    return
        child.control != nullptr &&
        child.visible;
}

container_child* item_container::getChildLink(int childId)
{
    for(container_child& child : children)
    {
        if(child.id == childId)
            return &child;
    }

    return nullptr;
}

const container_child* item_container::getChildLink(
    int childId) const
{
    for(const container_child& child : children)
    {
        if(child.id == childId)
            return &child;
    }

    return nullptr;
}

int item_container::addChild(
    item* control,
    STRUCT_pos localGeometry,
    item_margins margins,
    int anchors)
{
    if(control == nullptr)
        return -1;

    control->applyTheme(
        current_item_theme()
    );

    int id = nextChildId++;

    STRUCT_pos content =
        getContentRect();

    container_child child;
    child.id = id;
    child.control = control;
    child.local = localGeometry;
    child.margins = margins;
    child.anchors = anchors;
    child.visible = true;
    child.group = -1;
    child.referenceContentW = content.w;
    child.referenceContentH = content.h;

    // If the caller left the default preferred size but the control already
    // has a meaningful one, preserve it.
    if(localGeometry.w <= 0)
        child.local.w = control->getW();

    if(localGeometry.h <= 0)
        child.local.h = control->getH();

    children.push_back(child);

    if(layoutScratch.capacity() < children.size())
        layoutScratch.reserve(children.size());

    if(drawScratch.capacity() < children.size())
        drawScratch.reserve(children.size());

    performLayout();

    return id;
}

bool item_container::removeChild(int childId)
{
    for(auto it = children.begin();
        it != children.end();
        ++it)
    {
        if(it->id != childId)
            continue;

        if(focusedChildId == childId)
            clearChildFocus();

        children.erase(it);
        performLayout();
        return true;
    }

    return false;
}

void item_container::clearChildren()
{
    clearChildFocus();
    children.clear();
    nextChildId = 1;
}

item* item_container::getChild(int childId)
{
    container_child* child =
        getChildLink(childId);

    return
        (child == nullptr)
            ? nullptr
            : child->control;
}

const item* item_container::getChild(
    int childId) const
{
    const container_child* child =
        getChildLink(childId);

    return
        (child == nullptr)
            ? nullptr
            : child->control;
}

bool item_container::setChildGeometry(
    int childId,
    STRUCT_pos localGeometry)
{
    container_child* child =
        getChildLink(childId);

    if(child == nullptr)
        return false;

    child->local = localGeometry;

    STRUCT_pos content =
        getContentRect();

    child->referenceContentW = content.w;
    child->referenceContentH = content.h;

    performLayout();
    return true;
}

bool item_container::setChildMargins(
    int childId,
    item_margins margins)
{
    container_child* child =
        getChildLink(childId);

    if(child == nullptr)
        return false;

    child->margins = margins;
    performLayout();
    return true;
}

bool item_container::setChildAnchors(
    int childId,
    int anchors)
{
    container_child* child =
        getChildLink(childId);

    if(child == nullptr)
        return false;

    child->anchors = anchors;

    STRUCT_pos content =
        getContentRect();

    child->referenceContentW = content.w;
    child->referenceContentH = content.h;

    return true;
}

bool item_container::setChildVisible(
    int childId,
    bool visible)
{
    container_child* child =
        getChildLink(childId);

    if(child == nullptr)
        return false;

    child->visible = visible;

    if(!visible && focusedChildId == childId)
        clearChildFocus();

    return true;
}

bool item_container::setChildGroup(
    int childId,
    int group)
{
    container_child* child =
        getChildLink(childId);

    if(child == nullptr)
        return false;

    child->group = group;
    return true;
}

void item_container::setScrollOffsetInternal(
    int x,
    int y)
{
    scrollX = (x >= 0) ? x : 0;
    scrollY = (y >= 0) ? y : 0;
    performLayout();
}

void item_container::performLayout()
{
    STRUCT_pos content =
        getContentRect();

    layoutScratch.clear();

    if(layoutScratch.capacity() < children.size())
        layoutScratch.reserve(children.size());

    for(container_child& child : children)
    {
        if(
            child.control != nullptr &&
            childIsActive(child))
        {
            layoutScratch.push_back(&child);
        }
    }

    if(layoutMode == item_layout_mode::ABSOLUTE)
    {
        for(container_child* child : layoutScratch)
        {
            int x = child->local.x;
            int y = child->local.y;
            int w = child->local.w;
            int h = child->local.h;

            int deltaW =
                content.w -
                child->referenceContentW;

            int deltaH =
                content.h -
                child->referenceContentH;

            bool left =
                (child->anchors & ITEM_ANCHOR_LEFT) != 0;

            bool right =
                (child->anchors & ITEM_ANCHOR_RIGHT) != 0;

            bool top =
                (child->anchors & ITEM_ANCHOR_TOP) != 0;

            bool bottom =
                (child->anchors & ITEM_ANCHOR_BOTTOM) != 0;

            if(left && right)
                w += deltaW;
            else if(!left && right)
                x += deltaW;

            if(top && bottom)
                h += deltaH;
            else if(!top && bottom)
                y += deltaH;

            if(w < 1)
                w = 1;

            if(h < 1)
                h = 1;

            child->control->setGeometry(
                STRUCT_pos{
                    content.x +
                        x +
                        child->margins.left -
                        scrollX,
                    content.y +
                        y +
                        child->margins.top -
                        scrollY,
                    w -
                        child->margins.left -
                        child->margins.right,
                    h -
                        child->margins.top -
                        child->margins.bottom
                }
            );

            if(child->control->getW() < 1)
                child->control->setDimensions(
                    1,
                    child->control->getH()
                );

            if(child->control->getH() < 1)
                child->control->setDimensions(
                    child->control->getW(),
                    1
                );
        }

        return;
    }

    if(layoutScratch.empty())
        return;

    if(layoutMode == item_layout_mode::VERTICAL)
    {
        int y =
            content.y -
            scrollY;

        for(container_child* child : layoutScratch)
        {
            y += child->margins.top;

            int h =
                child->local.h > 0
                    ? child->local.h
                    : child->control->getH();

            int w =
                content.w -
                child->margins.left -
                child->margins.right;

            if(w < 1)
                w = 1;

            if(h < 1)
                h = 1;

            child->control->setGeometry(
                STRUCT_pos{
                    content.x +
                        child->margins.left -
                        scrollX,
                    y,
                    w,
                    h
                }
            );

            y +=
                h +
                child->margins.bottom +
                gap;
        }

        return;
    }

    if(layoutMode == item_layout_mode::HORIZONTAL)
    {
        int x =
            content.x -
            scrollX;

        for(container_child* child : layoutScratch)
        {
            x += child->margins.left;

            int w =
                child->local.w > 0
                    ? child->local.w
                    : child->control->getW();

            int h =
                content.h -
                child->margins.top -
                child->margins.bottom;

            if(w < 1)
                w = 1;

            if(h < 1)
                h = 1;

            child->control->setGeometry(
                STRUCT_pos{
                    x,
                    content.y +
                        child->margins.top -
                        scrollY,
                    w,
                    h
                }
            );

            x +=
                w +
                child->margins.right +
                gap;
        }

        return;
    }

    int columns =
        (gridColumns > 0)
            ? gridColumns
            : 1;

    int rows =
        (
            static_cast<int>(layoutScratch.size()) +
            columns -
            1
        ) /
        columns;

    if(rows < 1)
        rows = 1;

    int cellW =
        (
            content.w -
            gap * (columns - 1)
        ) /
        columns;

    int cellH =
        (
            content.h -
            gap * (rows - 1)
        ) /
        rows;

    if(cellW < 1)
        cellW = 1;

    if(cellH < 1)
        cellH = 1;

    for(unsigned int i = 0;
        i < layoutScratch.size();
        ++i)
    {
        int row =
            static_cast<int>(i) /
            columns;

        int column =
            static_cast<int>(i) %
            columns;

        container_child* child =
            layoutScratch[i];

        int x =
            content.x +
            column * (cellW + gap) +
            child->margins.left -
            scrollX;

        int y =
            content.y +
            row * (cellH + gap) +
            child->margins.top -
            scrollY;

        int w =
            cellW -
            child->margins.left -
            child->margins.right;

        int h =
            cellH -
            child->margins.top -
            child->margins.bottom;

        if(w < 1)
            w = 1;

        if(h < 1)
            h = 1;

        child->control->setGeometry(
            STRUCT_pos{x, y, w, h}
        );
    }
}

void item_container::clearChildHover()
{
    for(container_child& child : children)
    {
        if(child.control != nullptr)
            child.control->clearPointerState();
    }
}

item* item_container::getTopChildAt(
    int x,
    int y)
{
    item* top = nullptr;
    int topZ = INT_MIN;
    int topId = INT_MIN;

    STRUCT_pos content =
        getContentRect();

    if(
        x < content.x ||
        x >= content.x + content.w ||
        y < content.y ||
        y >= content.y + content.h)
    {
        return nullptr;
    }

    for(container_child& child : children)
    {
        if(
            child.control == nullptr ||
            !childIsActive(child) ||
            !child.control->isVisible() ||
            !child.control->contains(x, y))
        {
            continue;
        }

        int z = child.control->getZOrder();

        if(
            top == nullptr ||
            z > topZ ||
            (z == topZ && child.id > topId))
        {
            top = child.control;
            topZ = z;
            topId = child.id;
        }
    }

    return top;
}

const item* item_container::getTopChildAt(
    int x,
    int y) const
{
    const item* top = nullptr;
    int topZ = INT_MIN;
    int topId = INT_MIN;

    STRUCT_pos content =
        getContentRect();

    if(
        x < content.x ||
        x >= content.x + content.w ||
        y < content.y ||
        y >= content.y + content.h)
    {
        return nullptr;
    }

    for(const container_child& child : children)
    {
        if(
            child.control == nullptr ||
            !childIsActive(child) ||
            !child.control->isVisible() ||
            !child.control->contains(x, y))
        {
            continue;
        }

        int z = child.control->getZOrder();

        if(
            top == nullptr ||
            z > topZ ||
            (z == topZ && child.id > topId))
        {
            top = child.control;
            topZ = z;
            topId = child.id;
        }
    }

    return top;
}

item* item_container::getFocusedChild()
{
    if(focusedChildId < 0)
        return nullptr;

    container_child* child =
        getChildLink(focusedChildId);

    if(
        child == nullptr ||
        child->control == nullptr ||
        !childIsActive(*child) ||
        !child->control->isVisible() ||
        !child->control->hasFocus())
    {
        focusedChildId = -1;
        return nullptr;
    }

    return child->control;
}

bool item_container::setChildFocus(int childId)
{
    container_child* target =
        getChildLink(childId);

    if(
        target == nullptr ||
        target->control == nullptr ||
        !childIsActive(*target) ||
        !target->control->isVisible() ||
        !target->control->isFocusable())
    {
        return false;
    }

    clearChildFocus();

    target->control->setFocus(true);
    focusedChildId = childId;
    focused = true;

    return true;
}

void item_container::clearChildFocus()
{
    for(container_child& child : children)
    {
        if(
            child.control != nullptr &&
            child.control->isFocusable())
        {
            child.control->setFocus(false);
        }
    }

    focusedChildId = -1;
}

bool item_container::focusNextChild()
{
    if(children.empty())
        return false;

    int start = -1;

    for(unsigned int i = 0;
        i < children.size();
        ++i)
    {
        if(children[i].id == focusedChildId)
        {
            start = static_cast<int>(i);
            break;
        }
    }

    for(unsigned int step = 1;
        step <= children.size();
        ++step)
    {
        int index =
            (
                start +
                static_cast<int>(step)
            ) %
            static_cast<int>(children.size());

        container_child& child =
            children[index];

        if(
            child.control != nullptr &&
            childIsActive(child) &&
            child.visible &&
            child.control->isVisible() &&
            child.control->isFocusable())
        {
            return setChildFocus(child.id);
        }
    }

    return false;
}

bool item_container::focusPreviousChild()
{
    if(children.empty())
        return false;

    int start = 0;

    for(unsigned int i = 0;
        i < children.size();
        ++i)
    {
        if(children[i].id == focusedChildId)
        {
            start = static_cast<int>(i);
            break;
        }
    }

    for(unsigned int step = 1;
        step <= children.size();
        ++step)
    {
        int index =
            start -
            static_cast<int>(step);

        while(index < 0)
            index += static_cast<int>(children.size());

        container_child& child =
            children[index];

        if(
            child.control != nullptr &&
            childIsActive(child) &&
            child.visible &&
            child.control->isVisible() &&
            child.control->isFocusable())
        {
            return setChildFocus(child.id);
        }
    }

    return false;
}

bool item_container::isHover(
    int x,
    int y)
{
    bool inside =
        item::isHover(x, y);

    clearChildHover();

    if(!inside)
        return false;

    performLayout();

    item* child =
        getTopChildAt(x, y);

    if(child != nullptr)
        child->isHover(x, y);

    return true;
}

void item_container::handleEvent(
    int eventType,
    int eventKey)
{
    performLayout();

    item* hovered =
        getTopChildAt(
            getPointerX(),
            getPointerY()
        );

    if(
        hovered != nullptr &&
        eventType == KEYEV_DOWN &&
        eventKey == KEY_EXE &&
        hovered->wantsPointerFocus(
            getPointerX(),
            getPointerY()
        ))
    {
        for(container_child& child : children)
        {
            if(child.control == hovered)
            {
                setChildFocus(child.id);
                break;
            }
        }
    }

    item* keyboard =
        getFocusedChild();

    if(
        keyboard != nullptr &&
        keyboard != hovered)
    {
        keyboard->handleEvent(
            eventType,
            eventKey
        );
    }

    if(hovered != nullptr)
    {
        hovered->isClick(eventKey);

        hovered->handleEvent(
            eventType,
            eventKey
        );

        hovered->chckEvent(
            eventType,
            eventKey
        );
    }

    if(eventType == KEYEV_UP)
    {
        for(container_child& child : children)
        {
            if(child.control != nullptr)
                child.control->isClick(-1);
        }
    }
}

void item_container::drawChildren()
{
    performLayout();

    STRUCT_pos content =
        getContentRect();

    struct dwindow window = {
        content.x,
        content.y,
        content.x + content.w,
        content.y + content.h
    };

    struct dwindow oldWindow =
        dwindow_set(window);

    drawScratch.clear();

    if(drawScratch.capacity() < children.size())
        drawScratch.reserve(children.size());

    for(container_child& child : children)
    {
        if(
            child.control != nullptr &&
            childIsActive(child) &&
            child.control->isVisible())
        {
            drawScratch.push_back(&child);
        }
    }

    std::sort(
        drawScratch.begin(),
        drawScratch.end(),
        [](const container_child* a,
           const container_child* b)
        {
            if(
                a->control->getZOrder() !=
                b->control->getZOrder())
            {
                return
                    a->control->getZOrder() <
                    b->control->getZOrder();
            }

            return a->id < b->id;
        }
    );

    for(container_child* child : drawScratch)
        child->control->draw();

    dwindow_set(oldWindow);
}

void item_container::updateChildren(
    uint32_t nowTicks)
{
    performLayout();

    for(container_child& child : children)
    {
        if(
            child.control != nullptr &&
            childIsActive(child) &&
            child.control->isVisible())
        {
            child.control->update(nowTicks);
        }
    }
}

void item_container::update(
    uint32_t nowTicks)
{
    updateChildren(nowTicks);
}

void item_container::draw()
{
    if(!isVisible())
        return;

    drawChrome();
    drawChildren();
}


item_scroll_view::item_scroll_view(
    STRUCT_pos _pos,
    item_layout_mode _layout,
    int _padding,
    int _gap,
    bool _showScrollbars,
    int _zOrder)
    : item_container(
        _pos,
        _layout,
        _padding,
        _gap,
        _zOrder)
{
    showScrollbars = _showScrollbars;
}

int item_scroll_view::getContentWidth()
{
    performLayout();

    STRUCT_pos viewport =
        getContentRect();

    int maximum =
        viewport.w;

    for(const container_child& child : children)
    {
        if(
            child.control == nullptr ||
            !childIsActive(child))
        {
            continue;
        }

        int right =
            child.control->getX() -
            viewport.x +
            scrollX +
            child.control->getW() +
            child.margins.right;

        if(right > maximum)
            maximum = right;
    }

    return maximum;
}

int item_scroll_view::getContentHeight()
{
    performLayout();

    STRUCT_pos viewport =
        getContentRect();

    int maximum =
        viewport.h;

    for(const container_child& child : children)
    {
        if(
            child.control == nullptr ||
            !childIsActive(child))
        {
            continue;
        }

        int bottom =
            child.control->getY() -
            viewport.y +
            scrollY +
            child.control->getH() +
            child.margins.bottom;

        if(bottom > maximum)
            maximum = bottom;
    }

    return maximum;
}

int item_scroll_view::maxScrollX()
{
    int maximum =
        getContentWidth() -
        getContentRect().w;

    return
        (maximum > 0)
            ? maximum
            : 0;
}

int item_scroll_view::maxScrollY()
{
    int maximum =
        getContentHeight() -
        getContentRect().h;

    return
        (maximum > 0)
            ? maximum
            : 0;
}

void item_scroll_view::clampScroll()
{
    int x = scrollX;
    int y = scrollY;

    int maxX =
        maxScrollX();

    int maxY =
        maxScrollY();

    if(x < 0)
        x = 0;

    if(y < 0)
        y = 0;

    if(x > maxX)
        x = maxX;

    if(y > maxY)
        y = maxY;

    scrollX = x;
    scrollY = y;

    performLayout();
}

void item_scroll_view::setScroll(
    int x,
    int y)
{
    scrollX = x;
    scrollY = y;
    clampScroll();
}

void item_scroll_view::scrollBy(
    int dx,
    int dy)
{
    setScroll(
        scrollX + dx,
        scrollY + dy
    );
}

bool item_scroll_view::scrollToChild(
    int childId)
{
    container_child* child =
        getChildLink(childId);

    if(
        child == nullptr ||
        child->control == nullptr)
    {
        return false;
    }

    performLayout();

    STRUCT_pos viewport =
        getContentRect();

    int x = scrollX;
    int y = scrollY;

    int left =
        child->control->getX();

    int right =
        left +
        child->control->getW();

    int top =
        child->control->getY();

    int bottom =
        top +
        child->control->getH();

    if(left < viewport.x)
        x -= viewport.x - left;
    else if(right > viewport.x + viewport.w)
        x += right - (viewport.x + viewport.w);

    if(top < viewport.y)
        y -= viewport.y - top;
    else if(bottom > viewport.y + viewport.h)
        y += bottom - (viewport.y + viewport.h);

    setScroll(x, y);
    return true;
}

bool item_scroll_view::wantsPointerFocus(
    int x,
    int y) const
{
    if(!contains(x, y))
        return false;

    item_scroll_view* self =
        const_cast<item_scroll_view*>(this);

    self->isHover(x, y);

    if(
        pointerOnVerticalScrollbar() ||
        pointerOnHorizontalScrollbar())
    {
        return true;
    }

    return item_container::wantsPointerFocus(
        x,
        y
    );
}

bool item_scroll_view::pointerOnVerticalScrollbar() const
{
    if(!showScrollbars) return false;
    item_scroll_view* self = const_cast<item_scroll_view*>(this);
    STRUCT_pos viewport = getContentRect();
    if(self->getContentHeight() <= viewport.h) return false;
    return getPointerX() >= viewport.x + viewport.w - 5 && getPointerX() < viewport.x + viewport.w &&
           getPointerY() >= viewport.y && getPointerY() < viewport.y + viewport.h;
}

bool item_scroll_view::pointerOnHorizontalScrollbar() const
{
    if(!showScrollbars) return false;
    item_scroll_view* self = const_cast<item_scroll_view*>(this);
    STRUCT_pos viewport = getContentRect();
    if(self->getContentWidth() <= viewport.w) return false;
    return getPointerY() >= viewport.y + viewport.h - 5 && getPointerY() < viewport.y + viewport.h &&
           getPointerX() >= viewport.x && getPointerX() < viewport.x + viewport.w;
}

bool item_scroll_view::isHover(int x, int y)
{
    bool inside = item_container::isHover(x, y);
    if(inside && (pointerOnVerticalScrollbar() || pointerOnHorizontalScrollbar()))
        clearChildHover();
    return inside;
}

void item_scroll_view::handleEvent(int eventType, int eventKey)
{
    bool verticalBar = pointerOnVerticalScrollbar();
    bool horizontalBar = pointerOnHorizontalScrollbar();

    if(eventType == KEYEV_DOWN && eventKey == KEY_EXE && (verticalBar || horizontalBar))
    {
        clearChildFocus();
        clearChildHover();
        focused = true;
        STRUCT_pos viewport = getContentRect();
        if(verticalBar)
        {
            int denominator = viewport.h - 1;
            if(denominator < 1) denominator = 1;
            setScroll(scrollX, maxScrollY() * (getPointerY() - viewport.y) / denominator);
        }
        if(horizontalBar)
        {
            int denominator = viewport.w - 1;
            if(denominator < 1) denominator = 1;
            setScroll(maxScrollX() * (getPointerX() - viewport.x) / denominator, scrollY);
        }
        return;
    }

    item_container::handleEvent(eventType, eventKey);

    if(!focused || getFocusedChild() != nullptr || (eventType != KEYEV_DOWN && eventType != KEYEV_HOLD))
        return;

    if(eventKey == KEY_UP) scrollBy(0, -scrollStep);
    if(eventKey == KEY_DOWN) scrollBy(0, scrollStep);
    if(eventKey == KEY_LEFT) scrollBy(-scrollStep, 0);
    if(eventKey == KEY_RIGHT) scrollBy(scrollStep, 0);
}

void item_scroll_view::drawScrollbars()
{
    if(!showScrollbars)
        return;

    STRUCT_pos viewport =
        getContentRect();

    int contentW =
        getContentWidth();

    int contentH =
        getContentHeight();

    if(contentH > viewport.h)
    {
        int x1 =
            viewport.x +
            viewport.w -
            5;

        int y1 = viewport.y;
        int y2 =
            viewport.y +
            viewport.h -
            1;

        drect(
            x1,
            y1,
            x1 + 4,
            y2,
            C_LIGHT
        );

        int thumbH =
            viewport.h *
            viewport.h /
            contentH;

        if(thumbH < 8)
            thumbH = 8;

        int travel =
            viewport.h -
            thumbH;

        int maxY =
            contentH -
            viewport.h;

        int thumbY =
            y1 +
            (
                maxY > 0
                    ? travel *
                      scrollY /
                      maxY
                    : 0
            );

        drect(
            x1 + 1,
            thumbY,
            x1 + 3,
            thumbY + thumbH - 1,
            C_DARK
        );
    }

    if(contentW > viewport.w)
    {
        int x1 = viewport.x;
        int x2 =
            viewport.x +
            viewport.w -
            1;

        int y1 =
            viewport.y +
            viewport.h -
            5;

        drect(
            x1,
            y1,
            x2,
            y1 + 4,
            C_LIGHT
        );

        int thumbW =
            viewport.w *
            viewport.w /
            contentW;

        if(thumbW < 8)
            thumbW = 8;

        int travel =
            viewport.w -
            thumbW;

        int maxX =
            contentW -
            viewport.w;

        int thumbX =
            x1 +
            (
                maxX > 0
                    ? travel *
                      scrollX /
                      maxX
                    : 0
            );

        drect(
            thumbX,
            y1 + 1,
            thumbX + thumbW - 1,
            y1 + 3,
            C_DARK
        );
    }
}

void item_scroll_view::draw()
{
    if(!isVisible())
        return;

    clampScroll();
    item_container::draw();
    drawScrollbars();
}


item_tab_view::item_tab_view(
    STRUCT_pos _pos,
    std::vector<std::string> _tabs,
    int _selectedTab,
    int _headerHeight,
    int _padding,
    int _zOrder)
    : item_container(
        _pos,
        item_layout_mode::ABSOLUTE,
        _padding,
        4,
        _zOrder)
{
    tabs = _tabs;

    if(tabs.empty())
        tabs.push_back("Tab");

    headerHeight =
        (_headerHeight >= 18)
            ? _headerHeight
            : 18;

    selectedTab = 0;
    setSelectedTab(
        _selectedTab,
        false
    );
}

void item_tab_view::setFocus(bool value)
{
    bool entering = value && !focused;
    item_container::setFocus(value);
    if(!value)
    {
        headerFocused = false;
        return;
    }
    if(!entering) return;
    int tab = pointerTabIndex();
    if(tab >= 0)
    {
        headerFocused = true;
        clearChildFocus();
        setSelectedTab(tab);
    }
    else headerFocused = false;
}

STRUCT_pos item_tab_view::getContentRect() const
{
    int x =
        getX() +
        padding;

    int y =
        getY() +
        headerHeight +
        padding;

    int w =
        getW() -
        padding * 2;

    int h =
        getH() -
        headerHeight -
        padding * 2;

    if(w < 1)
        w = 1;

    if(h < 1)
        h = 1;

    return STRUCT_pos{x, y, w, h};
}

bool item_tab_view::childIsActive(
    const container_child& child) const
{
    return
        child.control != nullptr &&
        child.visible &&
        (
            child.group < 0 ||
            child.group == selectedTab
        );
}

void item_tab_view::drawChrome()
{
    drect(
        getX(),
        getY(),
        getX() + getW() - 1,
        getY() + getH() - 1,
        backgroundColor.getRGB()
    );

    drawOutlineRect(
        getX(),
        getY(),
        getX() + getW() - 1,
        getY() + getH() - 1,
        focused ? C_BLUE : borderColor.getRGB(),
        1
    );

    int count =
        static_cast<int>(
            tabs.size()
        );

    if(count <= 0)
        return;

    int tabWidth =
        getW() /
        count;

    for(int i = 0;
        i < count;
        ++i)
    {
        int x1 =
            getX() +
            i * tabWidth;

        int x2 =
            (i == count - 1)
                ? getX() +
                  getW() -
                  1
                : x1 +
                  tabWidth -
                  1;

        int fill =
            (i == selectedTab)
                ? C_LIGHT
                : C_WHITE;

        drect(
            x1,
            getY(),
            x2,
            getY() +
                headerHeight -
                1,
            fill
        );

        drawOutlineRect(
            x1,
            getY(),
            x2,
            getY() +
                headerHeight -
                1,
            (
                headerFocused &&
                i == selectedTab
            )
                ? C_BLUE
                : C_BLACK,
            (
                headerFocused &&
                i == selectedTab
            )
                ? 2
                : 1
        );

        dtext(
            x1 + 5,
            getY() +
                (headerHeight - 12) /
                2,
            C_BLACK,
            tabs[i].c_str()
        );
    }
}

int item_tab_view::addChildToTab(
    int tabIndex,
    item* control,
    STRUCT_pos localGeometry,
    item_margins margins,
    int anchors)
{
    if(
        tabIndex < 0 ||
        tabIndex >= static_cast<int>(
            tabs.size()
        ))
    {
        return -1;
    }

    int id =
        addChild(
            control,
            localGeometry,
            margins,
            anchors
        );

    if(id >= 0)
        setChildGroup(
            id,
            tabIndex
        );

    return id;
}

int item_tab_view::addSharedChild(
    item* control,
    STRUCT_pos localGeometry,
    item_margins margins,
    int anchors)
{
    int id =
        addChild(
            control,
            localGeometry,
            margins,
            anchors
        );

    if(id >= 0)
        setChildGroup(id, -1);

    return id;
}

int item_tab_view::addTab(
    const std::string& label)
{
    tabs.push_back(label);
    return static_cast<int>(tabs.size()) - 1;
}

bool item_tab_view::removeTab(int index)
{
    if(
        index < 0 ||
        index >= static_cast<int>(
            tabs.size()
        ) ||
        tabs.size() <= 1)
    {
        return false;
    }

    tabs.erase(
        tabs.begin() + index
    );

    for(container_child& child : children)
    {
        if(child.group == index)
        {
            child.visible = false;
        }
        else if(child.group > index)
        {
            --child.group;
        }
    }

    if(selectedTab >= static_cast<int>(tabs.size()))
        selectedTab = static_cast<int>(tabs.size()) - 1;

    clearChildFocus();
    performLayout();

    return true;
}

const std::string& item_tab_view::getTab(
    unsigned int index) const
{
    static const std::string empty = "";

    if(index >= tabs.size())
        return empty;

    return tabs[index];
}

void item_tab_view::setTabs(
    const std::vector<std::string>& value)
{
    tabs = value;

    if(tabs.empty())
        tabs.push_back("Tab");

    if(selectedTab >= static_cast<int>(tabs.size()))
        selectedTab = static_cast<int>(tabs.size()) - 1;

    clearChildFocus();
    performLayout();
}

void item_tab_view::setHeaderHeight(int value)
{
    headerHeight =
        (value >= 18)
            ? value
            : 18;

    performLayout();
}

bool item_tab_view::setSelectedTab(
    int index,
    bool notify)
{
    if(tabs.empty())
        return false;

    if(index < 0)
        index = 0;

    if(index >= static_cast<int>(tabs.size()))
        index = static_cast<int>(tabs.size()) - 1;

    bool changed =
        selectedTab != index;

    selectedTab = index;
    clearChildFocus();
    performLayout();

    if(changed && notify && onTabChanged)
        onTabChanged(selectedTab);

    return true;
}

bool item_tab_view::wantsPointerFocus(
    int x,
    int y) const
{
    if(!contains(x, y))
        return false;

    if(
        y >= getY() &&
        y < getY() + headerHeight)
    {
        return true;
    }

    return item_container::wantsPointerFocus(
        x,
        y
    );
}

int item_tab_view::pointerTabIndex() const
{
    if(
        getPointerY() < getY() ||
        getPointerY() >= getY() + headerHeight ||
        getPointerX() < getX() ||
        getPointerX() >= getX() + getW())
    {
        return -1;
    }

    int count =
        static_cast<int>(
            tabs.size()
        );

    if(count <= 0)
        return -1;

    int localX =
        getPointerX() -
        getX();

    int index =
        localX *
        count /
        getW();

    if(index >= count)
        index = count - 1;

    return index;
}

void item_tab_view::handleEvent(int eventType, int eventKey)
{
    if(headerFocused && focused && eventType == KEYEV_DOWN)
    {
        if(eventKey == KEY_LEFT)
        {
            int next = selectedTab - 1;
            if(next < 0) next = static_cast<int>(tabs.size()) - 1;
            setSelectedTab(next);
            return;
        }
        if(eventKey == KEY_RIGHT)
        {
            int next = selectedTab + 1;
            if(next >= static_cast<int>(tabs.size())) next = 0;
            setSelectedTab(next);
            return;
        }
        if(eventKey == KEY_DOWN)
        {
            headerFocused = false;
            focusNextChild();
            return;
        }
        if(eventKey == KEY_EXE) return;
    }

    if(eventType == KEYEV_DOWN && eventKey == KEY_EXE && param.status.hover)
    {
        int tab = pointerTabIndex();
        if(tab >= 0)
        {
            focused = true;
            headerFocused = true;
            clearChildFocus();
            setSelectedTab(tab);
            return;
        }
        headerFocused = false;
    }

    item_container::handleEvent(eventType, eventKey);
}

void item_tab_view::draw()
{
    if(!isVisible())
        return;

    drawChrome();
    drawChildren();
}


//******************************** Selection controls *********************************


item_radio_button::item_radio_button(
    STRUCT_pos _pos,
    std::string _label,
    bool _checked,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;
    label = _label;
    checked = _checked;
    zOrder = _zOrder;
}

void item_radio_button::setChecked(
    bool value,
    bool notify)
{
    if(checked == value)
        return;

    checked = value;

    if(notify && onChanged)
        onChanged(checked);
}

void item_radio_button::handleEvent(
    int eventType,
    int eventKey)
{
    if(
        eventType == KEYEV_DOWN &&
        eventKey == KEY_EXE &&
        param.status.hover &&
        !isDimmed())
    {
        if(checked && allowUncheck)
            setChecked(false);
        else
            setChecked(true);

        setClicked(false);
    }
}

void item_radio_button::draw()
{
    if(!isVisible())
        return;

    const item_theme& theme =
        current_item_theme();

    int box = getH() - 6;

    if(box < 12)
        box = 12;

    if(box > 18)
        box = 18;

    int cx = getX() + 3 + box / 2;
    int cy = getY() + getH() / 2;
    int radius = box / 2;

    drawCircleFilled(cx, cy, radius, theme.surface.getRGB());
    drawCircleOutline(cx, cy, radius, param.status.hover ? theme.accent.getRGB() : theme.border.getRGB());

    if(checked)
        drawCircleFilled(cx, cy, radius - 4, theme.accent.getRGB());

    dtext(
        getX() + box + 8,
        getY() + (getH() - 12) / 2,
        isDimmed()
            ? theme.disabled.getRGB()
            : theme.text.getRGB(),
        label.c_str()
    );
}


item_radio_group::item_radio_group(
    STRUCT_pos _pos,
    std::vector<std::string> _labels,
    int _selectedIndex,
    int _rowHeight,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;

    rowHeight = (_rowHeight >= 18) ? _rowHeight : 18;
    zOrder = _zOrder;

    setItems(_labels);
    setSelectedIndex(_selectedIndex, false);
}

void item_radio_group::setItems(
    const std::vector<std::string>& value)
{
    labels = value;

    if(labels.empty())
    {
        selectedIndex = -1;
        return;
    }

    if(selectedIndex < 0)
        selectedIndex = 0;

    if(selectedIndex >= static_cast<int>(labels.size()))
        selectedIndex = static_cast<int>(labels.size()) - 1;
}

int item_radio_group::addItem(const std::string& label)
{
    labels.push_back(label);

    if(selectedIndex < 0)
        selectedIndex = 0;

    return static_cast<int>(labels.size()) - 1;
}

bool item_radio_group::removeItem(unsigned int index)
{
    if(index >= labels.size())
        return false;

    labels.erase(labels.begin() + index);

    if(labels.empty())
        selectedIndex = -1;
    else if(selectedIndex >= static_cast<int>(labels.size()))
        selectedIndex = static_cast<int>(labels.size()) - 1;

    return true;
}

void item_radio_group::clearItems()
{
    labels.clear();
    selectedIndex = -1;
}

bool item_radio_group::setSelectedIndex(int index, bool notify)
{
    if(labels.empty())
    {
        selectedIndex = -1;
        return false;
    }

    if(index < 0)
        index = 0;

    if(index >= static_cast<int>(labels.size()))
        index = static_cast<int>(labels.size()) - 1;

    bool changed = selectedIndex != index;
    selectedIndex = index;

    if(changed && notify && onChanged)
        onChanged(selectedIndex, labels[selectedIndex]);

    return true;
}

std::string item_radio_group::getSelectedText() const
{
    if(selectedIndex < 0 || selectedIndex >= static_cast<int>(labels.size()))
        return "";

    return labels[selectedIndex];
}

int item_radio_group::pointerIndex() const
{
    if(!contains(getPointerX(), getPointerY()))
        return -1;

    int index = (getPointerY() - getY()) / rowHeight;

    if(index < 0 || index >= static_cast<int>(labels.size()))
        return -1;

    return index;
}

void item_radio_group::handleEvent(int eventType, int eventKey)
{
    if(eventType != KEYEV_DOWN && eventType != KEYEV_HOLD)
        return;

    if(eventType == KEYEV_DOWN && eventKey == KEY_EXE && param.status.hover)
    {
        focused = true;

        int index = pointerIndex();

        if(index >= 0)
            setSelectedIndex(index);

        return;
    }

    if(!focused || labels.empty())
        return;

    if(eventKey == KEY_UP || eventKey == KEY_LEFT)
        setSelectedIndex(selectedIndex - 1);

    if(eventKey == KEY_DOWN || eventKey == KEY_RIGHT)
        setSelectedIndex(selectedIndex + 1);
}

void item_radio_group::draw()
{
    if(!isVisible())
        return;

    const item_theme& theme =
        current_item_theme();

    drawOutlineRect(
        getX(),
        getY(),
        getX() + getW() - 1,
        getY() + getH() - 1,
        focused ? theme.accent.getRGB() : theme.border.getRGB(),
        1
    );

    int maxRows = getH() / rowHeight;
    int count = static_cast<int>(labels.size());

    if(count > maxRows)
        count = maxRows;

    for(int i = 0; i < count; ++i)
    {
        int rowY = getY() + i * rowHeight;
        int cx = getX() + 11;
        int cy = rowY + rowHeight / 2;

        drawCircleFilled(cx, cy, 7, theme.surface.getRGB());
        drawCircleOutline(cx, cy, 7, i == selectedIndex ? theme.accent.getRGB() : theme.border.getRGB());

        if(i == selectedIndex)
            drawCircleFilled(cx, cy, 3, theme.accent.getRGB());

        dtext(
            getX() + 23,
            rowY + (rowHeight - 12) / 2,
            theme.text.getRGB(),
            labels[i].c_str()
        );
    }
}


item_list_box::item_list_box(
    STRUCT_pos _pos,
    std::vector<std::string> _items,
    int _selectedIndex,
    int _rowHeight,
    bool _showScrollbar,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;

    rowHeight = (_rowHeight >= 16) ? _rowHeight : 16;
    showScrollbar = _showScrollbar;
    zOrder = _zOrder;

    setItems(_items);
    setSelectedIndex(_selectedIndex, false);

    applyTheme(
        current_item_theme()
    );
}

void item_list_box::applyTheme(
    const item_theme& theme)
{
    item::applyTheme(theme);

    backgroundColor = theme.surface;
    borderColor = theme.border;
    textColor = theme.text;
    selectionColor = theme.selection;
}

void item_list_box::setFocus(bool value)
{
    bool entering =
        value &&
        !focused;

    focused = value;

    if(!focused)
        focusJustAcquired = false;
    else if(entering)
        focusJustAcquired = param.status.hover;
}

void item_list_box::setItems(const std::vector<std::string>& value)
{
    items = value;

    if(items.empty())
    {
        selectedIndex = -1;
        scrollOffset = 0;
        return;
    }

    if(selectedIndex < 0)
        selectedIndex = 0;

    if(selectedIndex >= static_cast<int>(items.size()))
        selectedIndex = static_cast<int>(items.size()) - 1;

    ensureSelectionVisible();
}

int item_list_box::addItem(const std::string& value)
{
    items.push_back(value);

    if(selectedIndex < 0)
        selectedIndex = 0;

    ensureSelectionVisible();

    return static_cast<int>(items.size()) - 1;
}

bool item_list_box::removeItem(unsigned int index)
{
    if(index >= items.size())
        return false;

    items.erase(items.begin() + index);

    if(items.empty())
    {
        selectedIndex = -1;
        scrollOffset = 0;
    }
    else
    {
        if(selectedIndex >= static_cast<int>(items.size()))
            selectedIndex = static_cast<int>(items.size()) - 1;

        ensureSelectionVisible();
    }

    return true;
}

void item_list_box::clearItems()
{
    items.clear();
    selectedIndex = -1;
    scrollOffset = 0;
}

bool item_list_box::setSelectedIndex(int index, bool notify)
{
    if(items.empty())
    {
        selectedIndex = -1;
        return false;
    }

    if(index < 0)
        index = 0;

    if(index >= static_cast<int>(items.size()))
        index = static_cast<int>(items.size()) - 1;

    bool changed = selectedIndex != index;
    selectedIndex = index;
    ensureSelectionVisible();

    if(changed && notify && onSelected)
        onSelected(selectedIndex, items[selectedIndex]);

    return true;
}

std::string item_list_box::getSelectedText() const
{
    if(selectedIndex < 0 || selectedIndex >= static_cast<int>(items.size()))
        return "";

    return items[selectedIndex];
}

int item_list_box::rowCapacity() const
{
    int rows = (getH() - 4) / rowHeight;
    return (rows > 0) ? rows : 1;
}

int item_list_box::maxScrollOffset() const
{
    int maximum = static_cast<int>(items.size()) - rowCapacity();
    return (maximum > 0) ? maximum : 0;
}

void item_list_box::setScrollOffset(int value)
{
    if(value < 0)
        value = 0;

    int maximum = maxScrollOffset();

    if(value > maximum)
        value = maximum;

    scrollOffset = value;
}

void item_list_box::setRowHeight(int value)
{
    rowHeight = (value >= 16) ? value : 16;
    ensureSelectionVisible();
}

void item_list_box::ensureSelectionVisible()
{
    if(selectedIndex < 0)
        return;

    int capacity = rowCapacity();

    if(selectedIndex < scrollOffset)
        scrollOffset = selectedIndex;
    else if(selectedIndex >= scrollOffset + capacity)
        scrollOffset = selectedIndex - capacity + 1;

    setScrollOffset(scrollOffset);
}

void item_list_box::moveSelection(int delta)
{
    if(!items.empty())
        setSelectedIndex(selectedIndex + delta);
}

int item_list_box::pointerIndex() const
{
    if(!contains(getPointerX(), getPointerY()))
        return -1;

    int row = (getPointerY() - getY() - 2) / rowHeight;
    int index = scrollOffset + row;

    if(index < 0 || index >= static_cast<int>(items.size()))
        return -1;

    return index;
}

void item_list_box::handleEvent(int eventType, int eventKey)
{
    if(
        eventType != KEYEV_DOWN &&
        eventType != KEYEV_HOLD)
    {
        return;
    }

    if(!focused)
        return;

    if(
        focusJustAcquired &&
        eventType == KEYEV_DOWN &&
        eventKey == KEY_EXE)
    {
        focusJustAcquired = false;

        int index =
            pointerIndex();

        if(index >= 0)
            setSelectedIndex(index);

        return;
    }

    focusJustAcquired = false;

    if(eventKey == KEY_UP)
    {
        moveSelection(-1);
        return;
    }

    if(eventKey == KEY_DOWN)
    {
        moveSelection(1);
        return;
    }

    if(
        eventType == KEYEV_DOWN &&
        eventKey == KEY_EXE &&
        selectedIndex >= 0)
    {
        if(onActivated)
        {
            onActivated(
                selectedIndex,
                items[selectedIndex]
            );
        }

        return;
    }
}

void item_list_box::drawScrollbar()
{
    if(!showScrollbar)
        return;

    int total = static_cast<int>(items.size());
    int capacity = rowCapacity();

    if(total <= capacity || total <= 0)
        return;

    int x1 = getX() + getW() - 9;
    int x2 = getX() + getW() - 3;
    int y1 = getY() + 3;
    int y2 = getY() + getH() - 4;

    drect(x1, y1, x2, y2, C_LIGHT);

    int trackHeight = y2 - y1 + 1;
    int thumbHeight = trackHeight * capacity / total;

    if(thumbHeight < 8)
        thumbHeight = 8;

    if(thumbHeight > trackHeight)
        thumbHeight = trackHeight;

    int maximum = maxScrollOffset();
    int travel = trackHeight - thumbHeight;
    int thumbY = y1;

    if(maximum > 0)
        thumbY += travel * scrollOffset / maximum;

    drect(
        x1 + 1,
        thumbY,
        x2 - 1,
        thumbY + thumbHeight - 1,
        C_DARK
    );
}

void item_list_box::draw()
{
    if(!isVisible())
        return;

    drect(
        getX(),
        getY(),
        getX() + getW() - 1,
        getY() + getH() - 1,
        backgroundColor.getRGB()
    );

    drawOutlineRect(
        getX(),
        getY(),
        getX() + getW() - 1,
        getY() + getH() - 1,
        focused ? C_BLUE : borderColor.getRGB(),
        1
    );

    int capacity = rowCapacity();

    for(int row = 0; row < capacity; ++row)
    {
        int index = scrollOffset + row;

        if(index >= static_cast<int>(items.size()))
            break;

        int y = getY() + 2 + row * rowHeight;
        int right = getX() + getW() - (showScrollbar ? 11 : 3);

        if(index == selectedIndex)
        {
            drect(
                getX() + 2,
                y,
                right,
                y + rowHeight - 1,
                selectionColor.getRGB()
            );
        }

        dtext(
            getX() + 6,
            y + (rowHeight - 12) / 2,
            textColor.getRGB(),
            items[index].c_str()
        );
    }

    drawScrollbar();
}


item_combo_box::item_combo_box(
    STRUCT_pos _pos,
    std::vector<std::string> _items,
    int _selectedIndex,
    int _maxVisibleRows,
    int _rowHeight,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;

    maxVisibleRows = (_maxVisibleRows > 0) ? _maxVisibleRows : 1;
    rowHeight = (_rowHeight >= 18) ? _rowHeight : 18;
    zOrder = _zOrder;

    setItems(_items);
    setSelectedIndex(_selectedIndex, false);

    applyTheme(
        current_item_theme()
    );
}

void item_combo_box::applyTheme(
    const item_theme& theme)
{
    item::applyTheme(theme);

    backgroundColor = theme.surface;
    borderColor = theme.border;
    textColor = theme.text;
    selectionColor = theme.selection;
}

void item_combo_box::setFocus(bool value)
{
    bool entering =
        value &&
        !focused;

    focused = value;

    if(!focused)
    {
        focusJustAcquired = false;
        close();
    }
    else if(entering)
    {
        focusJustAcquired =
            param.status.hover;
    }
}

void item_combo_box::setItems(const std::vector<std::string>& value)
{
    items = value;

    if(items.empty())
    {
        selectedIndex = -1;
        highlightedIndex = -1;
        scrollOffset = 0;
        return;
    }

    if(selectedIndex < 0)
        selectedIndex = 0;

    if(selectedIndex >= static_cast<int>(items.size()))
        selectedIndex = static_cast<int>(items.size()) - 1;

    highlightedIndex = selectedIndex;
    ensureHighlightVisible();
}

int item_combo_box::addItem(const std::string& value)
{
    items.push_back(value);

    if(selectedIndex < 0)
    {
        selectedIndex = 0;
        highlightedIndex = 0;
    }

    return static_cast<int>(items.size()) - 1;
}

bool item_combo_box::removeItem(unsigned int index)
{
    if(index >= items.size())
        return false;

    items.erase(items.begin() + index);

    if(items.empty())
    {
        selectedIndex = -1;
        highlightedIndex = -1;
        scrollOffset = 0;
    }
    else
    {
        if(selectedIndex >= static_cast<int>(items.size()))
            selectedIndex = static_cast<int>(items.size()) - 1;

        highlightedIndex = selectedIndex;
        ensureHighlightVisible();
    }

    return true;
}

void item_combo_box::clearItems()
{
    items.clear();
    selectedIndex = -1;
    highlightedIndex = -1;
    scrollOffset = 0;
    close();
}

bool item_combo_box::setSelectedIndex(int index, bool notify)
{
    if(items.empty())
    {
        selectedIndex = -1;
        highlightedIndex = -1;
        return false;
    }

    if(index < 0)
        index = 0;

    if(index >= static_cast<int>(items.size()))
        index = static_cast<int>(items.size()) - 1;

    bool changed = selectedIndex != index;
    selectedIndex = index;
    highlightedIndex = index;
    ensureHighlightVisible();

    if(changed && notify && onChanged)
        onChanged(selectedIndex, items[selectedIndex]);

    return true;
}

std::string item_combo_box::getSelectedText() const
{
    if(selectedIndex < 0 || selectedIndex >= static_cast<int>(items.size()))
        return "";

    return items[selectedIndex];
}

int item_combo_box::dropdownRowCount() const
{
    int count = static_cast<int>(items.size());

    if(count > maxVisibleRows)
        count = maxVisibleRows;

    return count;
}

int item_combo_box::dropdownHeight() const
{
    return dropdownRowCount() * rowHeight;
}

bool item_combo_box::contains(int x, int y) const
{
    if(
        x >= getX() &&
        x < getX() + getW() &&
        y >= getY() &&
        y < getY() + getH())
    {
        return true;
    }

    if(!opened)
        return false;

    return
        x >= getX() &&
        x < getX() + getW() &&
        y >= getY() + getH() &&
        y < getY() + getH() + dropdownHeight();
}

void item_combo_box::open()
{
    if(items.empty())
        return;

    opened = true;
    focused = true;
    highlightedIndex = selectedIndex;
    ensureHighlightVisible();
}

void item_combo_box::close()
{
    opened = false;
}

void item_combo_box::toggle()
{
    if(opened)
        close();
    else
        open();
}

void item_combo_box::ensureHighlightVisible()
{
    if(highlightedIndex < 0)
        return;

    if(highlightedIndex < scrollOffset)
        scrollOffset = highlightedIndex;
    else if(highlightedIndex >= scrollOffset + maxVisibleRows)
        scrollOffset = highlightedIndex - maxVisibleRows + 1;

    int maximum = static_cast<int>(items.size()) - maxVisibleRows;

    if(maximum < 0)
        maximum = 0;

    if(scrollOffset > maximum)
        scrollOffset = maximum;

    if(scrollOffset < 0)
        scrollOffset = 0;
}

void item_combo_box::moveHighlight(int delta)
{
    if(items.empty())
        return;

    if(highlightedIndex < 0)
        highlightedIndex = selectedIndex;

    highlightedIndex += delta;

    if(highlightedIndex < 0)
        highlightedIndex = 0;

    if(highlightedIndex >= static_cast<int>(items.size()))
        highlightedIndex = static_cast<int>(items.size()) - 1;

    ensureHighlightVisible();
}

int item_combo_box::pointerDropdownIndex() const
{
    if(!opened)
        return -1;

    if(
        getPointerX() < getX() ||
        getPointerX() >= getX() + getW() ||
        getPointerY() < getY() + getH() ||
        getPointerY() >= getY() + getH() + dropdownHeight())
    {
        return -1;
    }

    int row = (getPointerY() - (getY() + getH())) / rowHeight;
    int index = scrollOffset + row;

    if(index < 0 || index >= static_cast<int>(items.size()))
        return -1;

    return index;
}

bool item_combo_box::handleSystemExit()
{
    if(opened || focused)
    {
        close();
        focused = false;
        return true;
    }

    return false;
}

void item_combo_box::handleEvent(int eventType, int eventKey)
{
    if(
        eventType != KEYEV_DOWN &&
        eventType != KEYEV_HOLD)
    {
        return;
    }

    if(!focused)
        return;

    if(
        focusJustAcquired &&
        eventType == KEYEV_DOWN &&
        eventKey == KEY_EXE)
    {
        focusJustAcquired = false;
        open();
        return;
    }

    focusJustAcquired = false;

    if(!opened)
    {
        if(
            eventType == KEYEV_DOWN &&
            eventKey == KEY_EXE)
        {
            open();
        }

        return;
    }

    if(eventKey == KEY_UP)
    {
        moveHighlight(-1);
        return;
    }

    if(eventKey == KEY_DOWN)
    {
        moveHighlight(1);
        return;
    }

    if(
        eventType == KEYEV_DOWN &&
        eventKey == KEY_EXE)
    {
        if(highlightedIndex >= 0)
            setSelectedIndex(highlightedIndex);

        close();
        focused = false;
        focusJustAcquired = false;
        return;
    }
}

void item_combo_box::draw()
{
    if(!isVisible())
        return;

    int x1 = getX();
    int y1 = getY();
    int x2 = getX() + getW() - 1;
    int y2 = getY() + getH() - 1;

    drect(x1, y1, x2, y2, backgroundColor.getRGB());

    drawOutlineRect(
        x1,
        y1,
        x2,
        y2,
        focused ? C_BLUE : borderColor.getRGB(),
        1
    );

    if(selectedIndex >= 0)
    {
        dtext(
            x1 + 6,
            y1 + (getH() - 12) / 2,
            textColor.getRGB(),
            items[selectedIndex].c_str()
        );
    }

    int ax = x2 - 12;
    int ay = y1 + getH() / 2;

    dline(ax, ay - 2, ax + 4, ay + 2, C_BLACK);
    dline(ax + 4, ay + 2, ax + 8, ay - 2, C_BLACK);

    if(!opened)
        return;

    int rows = dropdownRowCount();
    int dropTop = y2 + 1;
    int dropBottom = dropTop + rows * rowHeight - 1;

    drect(x1, dropTop, x2, dropBottom, C_WHITE);
    drawOutlineRect(x1, dropTop, x2, dropBottom, C_BLACK, 1);

    for(int row = 0; row < rows; ++row)
    {
        int index = scrollOffset + row;

        if(index >= static_cast<int>(items.size()))
            break;

        int rowY = dropTop + row * rowHeight;

        if(index == highlightedIndex)
        {
            drect(
                x1 + 1,
                rowY + 1,
                x2 - 1,
                rowY + rowHeight - 1,
                selectionColor.getRGB()
            );
        }

        dtext(
            x1 + 6,
            rowY + (rowHeight - 12) / 2,
            C_BLACK,
            items[index].c_str()
        );
    }
}


//******************************** Tree *********************************


item_tree::item_tree(
    STRUCT_pos _pos,
    int _rowHeight,
    int _indentWidth,
    bool _showLines,
    bool _showScrollbar,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;

    rowHeight =
        (_rowHeight >= 12)
            ? _rowHeight
            : 12;

    indentWidth =
        (_indentWidth >= 8)
            ? _indentWidth
            : 8;

    showLines = _showLines;
    showScrollbar = _showScrollbar;
    zOrder = _zOrder;
}

void item_tree::setFocus(bool value)
{
    bool entering =
        value &&
        !focused;

    focused = value;

    if(!focused)
        focusJustAcquired = false;
    else if(entering)
        focusJustAcquired = param.status.hover;
}

int item_tree::addNode(
    int parentId,
    const std::string& label,
    bool expanded,
    const bopti_image_t* icon)
{
    int newId = nextNodeId;

    while(nodes.find(newId) != nodes.end())
        ++newId;

    nextNodeId = newId + 1;

    if(!addNodeWithId(
        newId,
        parentId,
        label,
        expanded,
        icon))
    {
        return -1;
    }

    return newId;
}

bool item_tree::addNodeWithId(
    int id,
    int parentId,
    const std::string& label,
    bool expanded,
    const bopti_image_t* icon)
{
    if(id < 0 || nodes.find(id) != nodes.end())
        return false;

    if(parentId >= 0 && nodes.find(parentId) == nodes.end())
        return false;

    tree_node node(
        id,
        parentId,
        label,
        expanded,
        true,
        true,
        icon
    );

    nodes.emplace(
        id,
        node
    );

    if(parentId < 0)
    {
        roots.push_back(id);
    }
    else
    {
        nodes[parentId].children.push_back(id);
    }

    if(id >= nextNodeId)
        nextNodeId = id + 1;

    if(selectedNodeId < 0)
        selectedNodeId = id;

    invalidateVisibleCache();
    ensureSelectedVisible();

    return true;
}

tree_node* item_tree::getNode(int id)
{
    auto it = nodes.find(id);

    if(it == nodes.end())
        return nullptr;

    return &it->second;
}

const tree_node* item_tree::getNode(int id) const
{
    auto it = nodes.find(id);

    if(it == nodes.end())
        return nullptr;

    return &it->second;
}

bool item_tree::hasNode(int id) const
{
    return nodes.find(id) != nodes.end();
}

std::vector<int> item_tree::getAllNodeIds() const
{
    std::vector<int> ids;
    ids.reserve(nodes.size());

    for(const auto& entry : nodes)
        ids.push_back(entry.first);

    return ids;
}

void item_tree::removeFromParent(int id)
{
    auto it = nodes.find(id);

    if(it == nodes.end())
        return;

    int parentId = it->second.parentId;
    std::vector<int>* list = nullptr;

    if(parentId < 0)
    {
        list = &roots;
    }
    else
    {
        auto parent = nodes.find(parentId);

        if(parent == nodes.end())
            return;

        list = &parent->second.children;
    }

    for(auto childIt = list->begin();
        childIt != list->end();
        ++childIt)
    {
        if(*childIt == id)
        {
            list->erase(childIt);
            break;
        }
    }
}

void item_tree::removeNodeRecursive(int id)
{
    auto it = nodes.find(id);

    if(it == nodes.end())
        return;

    std::vector<int> children =
        it->second.children;

    for(int childId : children)
        removeNodeRecursive(childId);

    nodes.erase(id);
}

bool item_tree::removeNode(
    int id,
    bool recursive)
{
    auto it = nodes.find(id);

    if(it == nodes.end())
        return false;

    if(!recursive && !it->second.children.empty())
        return false;

    removeFromParent(id);
    removeNodeRecursive(id);
    invalidateVisibleCache();

    if(!hasNode(activatedNodeId))
        activatedNodeId = -1;

    if(!hasNode(selectedNodeId))
    {
        const std::vector<int>& visible =
            visibleNodeIds();

        selectedNodeId =
            visible.empty()
                ? -1
                : visible.front();
    }

    setScrollOffset(scrollOffset);
    ensureSelectedVisible();

    return true;
}

void item_tree::clear()
{
    nodes.clear();
    roots.clear();

    nextNodeId = 1;
    selectedNodeId = -1;
    activatedNodeId = -1;
    scrollOffset = 0;

    visibleNodeCache.clear();
    visibleNodeCacheDirty = true;
}

bool item_tree::setSelectedNode(int id)
{
    tree_node* node = getNode(id);

    if(
        node == nullptr ||
        !node->visible ||
        !node->enabled ||
        getVisibleIndex(id) < 0)
    {
        return false;
    }

    bool changed =
        selectedNodeId != id;

    selectedNodeId = id;
    ensureSelectedVisible();

    if(changed && onSelected)
    {
        onSelected(
            id,
            node->label
        );
    }

    return true;
}

bool item_tree::setNodeExpanded(
    int id,
    bool expanded)
{
    tree_node* node = getNode(id);

    if(node == nullptr)
        return false;

    if(node->children.empty())
        expanded = false;

    if(node->expanded == expanded)
        return true;

    node->expanded = expanded;
    invalidateVisibleCache();

    if(onExpanded)
    {
        onExpanded(
            id,
            expanded
        );
    }

    if(getVisibleIndex(selectedNodeId) < 0)
        selectedNodeId = id;

    ensureSelectedVisible();

    return true;
}

bool item_tree::toggleNode(int id)
{
    tree_node* node = getNode(id);

    if(node == nullptr || node->children.empty())
        return false;

    return setNodeExpanded(
        id,
        !node->expanded
    );
}

bool item_tree::setNodeLabel(
    int id,
    const std::string& label)
{
    tree_node* node = getNode(id);

    if(node == nullptr)
        return false;

    node->label = label;
    return true;
}

bool item_tree::setNodeVisible(
    int id,
    bool visible)
{
    tree_node* node = getNode(id);

    if(node == nullptr)
        return false;

    node->visible = visible;
    invalidateVisibleCache();

    if(getVisibleIndex(selectedNodeId) < 0)
    {
        const std::vector<int>& visibleNodes =
            visibleNodeIds();

        selectedNodeId =
            visibleNodes.empty()
                ? -1
                : visibleNodes.front();
    }

    setScrollOffset(scrollOffset);
    ensureSelectedVisible();

    return true;
}

bool item_tree::setNodeEnabled(
    int id,
    bool enabled)
{
    tree_node* node = getNode(id);

    if(node == nullptr)
        return false;

    node->enabled = enabled;

    if(!enabled && selectedNodeId == id)
        selectRelative(1);

    return true;
}

bool item_tree::setNodeIcon(
    int id,
    const bopti_image_t* icon)
{
    tree_node* node = getNode(id);

    if(node == nullptr)
        return false;

    node->icon = icon;
    return true;
}

void item_tree::setRowHeight(int value)
{
    rowHeight =
        (value >= 12)
            ? value
            : 12;

    setScrollOffset(scrollOffset);
    ensureSelectedVisible();
}

void item_tree::setIndentWidth(int value)
{
    indentWidth =
        (value >= 8)
            ? value
            : 8;
}

void item_tree::appendVisibleNodes(
    int id,
    std::vector<int>& output) const
{
    const tree_node* node =
        getNode(id);

    if(node == nullptr || !node->visible)
        return;

    output.push_back(id);

    if(!node->expanded)
        return;

    for(int childId : node->children)
    {
        appendVisibleNodes(
            childId,
            output
        );
    }
}

void item_tree::invalidateVisibleCache()
{
    visibleNodeCacheDirty = true;
}

const std::vector<int>& item_tree::visibleNodeIds() const
{
    if(!visibleNodeCacheDirty)
        return visibleNodeCache;

    visibleNodeCache.clear();

    if(visibleNodeCache.capacity() < nodes.size())
        visibleNodeCache.reserve(nodes.size());

    for(int rootId : roots)
    {
        appendVisibleNodes(
            rootId,
            visibleNodeCache
        );
    }

    visibleNodeCacheDirty = false;
    return visibleNodeCache;
}

std::vector<int> item_tree::getVisibleNodeIds() const
{
    const std::vector<int>& cached =
        visibleNodeIds();

    return std::vector<int>(
        cached.begin(),
        cached.end()
    );
}

int item_tree::getDepth(int id) const
{
    int depth = 0;
    const tree_node* node = getNode(id);

    while(
        node != nullptr &&
        node->parentId >= 0)
    {
        ++depth;
        node = getNode(node->parentId);
    }

    return depth;
}

int item_tree::getVisibleIndex(int id) const
{
    const std::vector<int>& visible =
        visibleNodeIds();

    for(unsigned int i = 0;
        i < visible.size();
        ++i)
    {
        if(visible[i] == id)
            return static_cast<int>(i);
    }

    return -1;
}

int item_tree::visibleRowCapacity() const
{
    int innerHeight = getH() - 4;

    if(innerHeight <= 0)
        return 1;

    int rows = innerHeight / rowHeight;

    return (rows > 0) ? rows : 1;
}

int item_tree::maxScrollOffset() const
{
    int count =
        static_cast<int>(
            visibleNodeIds().size()
        );

    int maxOffset =
        count -
        visibleRowCapacity();

    return
        (maxOffset > 0)
            ? maxOffset
            : 0;
}

void item_tree::setScrollOffset(int value)
{
    if(value < 0)
        value = 0;

    int maximum =
        maxScrollOffset();

    if(value > maximum)
        value = maximum;

    scrollOffset = value;
}

void item_tree::ensureSelectedVisible()
{
    if(selectedNodeId < 0)
        return;

    int index =
        getVisibleIndex(
            selectedNodeId
        );

    if(index < 0)
        return;

    int capacity =
        visibleRowCapacity();

    if(index < scrollOffset)
    {
        scrollOffset = index;
    }
    else if(
        index >= scrollOffset + capacity)
    {
        scrollOffset =
            index - capacity + 1;
    }

    setScrollOffset(scrollOffset);
}

void item_tree::selectRelative(int delta)
{
    const std::vector<int>& visible =
        visibleNodeIds();

    if(visible.empty())
    {
        selectedNodeId = -1;
        return;
    }

    int current =
        getVisibleIndex(
            selectedNodeId
        );

    if(current < 0)
        current = 0;

    int direction =
        (delta >= 0) ? 1 : -1;

    int index =
        current + direction;

    while(
        index >= 0 &&
        index < static_cast<int>(
            visible.size()
        ))
    {
        tree_node* node =
            getNode(visible[index]);

        if(
            node != nullptr &&
            node->enabled)
        {
            setSelectedNode(node->id);
            return;
        }

        index += direction;
    }
}

void item_tree::selectParentOrCollapse()
{
    tree_node* node =
        getNode(selectedNodeId);

    if(node == nullptr)
        return;

    if(
        node->expanded &&
        !node->children.empty())
    {
        setNodeExpanded(
            node->id,
            false
        );
        return;
    }

    if(node->parentId >= 0)
        setSelectedNode(node->parentId);
}

void item_tree::expandOrSelectChild()
{
    tree_node* node =
        getNode(selectedNodeId);

    if(
        node == nullptr ||
        node->children.empty())
    {
        return;
    }

    if(!node->expanded)
    {
        setNodeExpanded(
            node->id,
            true
        );
        return;
    }

    for(int childId : node->children)
    {
        tree_node* child =
            getNode(childId);

        if(
            child != nullptr &&
            child->visible &&
            child->enabled)
        {
            setSelectedNode(childId);
            return;
        }
    }
}

void item_tree::activateSelected()
{
    tree_node* node =
        getNode(selectedNodeId);

    if(
        node == nullptr ||
        !node->enabled)
    {
        return;
    }

    activatedNodeId =
        node->id;

    if(!node->children.empty())
        toggleNode(node->id);

    if(onActivated)
    {
        onActivated(
            node->id,
            node->label
        );
    }
}

int item_tree::getNodeAtPointer() const
{
    if(!contains(
        getPointerX(),
        getPointerY()))
    {
        return -1;
    }

    int localY =
        getPointerY() -
        getY() -
        2;

    if(localY < 0)
        return -1;

    int row =
        localY /
        rowHeight;

    int index =
        scrollOffset + row;

    const std::vector<int>& visible =
        visibleNodeIds();

    if(
        index < 0 ||
        index >= static_cast<int>(
            visible.size()
        ))
    {
        return -1;
    }

    return visible[index];
}

bool item_tree::pointerOnExpander(int id) const
{
    const tree_node* node =
        getNode(id);

    if(
        node == nullptr ||
        node->children.empty())
    {
        return false;
    }

    int depth =
        getDepth(id);

    int expanderX =
        getX() +
        5 +
        depth * indentWidth;

    return
        getPointerX() >= expanderX - 2 &&
        getPointerX() <= expanderX + 9;
}

void item_tree::handleEvent(
    int eventType,
    int eventKey)
{
    if(
        eventType != KEYEV_DOWN &&
        eventType != KEYEV_HOLD)
    {
        return;
    }

    if(!focused)
        return;

    if(
        focusJustAcquired &&
        eventType == KEYEV_DOWN &&
        eventKey == KEY_EXE)
    {
        focusJustAcquired = false;

        int nodeId =
            getNodeAtPointer();

        if(nodeId >= 0)
        {
            setSelectedNode(nodeId);

            if(pointerOnExpander(nodeId))
                toggleNode(nodeId);
        }

        return;
    }

    focusJustAcquired = false;

    if(eventKey == KEY_UP)
    {
        selectRelative(-1);
        return;
    }

    if(eventKey == KEY_DOWN)
    {
        selectRelative(1);
        return;
    }

    if(eventKey == KEY_LEFT)
    {
        selectParentOrCollapse();
        return;
    }

    if(eventKey == KEY_RIGHT)
    {
        expandOrSelectChild();
        return;
    }

    if(
        eventType == KEYEV_DOWN &&
        eventKey == KEY_EXE)
    {
        activateSelected();
    }
}

void item_tree::drawRow(
    int id,
    int rowIndex)
{
    tree_node* node = getNode(id);

    if(node == nullptr)
        return;

    int rowY =
        getY() +
        2 +
        rowIndex * rowHeight;

    int rowX1 = getX() + 2;
    int rowX2 =
        getX() +
        getW() -
        (showScrollbar ? 11 : 3);

    if(id == selectedNodeId)
    {
        drect(
            rowX1,
            rowY,
            rowX2,
            rowY + rowHeight - 1,
            selectionColor.getRGB()
        );
    }

    if(id == activatedNodeId)
    {
        int markerY =
            rowY +
            rowHeight / 2;

        drect(
            rowX2 - 6,
            markerY - 2,
            rowX2 - 2,
            markerY + 2,
            current_item_theme().accent.getRGB()
        );
    }

    int depth = getDepth(id);

    int baseX =
        getX() +
        5 +
        depth * indentWidth;

    if(showLines && depth > 0)
    {
        const tree_node* current = node;

        for(int level = depth;
            level > 0 &&
            current != nullptr;
            --level)
        {
            int lineX =
                getX() +
                9 +
                (level - 1) *
                indentWidth;

            dline(
                lineX,
                rowY,
                lineX,
                rowY + rowHeight - 1,
                lineColor.getRGB()
            );

            current =
                getNode(
                    current->parentId
                );
        }

        dline(
            baseX - indentWidth + 4,
            rowY + rowHeight / 2,
            baseX,
            rowY + rowHeight / 2,
            lineColor.getRGB()
        );
    }

    int contentX = baseX;

    if(!node->children.empty())
    {
        int cy =
            rowY +
            rowHeight / 2;

        drect_border(
            contentX,
            cy - 4,
            contentX + 8,
            cy + 4,
            C_WHITE,
            1,
            borderColor.getRGB()
        );

        dline(
            contentX + 2,
            cy,
            contentX + 6,
            cy,
            borderColor.getRGB()
        );

        if(!node->expanded)
        {
            dline(
                contentX + 4,
                cy - 2,
                contentX + 4,
                cy + 2,
                borderColor.getRGB()
            );
        }
    }

    contentX += 12;

    if(node->icon != nullptr)
    {
        int iconSize =
            rowHeight - 4;

        if(iconSize > 12)
            iconSize = 12;

        if(iconSize > 0)
        {
            dsubimage(
                contentX,
                rowY + 2,
                node->icon,
                0,
                0,
                iconSize,
                iconSize,
                DIMAGE_NONE
            );

            contentX +=
                iconSize + 3;
        }
    }

    int color =
        node->enabled
            ? textColor.getRGB()
            : disabledTextColor.getRGB();

    dtext(
        contentX,
        rowY + 3,
        color,
        node->label.c_str()
    );
}

void item_tree::drawScrollbar()
{
    if(!showScrollbar)
        return;

    int total =
        static_cast<int>(
            visibleNodeIds().size()
        );

    int capacity =
        visibleRowCapacity();

    if(total <= capacity || total <= 0)
        return;

    int x1 =
        getX() +
        getW() -
        9;

    int x2 =
        getX() +
        getW() -
        3;

    int y1 = getY() + 3;
    int y2 =
        getY() +
        getH() -
        4;

    drect(
        x1,
        y1,
        x2,
        y2,
        C_LIGHT
    );

    int trackHeight =
        y2 - y1 + 1;

    int thumbHeight =
        trackHeight *
        capacity /
        total;

    if(thumbHeight < 8)
        thumbHeight = 8;

    if(thumbHeight > trackHeight)
        thumbHeight = trackHeight;

    int maxOffset =
        maxScrollOffset();

    int thumbTravel =
        trackHeight -
        thumbHeight;

    int thumbY = y1;

    if(maxOffset > 0)
    {
        thumbY +=
            thumbTravel *
            scrollOffset /
            maxOffset;
    }

    drect(
        x1 + 1,
        thumbY,
        x2 - 1,
        thumbY + thumbHeight - 1,
        C_DARK
    );
}

void item_tree::draw()
{
    if(!isVisible())
        return;

    drect(
        getX(),
        getY(),
        getX() + getW() - 1,
        getY() + getH() - 1,
        backgroundColor.getRGB()
    );

    drawOutlineRect(
        getX(),
        getY(),
        getX() + getW() - 1,
        getY() + getH() - 1,
        focused
            ? C_BLUE
            : borderColor.getRGB(),
        1
    );

    const std::vector<int>& visible =
        visibleNodeIds();

    int capacity =
        visibleRowCapacity();

    for(int row = 0;
        row < capacity;
        ++row)
    {
        int index =
            scrollOffset +
            row;

        if(
            index >= static_cast<int>(
                visible.size()
            ))
        {
            break;
        }

        drawRow(
            visible[index],
            row
        );
    }

    drawScrollbar();
}


//******************************** Table *********************************

item_table::item_table(
    STRUCT_pos _pos,
    unsigned int rows,
    unsigned int columns,
    int _cellWidth,
    int _cellHeight,
    bool _showHeaders,
    int _zOrder)
{
    param.pos = _pos;
    param.status.visible = true;

    cellWidth = (_cellWidth >= 20) ? _cellWidth : 20;
    cellHeight = (_cellHeight >= 14) ? _cellHeight : 14;
    showHeaders = _showHeaders;

    zOrder = _zOrder;

    resize(rows, columns);
}

void item_table::resize(
    unsigned int rows,
    unsigned int columns)
{
    if(rows == 0)
        rows = 1;

    if(columns == 0)
        columns = 1;

    data.resize(rows);

    for(auto& row : data)
        row.resize(columns);

    columnLabels.resize(columns);

    for(unsigned int column = 0; column < columns; ++column)
    {
        if(columnLabels[column].empty())
        {
            if(column < 26)
            {
                char label[2] = {
                    static_cast<char>('A' + column),
                    0
                };

                columnLabels[column] = label;
            }
            else
            {
                columnLabels[column] = "#";
            }
        }
    }

    if(selectedRow >= rows)
        selectedRow = rows - 1;

    if(selectedColumn >= columns)
        selectedColumn = columns - 1;

    ensureSelectionVisible();
}

bool item_table::setCell(
    unsigned int row,
    unsigned int column,
    const std::string& value)
{
    if(
        row >= getRowCount() ||
        column >= getColumnCount())
    {
        return false;
    }

    data[row][column] = value;

    if(onCellChanged)
        onCellChanged(row, column, data[row][column]);

    return true;
}

const std::string& item_table::getCell(
    unsigned int row,
    unsigned int column) const
{
    static const std::string empty = "";

    if(
        row >= getRowCount() ||
        column >= getColumnCount())
    {
        return empty;
    }

    return data[row][column];
}

void item_table::clearCell(
    unsigned int row,
    unsigned int column)
{
    setCell(row, column, "");
}

void item_table::clearAll()
{
    for(auto& row : data)
    {
        for(auto& cell : row)
            cell.clear();
    }
}

void item_table::setColumnLabel(
    unsigned int column,
    const std::string& label)
{
    if(column >= columnLabels.size())
        return;

    columnLabels[column] = label;
}

const std::string& item_table::getColumnLabel(
    unsigned int column) const
{
    static const std::string empty = "";

    if(column >= columnLabels.size())
        return empty;

    return columnLabels[column];
}

void item_table::setCellSize(
    int width,
    int height)
{
    cellWidth = (width >= 20) ? width : 20;
    cellHeight = (height >= 14) ? height : 14;

    ensureSelectionVisible();
}

unsigned int item_table::visibleRows() const
{
    int availableHeight =
        getH() -
        (showHeaders ? headerHeight : 0);

    if(availableHeight <= 0)
        return 1;

    unsigned int count =
        static_cast<unsigned int>(
            availableHeight / cellHeight
        );

    return (count > 0) ? count : 1;
}

unsigned int item_table::visibleColumns() const
{
    int availableWidth =
        getW() -
        (showHeaders ? rowHeaderWidth : 0);

    if(availableWidth <= 0)
        return 1;

    unsigned int count =
        static_cast<unsigned int>(
            availableWidth / cellWidth
        );

    return (count > 0) ? count : 1;
}

void item_table::ensureSelectionVisible()
{
    unsigned int rowCount = getRowCount();
    unsigned int columnCount = getColumnCount();

    if(rowCount == 0 || columnCount == 0)
        return;

    unsigned int rowsVisible = visibleRows();
    unsigned int columnsVisible = visibleColumns();

    if(selectedRow < rowOffset)
        rowOffset = selectedRow;

    if(selectedRow >= rowOffset + rowsVisible)
        rowOffset = selectedRow - rowsVisible + 1;

    if(selectedColumn < columnOffset)
        columnOffset = selectedColumn;

    if(selectedColumn >= columnOffset + columnsVisible)
        columnOffset = selectedColumn - columnsVisible + 1;
}

void item_table::notifyCellChanged()
{
    if(onCellChanged)
    {
        onCellChanged(
            selectedRow,
            selectedColumn,
            data[selectedRow][selectedColumn]
        );
    }
}

void item_table::notifySelectionChanged()
{
    if(onSelectionChanged)
    {
        onSelectionChanged(
            selectedRow,
            selectedColumn
        );
    }
}

void item_table::setSelectedCell(
    unsigned int row,
    unsigned int column)
{
    if(
        row >= getRowCount() ||
        column >= getColumnCount())
    {
        return;
    }

    bool changed =
        row != selectedRow ||
        column != selectedColumn;

    selectedRow = row;
    selectedColumn = column;

    ensureSelectionVisible();

    if(changed)
        notifySelectionChanged();
}

char item_table::keyToChar(int key) const
{
    switch(key)
    {
        case KEY_0: return '0';
        case KEY_1: return '1';
        case KEY_2: return '2';
        case KEY_3: return '3';
        case KEY_4: return '4';
        case KEY_5: return '5';
        case KEY_6: return '6';
        case KEY_7: return '7';
        case KEY_8: return '8';
        case KEY_9: return '9';

        case KEY_DOT:    return '.';
        case KEY_ADD:    return '+';
        case KEY_SUB:    return '-';
        case KEY_MUL:    return '*';
        case KEY_DIV:    return '/';
        case KEY_LEFTP:  return '(';
        case KEY_RIGHTP: return ')';
        case KEY_COMMA:  return ',';

        default:
            return 0;
    }
}

void item_table::selectFromPointer()
{
    if(!param.status.hover)
        return;

    int localX = getPointerX() - getX();
    int localY = getPointerY() - getY();

    int dataX =
        localX -
        (showHeaders ? rowHeaderWidth : 0);

    int dataY =
        localY -
        (showHeaders ? headerHeight : 0);

    if(dataX < 0 || dataY < 0)
        return;

    unsigned int column =
        columnOffset +
        static_cast<unsigned int>(dataX / cellWidth);

    unsigned int row =
        rowOffset +
        static_cast<unsigned int>(dataY / cellHeight);

    if(
        row < getRowCount() &&
        column < getColumnCount())
    {
        setSelectedCell(row, column);
    }
}

void item_table::handleEvent(
    int eventType,
    int eventKey)
{
    if(eventType != KEYEV_DOWN && eventType != KEYEV_HOLD)
        return;

    if(
        eventType == KEYEV_DOWN &&
        eventKey == KEY_EXE &&
        param.status.hover)
    {
        selectFromPointer();
        editing = !editing;
        return;
    }

    if(!focused)
        return;

    if(eventType == KEYEV_DOWN && eventKey == KEY_EXE)
    {
        editing = !editing;
        return;
    }

    if(editing)
    {
        if(eventType != KEYEV_DOWN)
            return;

        std::string& cell =
            data[selectedRow][selectedColumn];

        if(eventKey == KEY_DEL)
        {
            if(!cell.empty())
            {
                cell.pop_back();
                notifyCellChanged();
            }

            return;
        }

        char c = keyToChar(eventKey);

        if(c != 0 && cell.size() < 64)
        {
            cell += c;
            notifyCellChanged();
        }

        return;
    }

    unsigned int row = selectedRow;
    unsigned int column = selectedColumn;

    if(eventKey == KEY_LEFT && column > 0)
        --column;

    if(eventKey == KEY_RIGHT && column + 1 < getColumnCount())
        ++column;

    if(eventKey == KEY_UP && row > 0)
        --row;

    if(eventKey == KEY_DOWN && row + 1 < getRowCount())
        ++row;

    setSelectedCell(row, column);
}

void item_table::draw()
{
    if(!isVisible())
        return;

    int x1 = getX();
    int y1 = getY();
    int x2 = getX() + getW() - 1;
    int y2 = getY() + getH() - 1;

    drect(x1, y1, x2, y2, C_WHITE);

    drawOutlineRect(
        x1,
        y1,
        x2,
        y2,
        focused ? C_BLUE : C_BLACK,
        focused ? 2 : 1
    );

    int dataStartX =
        x1 +
        (showHeaders ? rowHeaderWidth : 0);

    int dataStartY =
        y1 +
        (showHeaders ? headerHeight : 0);

    unsigned int rowsVisible = visibleRows();
    unsigned int columnsVisible = visibleColumns();

    if(showHeaders)
    {
        drect(
            dataStartX,
            y1,
            x2,
            dataStartY - 1,
            C_LIGHT
        );

        drect(
            x1,
            dataStartY,
            dataStartX - 1,
            y2,
            C_LIGHT
        );
    }

    for(unsigned int visibleColumn = 0;
        visibleColumn < columnsVisible;
        ++visibleColumn)
    {
        unsigned int column =
            columnOffset + visibleColumn;

        if(column >= getColumnCount())
            break;

        int cellX1 =
            dataStartX +
            static_cast<int>(visibleColumn) * cellWidth;

        int cellX2 =
            cellX1 + cellWidth - 1;

        if(cellX1 > x2)
            break;

        if(cellX2 > x2)
            cellX2 = x2;

        if(showHeaders)
        {
            drawOutlineRect(
                cellX1,
                y1,
                cellX2,
                dataStartY - 1,
                C_BLACK,
                1
            );

            int maxChars =
                (cellX2 - cellX1 - 6) / 7;

            if(maxChars < 1)
                maxChars = 1;

            dtext_opt(
                cellX1 + 3,
                y1 + 4,
                C_BLACK,
                C_NONE,
                DTEXT_LEFT,
                DTEXT_TOP,
                columnLabels[column].c_str(),
                maxChars
            );
        }
    }

    for(unsigned int visibleRow = 0;
        visibleRow < rowsVisible;
        ++visibleRow)
    {
        unsigned int row =
            rowOffset + visibleRow;

        if(row >= getRowCount())
            break;

        int cellY1 =
            dataStartY +
            static_cast<int>(visibleRow) * cellHeight;

        int cellY2 =
            cellY1 + cellHeight - 1;

        if(cellY1 > y2)
            break;

        if(cellY2 > y2)
            cellY2 = y2;

        if(showHeaders)
        {
            drawOutlineRect(
                x1,
                cellY1,
                dataStartX - 1,
                cellY2,
                C_BLACK,
                1
            );

            dprint(
                x1 + 3,
                cellY1 + 4,
                C_BLACK,
                "%d",
                static_cast<int>(row + 1)
            );
        }

        for(unsigned int visibleColumn = 0;
            visibleColumn < columnsVisible;
            ++visibleColumn)
        {
            unsigned int column =
                columnOffset + visibleColumn;

            if(column >= getColumnCount())
                break;

            int cellX1 =
                dataStartX +
                static_cast<int>(visibleColumn) * cellWidth;

            int cellX2 =
                cellX1 + cellWidth - 1;

            if(cellX1 > x2)
                break;

            if(cellX2 > x2)
                cellX2 = x2;

            bool selected =
                row == selectedRow &&
                column == selectedColumn;

            if(selected)
            {
                drect(
                    cellX1,
                    cellY1,
                    cellX2,
                    cellY2,
                    editing ? C_DARK : C_LIGHT
                );
            }

            drawOutlineRect(
                cellX1,
                cellY1,
                cellX2,
                cellY2,
                selected && focused ? C_BLUE : C_BLACK,
                selected && focused ? 2 : 1
            );

            int maxChars =
                (cellX2 - cellX1 - 6) / 7;

            if(maxChars < 1)
                maxChars = 1;

            dtext_opt(
                cellX1 + 3,
                cellY1 + 5,
                selected && editing ? C_WHITE : C_BLACK,
                C_NONE,
                DTEXT_LEFT,
                DTEXT_TOP,
                data[row][column].c_str(),
                maxChars
            );
        }
    }
}


//******************************** Image controls *********************************

item_image::item_image(
    STRUCT_pos _pos,
    bopti_image_t const* _image,
    int _sourceX,
    int _sourceY,
    bool _drawBorder,
    Class_color _borderColor,
    int _zOrder,
    item_image_mode _mode)
{
    param.pos = _pos;
    param.status.visible = true;

    image = _image;
    sourceX = (_sourceX >= 0) ? _sourceX : 0;
    sourceY = (_sourceY >= 0) ? _sourceY : 0;
    drawBorder = _drawBorder;
    borderColor = _borderColor;
    imageMode = _mode;

    // Default black means "follow theme border". A non-black constructor
    // color is considered an explicit custom border.
    useThemeBorder =
        _borderColor.getR() == 0 &&
        _borderColor.getG() == 0 &&
        _borderColor.getB() == 0;

    zOrder = _zOrder;

    applyTheme(
        current_item_theme()
    );
}

void item_image::applyTheme(
    const item_theme& theme)
{
    item::applyTheme(theme);

    if(useThemeBorder)
        borderColor = theme.border;
}

item_image::~item_image()
{
    clearImageCache();
}

void item_image::clearImageCache()
{
    if(transformedImage != nullptr)
    {
        image_free(transformedImage);
        transformedImage = nullptr;
    }
}

void item_image::invalidateImageCache()
{
    imageCacheDirty = true;
}

void item_image::setImage(bopti_image_t const* value)
{
    if(image == value)
        return;

    image = value;
    invalidateImageCache();
}

void item_image::setSource(int x, int y)
{
    sourceX = (x >= 0) ? x : 0;
    sourceY = (y >= 0) ? y : 0;
    invalidateImageCache();
}

void item_image::setImageMode(item_image_mode value)
{
    if(imageMode == value)
        return;

    imageMode = value;
    invalidateImageCache();
}

void item_image::setRotation(float degrees)
{
    // Keep the value bounded to limit floating-point precision loss.
    while(degrees >= 360.0f)
        degrees -= 360.0f;

    while(degrees <= -360.0f)
        degrees += 360.0f;

    if(rotationDegrees == degrees)
        return;

    rotationDegrees = degrees;
    invalidateImageCache();
}

void item_image::rebuildImageCache()
{
    clearImageCache();

    cacheItemW = getW();
    cacheItemH = getH();
    imageCacheDirty = false;

    if(image == nullptr)
        return;

    bool needsTransform =
        rotationDegrees != 0.0f ||
        imageMode == item_image_mode::STRETCH ||
        imageMode == item_image_mode::FIT ||
        imageMode == item_image_mode::FILL;

    // ORIGINAL and CENTER can be rendered directly from fxconv assets.
    if(!needsTransform)
        return;

    int availableW =
        static_cast<int>(image->width) - sourceX;

    int availableH =
        static_cast<int>(image->height) - sourceY;

    if(availableW <= 0 || availableH <= 0)
        return;

    int targetFormat =
        IMAGE_IS_ALPHA(image->format)
            ? IMAGE_RGB565A
            : IMAGE_RGB565;

    image_t* fullCopy =
        image_copy_alloc(
            reinterpret_cast<image_t const*>(image),
            targetFormat
        );

    if(fullCopy == nullptr || !image_valid(fullCopy))
    {
        if(fullCopy != nullptr)
            image_free(fullCopy);

        return;
    }

    image_t* working = fullCopy;

    if(sourceX != 0 || sourceY != 0)
    {
        image_t subImage;

        image_t* sub =
            image_sub(
                fullCopy,
                sourceX,
                sourceY,
                availableW,
                availableH,
                &subImage
            );

        image_t* cropped =
            image_copy_alloc(
                sub,
                targetFormat
            );

        image_free(fullCopy);
        working = cropped;

        if(working == nullptr || !image_valid(working))
        {
            if(working != nullptr)
                image_free(working);

            return;
        }
    }

    // Rotate before applying the layout scaling. This means FIT/FILL/STRETCH
    // describe the final rotated image rather than the unrotated source.
    if(rotationDegrees != 0.0f)
    {
        struct image_linear_map map;

        const float pi = 3.14159265358979323846f;
        float radians =
            rotationDegrees * pi / 180.0f;

        image_rotate(
            working,
            radians,
            true,
            &map
        );

        image_t* rotated =
            image_linear_alloc(
                working,
                &map
            );

        image_free(working);
        working = rotated;

        if(working == nullptr || !image_valid(working))
        {
            if(working != nullptr)
                image_free(working);

            return;
        }
    }

    bool needsScaling =
        imageMode == item_image_mode::STRETCH ||
        imageMode == item_image_mode::FIT ||
        imageMode == item_image_mode::FILL;

    if(needsScaling)
    {
        if(
            getW() <= 0 ||
            getH() <= 0 ||
            working->width == 0 ||
            working->height == 0)
        {
            image_free(working);
            return;
        }

        double scaleX =
            static_cast<double>(getW()) /
            static_cast<double>(working->width);

        double scaleY =
            static_cast<double>(getH()) /
            static_cast<double>(working->height);

        if(imageMode == item_image_mode::FIT)
        {
            double scale =
                (scaleX < scaleY)
                    ? scaleX
                    : scaleY;

            scaleX = scale;
            scaleY = scale;
        }
        else if(imageMode == item_image_mode::FILL)
        {
            double scale =
                (scaleX > scaleY)
                    ? scaleX
                    : scaleY;

            scaleX = scale;
            scaleY = scale;
        }

        int gammaX =
            static_cast<int>(
                scaleX * 65536.0
            );

        int gammaY =
            static_cast<int>(
                scaleY * 65536.0
            );

        if(gammaX < 1) gammaX = 1;
        if(gammaY < 1) gammaY = 1;

        struct image_linear_map map;

        image_scale(
            working,
            gammaX,
            gammaY,
            &map
        );

        image_t* scaled =
            image_linear_alloc(
                working,
                &map
            );

        image_free(working);
        working = scaled;

        if(working == nullptr || !image_valid(working))
        {
            if(working != nullptr)
                image_free(working);

            return;
        }
    }

    transformedImage = working;
}

void item_image::drawImageWithMode()
{
    if(image == nullptr)
        return;

    if(
        cacheItemW != getW() ||
        cacheItemH != getH())
    {
        imageCacheDirty = true;
    }

    if(imageCacheDirty)
        rebuildImageCache();

    int effects = 0;

    if(mirrorX)
        effects |= IMAGE_HFLIP;

    if(mirrorY)
        effects |= IMAGE_VFLIP;

    int drawX = getX();
    int drawY = getY();

    image_t const* source = nullptr;
    int left = 0;
    int top = 0;
    int sourceW = 0;
    int sourceH = 0;

    if(transformedImage != nullptr)
    {
        source = transformedImage;
        sourceW = transformedImage->width;
        sourceH = transformedImage->height;
    }
    else
    {
        source =
            reinterpret_cast<image_t const*>(image);

        left = sourceX;
        top = sourceY;

        sourceW =
            static_cast<int>(image->width) - sourceX;

        sourceH =
            static_cast<int>(image->height) - sourceY;
    }

    if(
        source == nullptr ||
        sourceW <= 0 ||
        sourceH <= 0)
    {
        return;
    }

    if(
        imageMode == item_image_mode::CENTER ||
        imageMode == item_image_mode::FIT ||
        imageMode == item_image_mode::FILL)
    {
        drawX =
            getX() +
            (getW() - sourceW) / 2;

        drawY =
            getY() +
            (getH() - sourceH) / 2;
    }

    // The item rectangle is the clipping viewport. This gives FILL its
    // center-crop behavior and also prevents ORIGINAL/CENTER from drawing
    // outside the control.
    struct dwindow imageWindow = {
        getX(),
        getY(),
        getX() + getW(),
        getY() + getH()
    };

    struct dwindow oldWindow =
        dwindow_set(imageWindow);

    if(effects == 0)
    {
        dsubimage(
            drawX,
            drawY,
            source,
            left,
            top,
            sourceW,
            sourceH,
            DIMAGE_NONE
        );
    }
    else if(
        source->format == IMAGE_RGB565 ||
        source->format == IMAGE_RGB565A)
    {
        dsubimage_rgb16(
            drawX,
            drawY,
            source,
            left,
            top,
            sourceW,
            sourceH,
            effects
        );
    }
    else if(
        source->format == IMAGE_P8_RGB565 ||
        source->format == IMAGE_P8_RGB565A)
    {
        dsubimage_p8(
            drawX,
            drawY,
            source,
            left,
            top,
            sourceW,
            sourceH,
            effects
        );
    }
    else
    {
        dsubimage_p4(
            drawX,
            drawY,
            source,
            left,
            top,
            sourceW,
            sourceH,
            effects
        );
    }

    dwindow_set(oldWindow);
}

void item_image::draw()
{
    if(!isVisible())
        return;

    drawImageWithMode();

    if(drawBorder)
    {
        drawOutlineRect(
            getX(),
            getY(),
            getX() + getW() - 1,
            getY() + getH() - 1,
            borderColor.getRGB(),
            1
        );
    }
}

item_image_button::item_image_button(
    STRUCT_pos _pos,
    bopti_image_t const* _image,
    std::function<void()> _callback,
    int _sourceX,
    int _sourceY,
    bool _drawBorder,
    int _zOrder,
    item_image_mode _mode)
    : item_image(
        _pos,
        _image,
        _sourceX,
        _sourceY,
        _drawBorder,
        Class_color(0, 0, 0),
        _zOrder,
        _mode)
{
    callback = _callback;
}

void item_image_button::handleEvent(
    int eventType,
    int eventKey)
{
    if(
        eventType == KEYEV_DOWN &&
        eventKey == KEY_EXE &&
        param.status.hover &&
        !param.status.dimmed &&
        param.status.visible &&
        callback)
    {
        callback();
    }
}

void item_image_button::draw()
{
    item_image::draw();

    if(
        isVisible() &&
        param.status.hover)
    {
        drawOutlineRect(
            getX(),
            getY(),
            getX() + getW() - 1,
            getY() + getH() - 1,
            current_item_theme().accent.getRGB(),
            current_item_theme().focusBorderSize
        );
    }
}


//******************************** Invisible time item *********************************

item_time::item_time(
    uint32_t _durationMs,
    std::function<void()> _callback,
    bool _repeat,
    bool autoStart)
{
    durationMs = (_durationMs == 0) ? 1 : _durationMs;
    callback = _callback;
    repeat = _repeat;

    setVisible(false);
    setSize(0, 0);

    if(autoStart)
        start();
}

uint32_t item_time::tickDelta(
    uint32_t nowTicks,
    uint32_t previousTicks)
{
    constexpr uint32_t RTC_TICKS_PER_DAY = 128u * 60u * 60u * 24u;

    if(nowTicks >= previousTicks)
        return nowTicks - previousTicks;

    return
        (RTC_TICKS_PER_DAY - previousTicks) +
        nowTicks;
}

uint64_t item_time::durationTicks() const
{
    return
        (static_cast<uint64_t>(durationMs) * 128ull + 999ull) /
        1000ull;
}

void item_time::start()
{
    lastTicks = rtc_ticks();
    running = true;
    paused = false;
}

void item_time::pause()
{
    if(!running)
        return;

    uint32_t now = rtc_ticks();
    elapsedTicks += tickDelta(now, lastTicks);
    lastTicks = now;

    running = false;
    paused = true;
}

void item_time::stop()
{
    running = false;
    paused = false;
}

void item_time::reset()
{
    elapsedTicks = 0;
    lastTicks = rtc_ticks();
}

void item_time::restart()
{
    reset();
    start();
}

void item_time::setDurationMs(uint32_t value)
{
    durationMs = (value == 0) ? 1 : value;
}

uint64_t item_time::getElapsedMs() const
{
    uint64_t ticks = elapsedTicks;

    if(running)
        ticks += tickDelta(rtc_ticks(), lastTicks);

    return (ticks * 1000ull) / 128ull;
}

uint64_t item_time::getRemainingMs() const
{
    uint64_t elapsed = getElapsedMs();

    if(elapsed >= durationMs)
        return 0;

    return static_cast<uint64_t>(durationMs) - elapsed;
}

void item_time::update(uint32_t nowTicks)
{
    if(!running)
        return;

    elapsedTicks += tickDelta(nowTicks, lastTicks);
    lastTicks = nowTicks;

    if(updateCallback)
        updateCallback((elapsedTicks * 1000ull) / 128ull);

    uint64_t targetTicks = durationTicks();

    if(targetTicks == 0 || elapsedTicks < targetTicks)
        return;

    if(callback)
        callback();

    if(repeat && targetTicks > 0)
    {
        elapsedTicks %= targetTicks;
    }
    else
    {
        elapsedTicks = targetTicks;
        running = false;
        paused = false;
    }
}


//******************************** Popup *********************************

item_popup::item_popup(
    std::string _title,
    std::string _message,
    int width,
    int height,
    bool _opened,
    int _zOrder)
{
    param.pos = STRUCT_pos{0, 0, width, height};
    title = _title;
    message = _message;
    opened = _opened;
    zOrder = _zOrder;

    setGlobalPage();
    centerPopup();
    setVisible(opened);
}

void item_popup::centerPopup()
{
    if(param.pos.w < 80)
        param.pos.w = 80;

    if(param.pos.h < 60)
        param.pos.h = 60;

    if(param.pos.w > DWIDTH - 12)
        param.pos.w = DWIDTH - 12;

    if(param.pos.h > DHEIGHT - 12)
        param.pos.h = DHEIGHT - 12;

    param.pos.x = (DWIDTH - param.pos.w) / 2;
    param.pos.y = (DHEIGHT - param.pos.h) / 2;
}

void item_popup::open()
{
    opened = true;
    setVisible(true);
    clearPointerState();
}

void item_popup::close()
{
    if(!opened)
        return;

    opened = false;
    setVisible(false);
    clearPointerState();

    if(onClose)
        onClose();
}

void item_popup::toggle()
{
    if(opened)
        close();
    else
        open();
}

bool item_popup::contains(int x, int y) const
{
    if(!opened || !isVisible())
        return false;

    // Modal popup: it owns the whole pointer surface while it is open.
    return x >= 0 && x < DWIDTH && y >= 0 && y < DHEIGHT;
}

bool item_popup::isHover(int x, int y)
{
    pointerX = x;
    pointerY = y;
    param.status.hover = opened && isVisible();
    return param.status.hover;
}

bool item_popup::pointerOnCloseButton() const
{
    int buttonX1 = getX() + getW() - closeButtonSize - 3;
    int buttonY1 = getY() + 3;
    int buttonX2 = buttonX1 + closeButtonSize - 1;
    int buttonY2 = buttonY1 + closeButtonSize - 1;

    return
        pointerX >= buttonX1 && pointerX <= buttonX2 &&
        pointerY >= buttonY1 && pointerY <= buttonY2;
}

void item_popup::handleEvent(
    int eventType,
    int eventKey)
{
    if(!opened || eventType != KEYEV_DOWN)
        return;

    if(eventKey == KEY_EXIT)
    {
        close();
        return;
    }

    if(eventKey == KEY_EXE && pointerOnCloseButton())
        close();
}

bool item_popup::handleSystemExit()
{
    if(!opened)
        return false;

    close();
    return true;
}

void item_popup::draw()
{
    if(!opened || !isVisible())
        return;

    int x1 = getX();
    int y1 = getY();
    int x2 = getX() + getW() - 1;
    int y2 = getY() + getH() - 1;

    drect(x1, y1, x2, y2, C_WHITE);
    drawOutlineRect(x1, y1, x2, y2, C_BLACK, 2);

    drect(
        x1 + 2,
        y1 + 2,
        x2 - 2,
        y1 + titleHeight,
        C_LIGHT
    );

    dline(
        x1 + 2,
        y1 + titleHeight + 1,
        x2 - 2,
        y1 + titleHeight + 1,
        C_BLACK
    );

    dtext(x1 + 8, y1 + 7, C_BLACK, title.c_str());

    int closeX1 = x2 - closeButtonSize - 2;
    int closeY1 = y1 + 3;
    int closeX2 = x2 - 3;
    int closeY2 = closeY1 + closeButtonSize - 1;

    drawOutlineRect(
        closeX1,
        closeY1,
        closeX2,
        closeY2,
        pointerOnCloseButton() ? C_RED : C_BLACK,
        1
    );

    dline(closeX1 + 4, closeY1 + 4, closeX2 - 4, closeY2 - 4, C_BLACK);
    dline(closeX2 - 4, closeY1 + 4, closeX1 + 4, closeY2 - 4, C_BLACK);

    int textY = y1 + titleHeight + 10;
    size_t start = 0;

    while(start <= message.size() && textY < y2 - 12)
    {
        size_t end = message.find('\n', start);
        std::string line =
            (end == std::string::npos)
            ? message.substr(start)
            : message.substr(start, end - start);

        dtext(x1 + 10, textY, C_BLACK, line.c_str());
        textY += 14;

        if(end == std::string::npos)
            break;

        start = end + 1;
    }
}


//******************************** Prompt / bool / numeric popups *********************************

static char popupDefaultKeyToChar(int key)
{
    switch(key)
    {
        case KEY_0: return '0';
        case KEY_1: return '1';
        case KEY_2: return '2';
        case KEY_3: return '3';
        case KEY_4: return '4';
        case KEY_5: return '5';
        case KEY_6: return '6';
        case KEY_7: return '7';
        case KEY_8: return '8';
        case KEY_9: return '9';

        case KEY_DOT:    return '.';
        case KEY_ADD:    return '+';
        case KEY_SUB:    return '-';
        case KEY_NEG:    return '-';
        case KEY_MUL:    return '*';
        case KEY_DIV:    return '/';
        case KEY_LEFTP:  return '(';
        case KEY_RIGHTP: return ')';
        case KEY_COMMA:  return ',';

        default:
            return 0;
    }
}

item_prompt_popup::item_prompt_popup(
    std::string title,
    std::string message,
    std::string _value,
    unsigned int _maxLength,
    std::function<void(const std::string&)> callback,
    bool opened,
    int zOrder)
    : item_popup(
        title,
        message,
        (DWIDTH * 2) / 3,
        (DHEIGHT * 2) / 3,
        opened,
        zOrder)
{
    maxLength = (_maxLength > 0) ? _maxLength : 1;
    value = _value;

    if(value.size() > maxLength)
        value.resize(maxLength);

    onSubmit = callback;
}

char item_prompt_popup::defaultKeyToChar(int key)
{
    item_text_input_state state;
    return item_text_key_to_char(
        key,
        state
    );
}

void item_prompt_popup::setValue(const std::string& text)
{
    value = text;

    if(value.size() > maxLength)
        value.resize(maxLength);
}

void item_prompt_popup::submit()
{
    if(onSubmit)
        onSubmit(value);

    close();
}

void item_prompt_popup::handleEvent(
    int eventType,
    int eventKey)
{
    bool wasOpen = isOpen();

    item_popup::handleEvent(
        eventType,
        eventKey
    );

    if(!wasOpen || !isOpen() || eventType != KEYEV_DOWN)
        return;

    if(eventKey == KEY_EXE)
    {
        submit();
        inputState.reset();
        return;
    }

    if(item_text_handle_modifier(
        eventKey,
        inputState))
    {
        return;
    }

    if(eventKey == KEY_DEL)
    {
        if(!value.empty())
            value.pop_back();

        return;
    }

    if(eventKey == KEY_ACON)
    {
        value.clear();
        inputState.reset();
        return;
    }

    char c = 0;

    if(extendedKeyMapper)
    {
        c =
            extendedKeyMapper(
                eventKey,
                inputState
            );
    }

    if(c == 0 && keyMapper)
        c = keyMapper(eventKey);

    if(c == 0)
    {
        c =
            item_text_key_to_char(
                eventKey,
                inputState
            );
    }

    if(c != 0 && value.size() < maxLength)
    {
        value += c;
        item_text_consume_modifier(
            inputState
        );
    }
}

void item_prompt_popup::draw()
{
    if(!isOpen())
        return;

    item_popup::draw();

    const item_theme& theme =
        current_item_theme();

    int x1 = getX() + 12;
    int x2 = getX() + getW() - 13;
    int y2 = getY() + getH() - 16;
    int y1 = y2 - 28;

    drect(
        x1,
        y1,
        x2,
        y2,
        theme.surface.getRGB()
    );

    drawOutlineRect(
        x1,
        y1,
        x2,
        y2,
        theme.accent.getRGB(),
        theme.focusBorderSize
    );

    dtext(
        x1 + 6,
        y1 + 7,
        theme.text.getRGB(),
        value.empty()
            ? "_"
            : value.c_str()
    );

    dtext(
        x1,
        y1 - 14,
        theme.textMuted.getRGB(),
        "ALPHA text  SHIFT symbols  EXE OK"
    );

    const char* mode =
        item_text_modifier_label(
            inputState
        );

    if(mode[0] != '\0')
    {
        dtext(
            x2 - 24,
            y1 + 7,
            theme.accent.getRGB(),
            mode
        );
    }
}


item_bool_popup::item_bool_popup(
    std::string title,
    std::string message,
    std::vector<std::string> _buttons,
    std::function<void(int, const std::string&)> callback,
    bool opened,
    int zOrder)
    : item_popup(
        title,
        message,
        (DWIDTH * 2) / 3,
        (DHEIGHT * 2) / 3,
        opened,
        zOrder)
{
    onChoice = callback;
    setButtons(_buttons);
}

void item_bool_popup::setButtons(
    const std::vector<std::string>& value)
{
    buttons = value;

    if(buttons.empty())
    {
        buttons.push_back("Yes");
        buttons.push_back("No");
    }

    if(selectedIndex >= static_cast<int>(buttons.size()))
        selectedIndex = static_cast<int>(buttons.size()) - 1;

    if(selectedIndex < 0)
        selectedIndex = 0;
}

void item_bool_popup::setSelectedIndex(int index)
{
    if(buttons.empty())
    {
        selectedIndex = 0;
        return;
    }

    if(index < 0)
        index = 0;

    if(index >= static_cast<int>(buttons.size()))
        index = static_cast<int>(buttons.size()) - 1;

    selectedIndex = index;
}

int item_bool_popup::pointerButtonIndex() const
{
    if(buttons.empty())
        return -1;

    int left = getX() + 10;
    int right = getX() + getW() - 11;
    int top = getY() + getH() - 42;
    int bottom = getY() + getH() - 14;

    if(
        getPointerX() < left ||
        getPointerX() > right ||
        getPointerY() < top ||
        getPointerY() > bottom)
    {
        return -1;
    }

    int totalWidth = right - left + 1;
    int count = static_cast<int>(buttons.size());
    int index =
        (getPointerX() - left) * count /
        ((totalWidth > 0) ? totalWidth : 1);

    if(index < 0)
        index = 0;

    if(index >= count)
        index = count - 1;

    return index;
}

void item_bool_popup::choose(int index)
{
    if(
        index < 0 ||
        index >= static_cast<int>(buttons.size()))
    {
        return;
    }

    selectedIndex = index;

    if(onChoice)
        onChoice(index, buttons[index]);

    if(onBool && buttons.size() == 2)
        onBool(index == 0);

    close();
}

void item_bool_popup::handleEvent(
    int eventType,
    int eventKey)
{
    bool wasOpen = isOpen();

    item_popup::handleEvent(
        eventType,
        eventKey
    );

    if(!wasOpen || !isOpen() || eventType != KEYEV_DOWN)
        return;

    if(eventKey == KEY_LEFT)
    {
        if(selectedIndex > 0)
            --selectedIndex;

        return;
    }

    if(eventKey == KEY_RIGHT)
    {
        if(selectedIndex + 1 < static_cast<int>(buttons.size()))
            ++selectedIndex;

        return;
    }

    if(eventKey == KEY_EXE)
    {
        int pointerIndex = pointerButtonIndex();

        if(pointerIndex >= 0)
            selectedIndex = pointerIndex;

        choose(selectedIndex);
    }
}

void item_bool_popup::draw()
{
    if(!isOpen())
        return;

    item_popup::draw();

    if(buttons.empty())
        return;

    int left = getX() + 10;
    int right = getX() + getW() - 11;
    int top = getY() + getH() - 42;
    int bottom = getY() + getH() - 14;

    int count = static_cast<int>(buttons.size());
    int totalWidth = right - left + 1;

    int pointerIndex = pointerButtonIndex();

    for(int i = 0; i < count; ++i)
    {
        int x1 = left + totalWidth * i / count;
        int x2 = left + totalWidth * (i + 1) / count - 2;

        bool selected =
            i == selectedIndex ||
            i == pointerIndex;

        drect(
            x1,
            top,
            x2,
            bottom,
            selected ? C_LIGHT : C_WHITE
        );

        drawOutlineRect(
            x1,
            top,
            x2,
            bottom,
            selected ? C_BLUE : C_BLACK,
            selected ? 2 : 1
        );

        dtext(
            x1 + 5,
            top + 7,
            C_BLACK,
            buttons[i].c_str()
        );
    }
}


item_numeric_popup::item_numeric_popup(
    std::string title,
    std::string message,
    double _value,
    double _minValue,
    double _maxValue,
    int _precision,
    double _step,
    std::function<void(double)> callback,
    bool opened,
    int zOrder)
    : item_popup(
        title,
        message,
        (DWIDTH * 2) / 3,
        (DHEIGHT * 2) / 3,
        opened,
        zOrder)
{
    setRange(_minValue, _maxValue);
    setPrecision(_precision);
    step = (_step > 0.0) ? _step : 1.0;
    onSubmit = callback;
    setValue(_value);
}

void item_numeric_popup::setRange(
    double _minValue,
    double _maxValue)
{
    minValue = _minValue;
    maxValue = _maxValue;

    if(maxValue < minValue)
    {
        double swap = minValue;
        minValue = maxValue;
        maxValue = swap;
    }

    if(value < minValue)
        value = minValue;

    if(value > maxValue)
        value = maxValue;

    syncBuffer();
}

void item_numeric_popup::setPrecision(int value)
{
    if(value < 0)
        value = 0;

    if(value > 6)
        value = 6;

    precision = value;
    syncBuffer();
}

void item_numeric_popup::syncBuffer()
{
    char text[48];
    formatGraphNumber(
        value,
        precision > 4 ? 4 : precision,
        text,
        sizeof(text)
    );

    buffer = text;
}

void item_numeric_popup::setValue(double newValue)
{
    if(newValue < minValue)
        newValue = minValue;

    if(newValue > maxValue)
        newValue = maxValue;

    value = newValue;
    syncBuffer();
}

bool item_numeric_popup::parseBuffer(double& output) const
{
    if(buffer.empty() || buffer == "-" || buffer == ".")
        return false;

    bool negative = false;
    size_t index = 0;

    if(buffer[0] == '-')
    {
        negative = true;
        index = 1;
    }

    double whole = 0.0;
    double fraction = 0.0;
    double factor = 0.1;
    bool decimal = false;
    bool digitFound = false;

    for(; index < buffer.size(); ++index)
    {
        char c = buffer[index];

        if(c == '.')
        {
            if(decimal)
                return false;

            decimal = true;
            continue;
        }

        if(c < '0' || c > '9')
            return false;

        digitFound = true;
        int digit = c - '0';

        if(!decimal)
        {
            whole = whole * 10.0 + digit;
        }
        else
        {
            fraction += static_cast<double>(digit) * factor;
            factor *= 0.1;
        }
    }

    if(!digitFound)
        return false;

    output = whole + fraction;

    if(negative)
        output = -output;

    return true;
}

void item_numeric_popup::submit()
{
    double parsed = value;

    if(parseBuffer(parsed))
        setValue(parsed);

    if(onSubmit)
        onSubmit(value);

    close();
}

void item_numeric_popup::handleEvent(
    int eventType,
    int eventKey)
{
    bool wasOpen = isOpen();

    item_popup::handleEvent(
        eventType,
        eventKey
    );

    if(
        !wasOpen ||
        !isOpen() ||
        (eventType != KEYEV_DOWN && eventType != KEYEV_HOLD))
    {
        return;
    }

    if(eventType == KEYEV_DOWN && eventKey == KEY_EXE)
    {
        submit();
        return;
    }

    if(eventType == KEYEV_DOWN && eventKey == KEY_DEL)
    {
        if(!buffer.empty())
            buffer.pop_back();

        return;
    }

    if(eventKey == KEY_UP)
    {
        setValue(value + step);
        return;
    }

    if(eventKey == KEY_DOWN)
    {
        setValue(value - step);
        return;
    }

    if(eventType != KEYEV_DOWN)
        return;

    if(eventKey == KEY_NEG || eventKey == KEY_SUB)
    {
        if(!buffer.empty() && buffer[0] == '-')
            buffer.erase(0, 1);
        else
            buffer.insert(buffer.begin(), '-');

        return;
    }

    if(eventKey == KEY_DOT)
    {
        if(buffer.find('.') == std::string::npos)
            buffer += '.';

        return;
    }

    char c = popupDefaultKeyToChar(eventKey);

    if(c >= '0' && c <= '9' && buffer.size() < 24)
        buffer += c;
}

void item_numeric_popup::draw()
{
    if(!isOpen())
        return;

    item_popup::draw();

    int x1 = getX() + 12;
    int x2 = getX() + getW() - 13;
    int y2 = getY() + getH() - 16;
    int y1 = y2 - 28;

    drect(x1, y1, x2, y2, C_WHITE);
    drawOutlineRect(x1, y1, x2, y2, C_BLUE, 2);

    dtext(
        x1 + 6,
        y1 + 7,
        C_BLACK,
        buffer.empty() ? "0" : buffer.c_str()
    );

    dtext(
        x1,
        y1 - 14,
        C_DARK,
        "UP/DOWN: step  EXE: OK"
    );
}



//******************************** Options popup *********************************

item_options_popup::item_options_popup(
    std::string title,
    std::string message,
    bool opened,
    int zOrder)
    : item_popup(
        title,
        message,
        (DWIDTH * 2) / 3,
        (DHEIGHT * 2) / 3,
        opened,
        zOrder)
{
    if(opened)
        captureSnapshot();
}

void item_options_popup::captureSnapshot()
{
    snapshotValues.clear();

    for(const option_popup_entry& option : options)
        snapshotValues.push_back(option.value);
}

void item_options_popup::restoreSnapshot()
{
    if(snapshotValues.size() != options.size())
        return;

    for(unsigned int i = 0; i < options.size(); ++i)
        options[i].value = snapshotValues[i];
}

void item_options_popup::open()
{
    captureSnapshot();
    applied = false;
    item_popup::open();
}

int item_options_popup::addCheckbox(const std::string& label, bool value)
{
    int id = nextOptionId++;

    options.emplace_back(
        id,
        option_popup_type::CHECKBOX,
        label,
        value,
        0
    );

    return id;
}

int item_options_popup::addToggle(const std::string& label, bool value)
{
    int id = nextOptionId++;

    options.emplace_back(
        id,
        option_popup_type::TOGGLE,
        label,
        value,
        0
    );

    return id;
}

int item_options_popup::addRadio(
    const std::string& label,
    int radioGroup,
    bool selected)
{
    int id = nextOptionId++;

    if(selected)
    {
        for(option_popup_entry& option : options)
        {
            if(
                option.type == option_popup_type::RADIO &&
                option.radioGroup == radioGroup)
            {
                option.value = false;
            }
        }
    }

    options.emplace_back(
        id,
        option_popup_type::RADIO,
        label,
        selected,
        radioGroup
    );

    return id;
}

int item_options_popup::optionIndexById(int id) const
{
    for(unsigned int i = 0; i < options.size(); ++i)
    {
        if(options[i].id == id)
            return static_cast<int>(i);
    }

    return -1;
}

bool item_options_popup::removeOption(int id)
{
    int index = optionIndexById(id);

    if(index < 0)
        return false;

    options.erase(options.begin() + index);

    if(selectedIndex >= static_cast<int>(options.size()))
        selectedIndex = static_cast<int>(options.size()) - 1;

    if(selectedIndex < 0)
        selectedIndex = 0;

    return true;
}

void item_options_popup::clearOptions()
{
    options.clear();
    snapshotValues.clear();
    selectedIndex = 0;
    scrollOffset = 0;
    nextOptionId = 1;
}

bool item_options_popup::hasOption(int id) const
{
    return optionIndexById(id) >= 0;
}

bool item_options_popup::getValue(int id) const
{
    int index = optionIndexById(id);

    if(index < 0)
        return false;

    return options[index].value;
}

bool item_options_popup::setValue(int id, bool value)
{
    int index = optionIndexById(id);

    if(index < 0)
        return false;

    if(options[index].type == option_popup_type::RADIO)
    {
        if(value)
            selectRadio(index);
        else
            options[index].value = false;
    }
    else
    {
        options[index].value = value;
    }

    return true;
}

int item_options_popup::getSelectedRadio(int radioGroup) const
{
    for(const option_popup_entry& option : options)
    {
        if(
            option.type == option_popup_type::RADIO &&
            option.radioGroup == radioGroup &&
            option.value)
        {
            return option.id;
        }
    }

    return -1;
}

void item_options_popup::selectRadio(int index)
{
    if(index < 0 || index >= static_cast<int>(options.size()))
        return;

    option_popup_entry& selected = options[index];

    if(selected.type != option_popup_type::RADIO)
        return;

    for(option_popup_entry& option : options)
    {
        if(
            option.type == option_popup_type::RADIO &&
            option.radioGroup == selected.radioGroup)
        {
            option.value = false;
        }
    }

    selected.value = true;
}

void item_options_popup::toggleOption(int index)
{
    if(index < 0 || index >= static_cast<int>(options.size()))
        return;

    if(options[index].type == option_popup_type::RADIO)
        selectRadio(index);
    else
        options[index].value = !options[index].value;
}

int item_options_popup::pointerOptionIndex() const
{
    int left = getX() + 12;
    int right = getX() + getW() - 13;
    int top = getY() + 58;
    int bottom = getY() + getH() - 56;

    if(
        getPointerX() < left ||
        getPointerX() > right ||
        getPointerY() < top ||
        getPointerY() > bottom)
    {
        return -1;
    }

    int row = (getPointerY() - top) / rowHeight;
    int index = scrollOffset + row;

    if(index < 0 || index >= static_cast<int>(options.size()))
        return -1;

    return index;
}

int item_options_popup::pointerButtonIndex() const
{
    int left = getX() + 12;
    int right = getX() + getW() - 13;
    int top = getY() + getH() - 42;
    int bottom = getY() + getH() - 14;

    if(
        getPointerX() < left ||
        getPointerX() > right ||
        getPointerY() < top ||
        getPointerY() > bottom)
    {
        return -1;
    }

    int middle = (left + right) / 2;

    return (getPointerX() <= middle) ? 0 : 1;
}

void item_options_popup::apply()
{
    applied = true;

    if(onApply)
        onApply(*this);

    item_popup::close();
}

void item_options_popup::cancel()
{
    restoreSnapshot();
    applied = false;

    if(onCancel)
        onCancel();

    item_popup::close();
}

bool item_options_popup::handleSystemExit()
{
    if(!isOpen())
        return false;

    cancel();
    return true;
}

void item_options_popup::handleEvent(int eventType, int eventKey)
{
    if(!isOpen())
        return;

    if(eventType == KEYEV_DOWN && eventKey == KEY_EXIT)
    {
        cancel();
        return;
    }

    bool wasOpen = isOpen();

    item_popup::handleEvent(eventType, eventKey);

    if(wasOpen && !isOpen())
    {
        if(!applied)
        {
            restoreSnapshot();

            if(onCancel)
                onCancel();
        }

        return;
    }

    if(
        !isOpen() ||
        (eventType != KEYEV_DOWN && eventType != KEYEV_HOLD))
    {
        return;
    }

    if(eventType == KEYEV_DOWN && eventKey == KEY_F1)
    {
        apply();
        return;
    }

    if(eventType == KEYEV_DOWN && eventKey == KEY_F2)
    {
        cancel();
        return;
    }

    if(options.empty())
        return;

    if(eventKey == KEY_UP)
    {
        --selectedIndex;

        if(selectedIndex < 0)
            selectedIndex = 0;
    }

    if(eventKey == KEY_DOWN)
    {
        ++selectedIndex;

        if(selectedIndex >= static_cast<int>(options.size()))
            selectedIndex = static_cast<int>(options.size()) - 1;
    }

    int visibleRows = (getH() - 114) / rowHeight;

    if(visibleRows < 1)
        visibleRows = 1;

    if(selectedIndex < scrollOffset)
        scrollOffset = selectedIndex;

    if(selectedIndex >= scrollOffset + visibleRows)
        scrollOffset = selectedIndex - visibleRows + 1;

    int maxOffset = static_cast<int>(options.size()) - visibleRows;

    if(maxOffset < 0)
        maxOffset = 0;

    if(scrollOffset > maxOffset)
        scrollOffset = maxOffset;

    if(eventType == KEYEV_DOWN && eventKey == KEY_EXE)
    {
        int button = pointerButtonIndex();

        if(button == 0)
        {
            apply();
            return;
        }

        if(button == 1)
        {
            cancel();
            return;
        }

        int pointerIndex = pointerOptionIndex();

        if(pointerIndex >= 0)
            selectedIndex = pointerIndex;

        toggleOption(selectedIndex);
    }
}

void item_options_popup::draw()
{
    if(!isOpen())
        return;

    item_popup::draw();

    int left = getX() + 12;
    int right = getX() + getW() - 13;
    int top = getY() + 58;
    int bottom = getY() + getH() - 56;

    int visibleRows = (bottom - top + 1) / rowHeight;

    if(visibleRows < 1)
        visibleRows = 1;

    for(int row = 0; row < visibleRows; ++row)
    {
        int index = scrollOffset + row;

        if(index >= static_cast<int>(options.size()))
            break;

        option_popup_entry& option = options[index];
        int y = top + row * rowHeight;

        if(index == selectedIndex)
        {
            drect(
                left,
                y,
                right,
                y + rowHeight - 1,
                C_LIGHT
            );
        }

        int markerX = left + 10;
        int markerY = y + rowHeight / 2;

        if(option.type == option_popup_type::RADIO)
        {
            drawCircleFilled(markerX, markerY, 7, C_WHITE);
            drawCircleOutline(markerX, markerY, 7, C_BLACK);

            if(option.value)
                drawCircleFilled(markerX, markerY, 3, C_BLACK);
        }
        else
        {
            drect(
                markerX - 7,
                markerY - 7,
                markerX + 7,
                markerY + 7,
                option.value ? C_BLACK : C_WHITE
            );

            drawOutlineRect(
                markerX - 7,
                markerY - 7,
                markerX + 7,
                markerY + 7,
                C_BLACK,
                1
            );

            if(option.value)
            {
                dline(
                    markerX - 4,
                    markerY,
                    markerX - 1,
                    markerY + 4,
                    C_WHITE
                );

                dline(
                    markerX - 1,
                    markerY + 4,
                    markerX + 5,
                    markerY - 4,
                    C_WHITE
                );
            }
        }

        dtext(
            left + 24,
            y + (rowHeight - 12) / 2,
            C_BLACK,
            option.label.c_str()
        );
    }

    int buttonTop = getY() + getH() - 42;
    int buttonBottom = getY() + getH() - 14;
    int middle = (left + right) / 2;

    drect(left, buttonTop, middle - 2, buttonBottom, C_WHITE);
    drect(middle + 2, buttonTop, right, buttonBottom, C_WHITE);

    drawOutlineRect(
        left,
        buttonTop,
        middle - 2,
        buttonBottom,
        C_BLUE,
        1
    );

    drawOutlineRect(
        middle + 2,
        buttonTop,
        right,
        buttonBottom,
        C_BLACK,
        1
    );

    dtext(
        left + 8,
        buttonTop + 7,
        C_BLACK,
        applyLabel.c_str()
    );

    dtext(
        middle + 10,
        buttonTop + 7,
        C_BLACK,
        cancelLabel.c_str()
    );

    dtext(
        left,
        buttonTop - 14,
        C_DARK,
        "F1: Apply   F2: Cancel"
    );
}


//******************************** List classe *********************************
int liste_item::addItem(item* i)
{
    if(i == nullptr)
        return -1;

    int newId = nextId;

    while(listItem.find(newId) != listItem.end())
        ++newId;

    nextId = newId + 1;

    i->setId(newId);

    if(i->getPageId() == ITEM_PAGE_AUTO)
        i->setPageId(activePageId);

    listItem[newId] = i;

    return newId;
}

void liste_item::removeItem(item* i)
{
    if(i == nullptr)
        return;

    auto it = listItem.find(i->getId());

    if(it != listItem.end() && it->second == i)
    {
        if(focusedItemId == it->first)
            focusedItemId = -1;

        listItem.erase(it);
        return;
    }

    // Fallback in case the item's ID was changed manually.
    for(auto itItem = listItem.begin(); itItem != listItem.end(); ++itItem)
    {
        if(itItem->second == i)
        {
            if(focusedItemId == itItem->first)
                focusedItemId = -1;

            listItem.erase(itItem);
            return;
        }
    }
}

bool liste_item::removeItemById(int id)
{
    auto it = listItem.find(id);

    if(it == listItem.end())
        return false;

    if(focusedItemId == id)
        focusedItemId = -1;

    listItem.erase(it);
    return true;
}

void liste_item::draw()
{
    // Conservative phase-7 ordering path. This is deliberately allocation-free
    // and avoids the phase-8 drawScratch/std::sort regression path.
    int lastZ = INT_MIN;
    int lastId = INT_MIN;

    for(unsigned int drawn = 0; drawn < listItem.size(); ++drawn)
    {
        item* nextItem = nullptr;
        int nextZ = INT_MAX;
        int nextId = INT_MAX;

        for(auto& entry : listItem)
        {
            item* currentItem = entry.second;

            if(currentItem == nullptr || !isItemOnActivePage(currentItem))
                continue;

            int currentZ = currentItem->getZOrder();
            int currentId = entry.first;

            bool afterLast =
                currentZ > lastZ ||
                (currentZ == lastZ && currentId > lastId);

            bool beforeNext =
                currentZ < nextZ ||
                (currentZ == nextZ && currentId < nextId);

            if(afterLast && beforeNext)
            {
                nextItem = currentItem;
                nextZ = currentZ;
                nextId = currentId;
            }
        }

        if(nextItem == nullptr)
            break;

        nextItem->draw();
        lastZ = nextZ;
        lastId = nextId;
    }
}

void liste_item::update(uint32_t nowTicks)
{
    for(auto& entry : listItem)
    {
        item* currentItem = entry.second;

        if(currentItem == nullptr || !isItemOnActivePage(currentItem))
            continue;

        currentItem->update(nowTicks);
    }
}

void liste_item::applyTheme(
    const item_theme& theme)
{
    for(auto& entry : listItem)
    {
        if(entry.second != nullptr)
            entry.second->applyTheme(theme);
    }
}

item* liste_item::getItems(unsigned int idx)
{
    if(idx >= listItem.size())
        return nullptr;

    auto it = listItem.begin();

    for(unsigned int i = 0; i < idx; ++i)
        ++it;

    return it->second;
}

item* liste_item::getItemsById(int id)
{
    auto it = listItem.find(id);

    if(it == listItem.end())
        return nullptr;

    return it->second;
}

item* liste_item::getTopItemAt(int x, int y)
{
    item* topItem = nullptr;
    int topZ = INT_MIN;
    int topId = INT_MIN;

    for(auto& entry : listItem)
    {
        item* currentItem = entry.second;

        if(
            currentItem == nullptr ||
            !isItemOnActivePage(currentItem) ||
            !currentItem->isVisible())
        {
            continue;
        }

        if(!currentItem->contains(x, y))
            continue;

        int currentZ = currentItem->getZOrder();
        int currentId = entry.first;

        if(
            topItem == nullptr ||
            currentZ > topZ ||
            (currentZ == topZ && currentId > topId))
        {
            topItem = currentItem;
            topZ = currentZ;
            topId = currentId;
        }
    }

    return topItem;
}

item* liste_item::getKeyboardCapture()
{
    // Pick the top-most active item that captures the keyboard. This lets
    // modal controls (notably item_popup) override a focused control below.
    item* topItem = nullptr;
    int topZ = INT_MIN;
    int topId = INT_MIN;

    for(auto& entry : listItem)
    {
        item* currentItem = entry.second;

        if(
            currentItem == nullptr ||
            !isItemOnActivePage(currentItem) ||
            !currentItem->isVisible() ||
            !currentItem->capturesKeyboard())
        {
            continue;
        }

        int currentZ = currentItem->getZOrder();
        int currentId = entry.first;

        if(
            topItem == nullptr ||
            currentZ > topZ ||
            (currentZ == topZ && currentId > topId))
        {
            topItem = currentItem;
            topZ = currentZ;
            topId = currentId;
        }
    }

    return topItem;
}

bool liste_item::setZOrderById(int id, int _zOrder)
{
    item* currentItem = getItemsById(id);

    if(currentItem == nullptr)
        return false;

    currentItem->setZOrder(_zOrder);
    return true;
}

bool liste_item::setFocusById(int id)
{
    item* target = getItemsById(id);

    if(
        target == nullptr ||
        !isItemOnActivePage(target) ||
        !target->isVisible() ||
        !target->isFocusable())
    {
        return false;
    }

    if(
        focusedItemId == id &&
        target->hasFocus())
    {
        return true;
    }

    for(auto& entry : listItem)
    {
        if(entry.second != nullptr && entry.second->isFocusable())
            entry.second->setFocus(false);
    }

    target->setFocus(true);
    focusedItemId = id;

    return true;
}

void liste_item::clearFocus()
{
    for(auto& entry : listItem)
    {
        if(entry.second != nullptr && entry.second->isFocusable())
            entry.second->setFocus(false);
    }

    focusedItemId = -1;
}

item* liste_item::getFocusedItem()
{
    if(focusedItemId < 0)
        return nullptr;

    auto it = listItem.find(focusedItemId);

    if(it == listItem.end() || it->second == nullptr)
    {
        focusedItemId = -1;
        return nullptr;
    }

    if(
        !isItemOnActivePage(it->second) ||
        !it->second->isVisible() ||
        !it->second->isFocusable() ||
        !it->second->hasFocus())
    {
        focusedItemId = -1;
        return nullptr;
    }

    return it->second;
}

bool liste_item::focusNext()
{
    if(listItem.empty())
        return false;

    auto it =
        (focusedItemId >= 0)
        ? listItem.upper_bound(focusedItemId)
        : listItem.begin();

    if(it == listItem.end())
        it = listItem.begin();

    for(unsigned int checked = 0; checked < listItem.size(); ++checked)
    {
        if(
            it->second != nullptr &&
            isItemOnActivePage(it->second) &&
            it->second->isVisible() &&
            it->second->isFocusable())
        {
            return setFocusById(it->first);
        }

        ++it;

        if(it == listItem.end())
            it = listItem.begin();
    }

    return false;
}

bool liste_item::focusPrevious()
{
    if(listItem.empty())
        return false;

    auto it = listItem.end();

    if(focusedItemId >= 0)
    {
        it = listItem.lower_bound(focusedItemId);

        if(it == listItem.begin())
            it = listItem.end();
    }

    for(unsigned int checked = 0; checked < listItem.size(); ++checked)
    {
        if(it == listItem.begin())
            it = listItem.end();

        --it;

        if(
            it->second != nullptr &&
            isItemOnActivePage(it->second) &&
            it->second->isVisible() &&
            it->second->isFocusable())
        {
            return setFocusById(it->first);
        }
    }

    return false;
}

bool liste_item::isItemOnActivePage(const item* currentItem) const
{
    if(currentItem == nullptr)
        return false;

    return
        currentItem->getPageId() == ITEM_PAGE_GLOBAL ||
        currentItem->getPageId() == activePageId;
}

void liste_item::setActivePageId(int pageId)
{
    activePageId = pageId;

    for(auto& entry : listItem)
    {
        if(entry.second != nullptr)
            entry.second->clearPointerState();
    }

    item* focused = getFocusedItem();

    if(focused == nullptr || !isItemOnActivePage(focused))
        clearFocus();
}

bool liste_item::moveItemToPage(int itemId, int pageId)
{
    item* currentItem = getItemsById(itemId);

    if(currentItem == nullptr)
        return false;

    if(focusedItemId == itemId &&
       pageId != ITEM_PAGE_GLOBAL &&
       pageId != activePageId)
    {
        clearFocus();
    }

    currentItem->setPageId(pageId);
    currentItem->clearPointerState();
    return true;
}

bool liste_item::handleSystemExit()
{
    // EXIT is only a UI close/back/defocus action. It is never an application
    // quit request. MENU remains the system mechanism for leaving the add-in.
    item* capture = getKeyboardCapture();

    if(capture != nullptr && capture->handleSystemExit())
        return true;

    item* focused = getFocusedItem();

    if(focused != nullptr && focused != capture && focused->handleSystemExit())
        return true;

    return false;
}

void liste_item::dispatchGlobalEvent(int eventType, int eventKey)
{
    for(auto& entry : listItem)
    {
        item* currentItem = entry.second;

        if(
            currentItem != nullptr &&
            isItemOnActivePage(currentItem) &&
            currentItem->isVisible() &&
            currentItem->wantsGlobalKeyboard())
        {
            currentItem->handleGlobalEvent(eventType, eventKey);
        }
    }
}
//==============================================================================