#include "CASIO_GUI/casio.hpp"

/*
 * Example 03 - Common controls
 *
 * Demonstrates:
 *   - TextBox MIXED mode
 *   - Numeric
 *   - Slider
 *   - Checkbox
 *   - RadioGroup
 *   - ComboBox
 *   - LED
 *   - Button callback
 *
 * TextBox MIXED mode:
 *   - ALPHA toggles text/numeric mode.
 *   - SHIFT toggles Caps Lock while in text mode.
 */

int main()
{
    casio::GUI gui;

    casio::Label title(
        {8, 3, 380, 18},
        "Controls",
        casio::TextAlign::CENTER,
        true
    );

    casio::TextBox textBox(
        {8, 28, 150, 28},
        "",
        "Text / numeric",
        24,
        0,
        casio::TextBoxInputMode::MIXED
    );

    textBox.setAutoAlphaOnFocus(true);

    casio::Numeric numeric(
        {170, 28, 82, 28},
        25,
        0,
        100,
        5
    );

    casio::Slider slider(
        {8, 66, 150, 24},
        25,
        0,
        100,
        5
    );

    casio::Checkbox check(
        {170, 66, 150, 24},
        "Enabled",
        true
    );

    casio::RadioGroup radio(
        {8, 102, 116, 72},
        {"A", "B", "C"},
        0,
        22
    );

    casio::ComboBox combo(
        {138, 102, 126, 26},
        {"Low", "Medium", "High"},
        1,
        3,
        22
    );

    casio::LED led(
        {286, 102, 92, 28},
        casio::LEDState::ON,
        "State",
        casio::LEDShape::CIRCLE
    );

    casio::Button reset(
        {286, 144, 92, 32}
    );

    reset.setLabelOff("Reset");
    reset.setLabelOn("Reset");
    reset.setLabelOffHover("Reset");
    reset.setLabelOnHover("Reset");

    casio::StatusBar status(
        {0, casio::HEIGHT - 22, casio::WIDTH, 22},
        "Ready",
        "",
        ""
    );

    slider.setOnChanged(
        [&numeric, &status](int value)
        {
            numeric.setValue(value);
            status.setLeftText(
                "Slider changed"
            );
        }
    );

    numeric.setOnChanged(
        [&slider, &status](int value)
        {
            slider.setValue(value);
            status.setLeftText(
                "Numeric changed"
            );
        }
    );

    check.setOnChanged(
        [&led, &status](bool checked)
        {
            led.setState(
                checked
                    ? casio::LEDState::ON
                    : casio::LEDState::OFF
            );

            status.setLeftText(
                "Checkbox changed"
            );
        }
    );

    radio.setOnChanged(
        [&status](
            int,
            const std::string&)
        {
            status.setLeftText(
                "Radio selection changed"
            );
        }
    );

    combo.setOnChanged(
        [&status](
            int,
            const std::string&)
        {
            status.setLeftText(
                "Combo selection changed"
            );
        }
    );

    textBox.setOnTextChanged(
        [&status](
            const std::string&)
        {
            status.setLeftText(
                "Text changed"
            );
        }
    );

    reset.connectCallback(
        [&textBox, &numeric, &slider, &check, &radio, &combo, &led, &status]()
        {
            textBox.clear();
            numeric.setValue(25);
            slider.setValue(25);
            check.setChecked(true);
            radio.setSelectedIndex(0);
            combo.setSelectedIndex(1);
            led.setState(
                casio::LEDState::ON
            );

            status.setLeftText(
                "Controls reset"
            );
        }
    );

    gui.addItem(&title);
    gui.addItem(&textBox);
    gui.addItem(&numeric);
    gui.addItem(&slider);
    gui.addItem(&check);
    gui.addItem(&radio);
    gui.addItem(&combo);
    gui.addItem(&led);
    gui.addItem(&reset);
    gui.addGlobalItem(&status);

    gui.runGUI();
    return 0;
}
