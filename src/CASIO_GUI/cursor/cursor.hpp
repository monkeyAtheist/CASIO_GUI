#ifndef CURSOR_H
#define CURSOR_H

#include <gint/display.h>
#include <gint/keyboard.h>

class Cursor
{
public:
    enum class cursorStyle
    {
        ARROW,
        CROSS
    };

    Cursor(
        int x,
        int y,
        cursorStyle style = cursorStyle::CROSS)
        : x_(x), y_(y), style_(style)
    {
        clamp();
    }

    void move(int key);
    void draw() const;

    int x() const   {return x_;}
    int y() const   {return y_;}

    void setX(int x) {x_ = x; clamp();}
    void setY(int y) {y_ = y; clamp();}
    void setPos(int x, int y) {x_ = x; y_ = y; clamp();}

    void setSpeed(int speed) {speed_ = (speed > 0) ? speed : 1;}
    int getSpeed() const {return speed_;}

    void setStyle(cursorStyle style) {style_ = style; clamp();}
    cursorStyle getStyle() const {return style_;}

    void setVisible(bool visible) {visible_ = visible;}
    bool isVisible() const {return visible_;}

    void setFocused(bool value) {focused_ = value;};
    bool isFocused() const {return focused_;};

    void setNormalColor(int color) {normalColor_ = color;};
    int getNormalColor() const {return normalColor_;};

    void setFocusColor(int color) {focusColor_ = color;};
    int getFocusColor() const {return focusColor_;};

    void setFocusThickness(int value)
    {
        focusThickness_ = (value < 1) ? 1 : ((value > 5) ? 5 : value);
    };
    int getFocusThickness() const {return focusThickness_;};

    void setFocusRadiusBonus(int value)
    {
        focusRadiusBonus_ = (value < 0) ? 0 : ((value > 5) ? 5 : value);
        clamp();
    };
    int getFocusRadiusBonus() const {return focusRadiusBonus_;};

    void show() {visible_ = true;}
    void hide() {visible_ = false;}

private:
    void clamp();
    void drawArrow() const;
    void drawCross() const;

private:
    int x_;
    int y_;
    int speed_ = 10;
    bool visible_ = true;
    bool focused_ = false;
    int normalColor_ = C_BLACK;
    int focusColor_ = C_RED;
    int focusThickness_ = 3;
    int focusRadiusBonus_ = 2;
    cursorStyle style_ = cursorStyle::CROSS;
};


#endif // CURSOR_H
