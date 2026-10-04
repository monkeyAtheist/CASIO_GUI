#include "CASIO_GUI/casio.hpp"

/*
 * Example 09 - Instruments
 *
 * Demonstrates:
 *   - ColorWheel
 *   - Numeric
 *   - LED
 *   - Gauge
 *   - Dial
 *   - ProgressBar
 *
 * ColorWheel and Numeric are linked bidirectionally.
 */

int main()
{
    casio::GUI gui;

    casio::Label title(
        {8, 2, 380, 18},
        "Instruments",
        casio::TextAlign::CENTER,
        true
    );

    casio::ColorWheel wheel(
        {8, 28, 110, 110},
        0.0f,
        5.0f
    );

    casio::Numeric hue(
        {8, 146, 110, 28},
        0,
        0,
        359,
        5
    );

    casio::LED colorLed(
        {132, 32, 94, 28},
        casio::LEDState::ON,
        "Color",
        casio::LEDShape::RECTANGLE
    );

    colorLed.setOnColor(
        wheel.getColor()
    );

    casio::Gauge gauge(
        {132, 68, 120, 106},
        50,
        0,
        100,
        135.0f,
        405.0f,
        10,
        4,
        "%"
    );

    gauge.addZone(
        0,
        60,
        casio::Color(20, 160, 70)
    );

    gauge.addZone(
        60,
        80,
        casio::Color(235, 155, 20)
    );

    gauge.addZone(
        80,
        100,
        casio::Color(210, 40, 40)
    );

    casio::Dial dial(
        {262, 32, 126, 110},
        35,
        0,
        100,
        5,
        135.0f,
        405.0f,
        10,
        4,
        "%"
    );

    casio::ProgressBar progress(
        {262, 154, 126, 20},
        35,
        0,
        100,
        casio::Orientation::HORIZONTAL,
        true
    );

    wheel.setOnChanged(
        [&hue, &colorLed](
            float value,
            casio::Color color)
        {
            hue.setValue(
                static_cast<int>(
                    value + 0.5f
                )
            );

            colorLed.setOnColor(color);
            colorLed.setState(
                casio::LEDState::ON
            );
        }
    );

    hue.setOnChanged(
        [&wheel](int value)
        {
            wheel.setHue(
                static_cast<float>(value)
            );
        }
    );

    dial.setOnChanged(
        [&gauge, &progress](int value)
        {
            gauge.setValue(value);
            progress.setValue(value);
        }
    );

    gui.addItem(&title);
    gui.addItem(&wheel);
    gui.addItem(&hue);
    gui.addItem(&colorLed);
    gui.addItem(&gauge);
    gui.addItem(&dial);
    gui.addItem(&progress);

    gui.runGUI();
    return 0;
}
