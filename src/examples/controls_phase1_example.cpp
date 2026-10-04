#include "CASIO_GUI/casio.hpp"

int main()
{
    casio::GUI gui;

    auto* wheel = new casio::ColorWheel(
        {10, 10, 90, 90},
        0.0f,
        3.0f
    );

    auto* checkbox = new casio::Checkbox(
        {110, 15, 130, 24},
        "Grid",
        true
    );

    auto* slider = new casio::VerticalSlider(
        {255, 10, 24, 100},
        50,
        0,
        100,
        5
    );

    auto* progress = new casio::ProgressBar(
        {110, 55, 120, 18},
        65
    );

    auto* verticalProgress =
        new casio::VerticalProgressBar(
            {290, 10, 18, 100},
            40
        );

    auto* scroll = new casio::ScrollBar(
        {110, 85, 120, 16},
        20,
        0,
        100,
        20,
        5
    );

    auto* verticalScroll =
        new casio::VerticalScrollBar(
            {320, 10, 16, 100},
            20,
            0,
            100,
            20,
            5
        );

    auto* led = new casio::LED(
        {110, 115, 100, 24},
        casio::LEDState::ON,
        "RUN"
    );

    wheel->setOnChanged(
        [led](float hue, casio::Color color)
        {
            (void)hue;
            led->setOnColor(color);
        }
    );

    checkbox->setOnChanged(
        [led](bool checked)
        {
            led->setState(
                checked
                    ? casio::LEDState::ON
                    : casio::LEDState::OFF
            );
        }
    );

    slider->setOnChanged(
        [progress](int value)
        {
            progress->setValue(value);
        }
    );

    scroll->setOnScroll(
        [verticalScroll](int value)
        {
            verticalScroll->setPosition(value);
        }
    );

    led->setBlink(true, 500);

    gui.item.addItem(wheel);
    gui.item.addItem(checkbox);
    gui.item.addItem(slider);
    gui.item.addItem(progress);
    gui.item.addItem(verticalProgress);
    gui.item.addItem(scroll);
    gui.item.addItem(verticalScroll);
    gui.item.addItem(led);

    gui.runGUI();

    return 1;
}
