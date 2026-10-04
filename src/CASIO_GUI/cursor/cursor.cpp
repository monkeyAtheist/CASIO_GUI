#include "cursor.hpp"

void Cursor::clamp()
{
    if(style_ == cursorStyle::CROSS)
    {
        const int radius =
            7 +
            focusRadiusBonus_;

        if(x_ < radius)
            x_ = radius;

        if(y_ < radius)
            y_ = radius;

        if(x_ > DWIDTH - 1 - radius)
            x_ = DWIDTH - 1 - radius;

        if(y_ > DHEIGHT - 1 - radius)
            y_ = DHEIGHT - 1 - radius;

        return;
    }

    if(x_ < 0)
        x_ = 0;

    if(y_ < 0)
        y_ = 0;

    if(x_ > DWIDTH - 10)
        x_ = DWIDTH - 10;

    if(y_ > DHEIGHT - 16)
        y_ = DHEIGHT - 16;
}

void Cursor::move(int key)
{
    if(!visible_)
        return;

    switch(key)
    {
        case KEY_LEFT:
            x_ -= speed_;
            break;

        case KEY_RIGHT:
            x_ += speed_;
            break;

        case KEY_UP:
            y_ -= speed_;
            break;

        case KEY_DOWN:
            y_ += speed_;
            break;

        default:
            return;
    }

    clamp();
}

void Cursor::drawArrow() const
{
    dline(x_,     y_,      x_,     y_ + 13, (focused_ ? focusColor_ : normalColor_));
    dline(x_,     y_ + 13, x_ + 3, y_ + 10, (focused_ ? focusColor_ : normalColor_));
    dline(x_ + 3, y_ + 10, x_ + 6, y_ + 15, (focused_ ? focusColor_ : normalColor_));
    dline(x_ + 6, y_ + 15, x_ + 8, y_ + 14, (focused_ ? focusColor_ : normalColor_));
    dline(x_ + 8, y_ + 14, x_ + 5, y_ + 9,  (focused_ ? focusColor_ : normalColor_));
    dline(x_ + 5, y_ + 9,  x_ + 9, y_ + 9,  (focused_ ? focusColor_ : normalColor_));
    dline(x_ + 9, y_ + 9,  x_,     y_,      (focused_ ? focusColor_ : normalColor_));
}

void Cursor::drawCross() const
{
    const int baseRadius = 6;
    const int radius =
        baseRadius +
        (focused_ ? focusRadiusBonus_ : 0);

    if(!focused_)
    {
        dline(x_ - radius, y_ - 1, x_ + radius, y_ - 1, C_WHITE);
        dline(x_ - radius, y_ + 1, x_ + radius, y_ + 1, C_WHITE);
        dline(x_ - 1, y_ - radius, x_ - 1, y_ + radius, C_WHITE);
        dline(x_ + 1, y_ - radius, x_ + 1, y_ + radius, C_WHITE);

        dline(x_ - radius, y_, x_ + radius, y_, normalColor_);
        dline(x_, y_ - radius, x_, y_ + radius, normalColor_);
        return;
    }

    // Strong focus marker: larger, outlined, and thick.
    for(int offset = -2;
        offset <= 2;
        ++offset)
    {
        dline(
            x_ - radius,
            y_ + offset,
            x_ + radius,
            y_ + offset,
            C_BLACK
        );

        dline(
            x_ + offset,
            y_ - radius,
            x_ + offset,
            y_ + radius,
            C_BLACK
        );
    }

    dline(x_ - radius, y_ - 1, x_ + radius, y_ - 1, C_WHITE);
    dline(x_ - radius, y_ + 1, x_ + radius, y_ + 1, C_WHITE);
    dline(x_ - 1, y_ - radius, x_ - 1, y_ + radius, C_WHITE);
    dline(x_ + 1, y_ - radius, x_ + 1, y_ + radius, C_WHITE);

    int half =
        focusThickness_ /
        2;

    for(int offset = -half;
        offset <= half;
        ++offset)
    {
        dline(
            x_ - radius,
            y_ + offset,
            x_ + radius,
            y_ + offset,
            focusColor_
        );

        dline(
            x_ + offset,
            y_ - radius,
            x_ + offset,
            y_ + radius,
            focusColor_
        );
    }

    drect(
        x_ - 2,
        y_ - 2,
        x_ + 2,
        y_ + 2,
        focusColor_
    );
}


void Cursor::draw() const
{
    if(!visible_)
        return;

    switch(style_)
    {
        case cursorStyle::ARROW:
            drawArrow();
            break;

        case cursorStyle::CROSS:
            drawCross();
            break;
    }
}
