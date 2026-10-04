#ifndef GUI_COLOR_HPP
#define GUI_COLOR_HPP

#include <gint/display.h>
#include <gint/keyboard.h>

#define MAX_INT_COLOR_VALUE 255
#define DEFAULT_ITEM_COLOR Class_color(0, 0, 0)     // Default color for items (black)

//******************************** Class color *********************************
class Class_color
{
public:

    Class_color(int _r = 0, int _g = 0, int _b = 0)
        : r(_r), g(_g), b(_b)
    {
        if(r > MAX_INT_COLOR_VALUE) r = MAX_INT_COLOR_VALUE;
        if(r < 0) r = 0;

        if(g > MAX_INT_COLOR_VALUE) g = MAX_INT_COLOR_VALUE;
        if(g < 0) g = 0;

        if(b > MAX_INT_COLOR_VALUE) b = MAX_INT_COLOR_VALUE;
        if(b < 0) b = 0;
    }

    int getR() const
    {
        return r;
    }

    int getG() const
    {
        return g;
    }

    int getB() const
    {
        return b;
    }

    int getRGB() const
    {
        return C_RGB(
            r * 31 / 255,
            g * 31 / 255,
            b * 31 / 255
        );
    }

    void setR(int _r)
    {
        r = _r;

        if(r > 255) r = 255;
        if(r < 0)   r = 0;
    }

    void setG(int _g)
    {
        g = _g;

        if(g > 255) g = 255;
        if(g < 0)   g = 0;
    }

    void setB(int _b)
    {
        b = _b;

        if(b > 255) b = 255;
        if(b < 0)   b = 0;
    }

    void setRGB(int _r, int _g, int _b)
    {
        setR(_r);
        setG(_g);
        setB(_b);
    }

private:

    int r;
    int g;
    int b;
};


// Color of the item
typedef struct RawitemColor{
    Class_color fillColor;
    Class_color borderColor;
    Class_color TextColor;

    RawitemColor() = default;
    RawitemColor(Class_color _fillColor, Class_color _borderColor, Class_color _TextColor)
        : fillColor(_fillColor), borderColor(_borderColor), TextColor(_TextColor)
    {
    }
}RawitemColor;

// Color of the item in different states
typedef struct itemColor{
    RawitemColor offColor;
    RawitemColor onColor;
    RawitemColor hoverOffColor;
    RawitemColor hoverOnColor;
    RawitemColor clickColor;
    RawitemColor dragColor;
    RawitemColor dimmedColor;

    itemColor() = default;
    itemColor(
        RawitemColor _offColor,
        RawitemColor _onColor,
        RawitemColor _hoverOffColor,
        RawitemColor _hoverOnColor,
        RawitemColor _clickColor,
        RawitemColor _dragColor,
        RawitemColor _dimmedColor)
        : offColor(_offColor),
          onColor(_onColor),
          hoverOffColor(_hoverOffColor),
          hoverOnColor(_hoverOnColor),
          clickColor(_clickColor),
          dragColor(_dragColor),
          dimmedColor(_dimmedColor)
    {
    }
}itemColor;

#endif // GUI_COLOR_HPP
