#include "CASIO_GUI/casio.hpp"

/*
 * Example 00 - Minimal GUI
 *
 * Demonstrates:
 *   - GUI
 *   - Label
 *   - Button
 *   - StatusBar
 *   - theme preset
 *
 * Controls:
 *   - Move the cursor with the arrows.
 *   - Put the cursor on the button and press EXE.
 *   - MENU is left to the calculator/OS.
 */

int main()
{
    casio::GUI gui;

    gui.setThemePreset(
        casio::ThemePreset::LIGHT
    );

    casio::Label title(
        {8, 8, 380, 22},
        "CASIO GUI 1.0 - Minimal example",
        casio::TextAlign::CENTER,
        true
    );

    casio::Button button(
        {128, 72, 140, 42}
    );

    button.setLabelOff("Press EXE");
    button.setLabelOn("Press EXE");
    button.setLabelOffHover("Press EXE");
    button.setLabelOnHover("Press EXE");

    casio::StatusBar status(
        {0, casio::HEIGHT - 22, casio::WIDTH, 22},
        "Ready",
        "",
        "v1.0"
    );

    button.connectCallback(
        [&status]()
        {
            status.setLeftText(
                "Button activated"
            );

            status.setStatus(
                casio::LEDState::ON,
                true
            );
        }
    );

    gui.addItem(&title);
    gui.addItem(&button);
    gui.addGlobalItem(&status);

    gui.runGUI();
    return 0;
}
