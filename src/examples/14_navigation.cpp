#include "CASIO_GUI/casio.hpp"

/*
 * Example 14 - MenuBar, SoftKeyBar and navigation
 *
 * Demonstrates:
 *   - MenuBar
 *   - keyboard-focused menu selection
 *   - SoftKeyBar global keyboard handling
 *   - theme switching
 *
 * Controls:
 *   - EXE on MenuBar: take focus / activate.
 *   - LEFT/RIGHT: select menu entry while focused.
 *   - F1/F2/F3: theme.
 */

int main()
{
    casio::GUI gui;

    casio::MenuBar menu(
        {0, 0, casio::WIDTH, 24},
        {"File", "Edit", "View"},
        0
    );

    casio::Label content(
        {20, 70, 356, 70},
        "MenuBar selection follows keyboard focus.\nSoft keys remain global.",
        casio::TextAlign::CENTER,
        false
    );

    casio::StatusBar status(
        {0, casio::HEIGHT - 46, casio::WIDTH, 22},
        "Ready",
        "",
        ""
    );

    menu.setCallback(
        0,
        [&status]()
        {
            status.setLeftText(
                "File selected"
            );
        }
    );

    menu.setCallback(
        1,
        [&status]()
        {
            status.setLeftText(
                "Edit selected"
            );
        }
    );

    menu.setCallback(
        2,
        [&status]()
        {
            status.setLeftText(
                "View selected"
            );
        }
    );

    casio::SoftKeyBar softKeys(
        {"Light", "Dark", "HC", "", "", ""}
    );

    softKeys.setCallback(
        0,
        [&gui]()
        {
            gui.setThemePreset(
                casio::ThemePreset::LIGHT
            );
        }
    );

    softKeys.setCallback(
        1,
        [&gui]()
        {
            gui.setThemePreset(
                casio::ThemePreset::DARK
            );
        }
    );

    softKeys.setCallback(
        2,
        [&gui]()
        {
            gui.setThemePreset(
                casio::ThemePreset::HIGH_CONTRAST
            );
        }
    );

    gui.addItem(&menu);
    gui.addItem(&content);
    gui.addGlobalItem(&status);
    gui.addGlobalItem(&softKeys);

    gui.runGUI();
    return 0;
}
