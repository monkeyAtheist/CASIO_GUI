#include "CASIO_GUI/casio.hpp"

/*
 * Example 02 - Tab and TabView
 *
 * Demonstrates:
 *   - Tab: lightweight tab selector
 *   - TabView: tabs + child content
 *   - focus-aware keyboard navigation
 *   - children that keep their normal focus rules
 *
 * TabView:
 *   - EXE on the header takes focus.
 *   - LEFT / RIGHT changes tab.
 *   - EXIT releases focus.
 */

int main()
{
    casio::GUI gui;

    casio::Label title(
        {8, 4, 380, 20},
        "Tab + TabView",
        casio::TextAlign::CENTER,
        true
    );

    casio::Tab simpleTab(
        {18, 30, 220, 26},
        {"One", "Two", "Three"},
        0
    );

    casio::StatusBar status(
        {0, casio::HEIGHT - 22, casio::WIDTH, 22},
        "Select a tab",
        "",
        ""
    );

    simpleTab.setOnTabChanged(
        [&status](int)
        {
            status.setLeftText(
                "Simple Tab changed"
            );
        }
    );

    casio::TabView tabs(
        {18, 64, 360, 132},
        {"General", "Input", "Info"},
        0,
        24,
        6
    );

    casio::Label generalLabel(
        {0, 0, 180, 20},
        "General tab",
        casio::TextAlign::LEFT,
        false
    );

    casio::Checkbox generalCheck(
        {0, 0, 180, 24},
        "Standalone checkbox",
        false
    );

    casio::TextBox inputText(
        {0, 0, 170, 28},
        "",
        "Text / numeric",
        24,
        0,
        casio::TextBoxInputMode::MIXED
    );

    casio::Numeric inputNumeric(
        {0, 0, 100, 28},
        10,
        0,
        100,
        1
    );

    casio::Label infoLabel(
        {0, 0, 300, 48},
        "Children are non-owning.\nTabView only displays the active tab.",
        casio::TextAlign::CENTER,
        false
    );

    tabs.addChildToTab(
        0,
        &generalLabel,
        {8, 8, 180, 20}
    );

    tabs.addChildToTab(
        0,
        &generalCheck,
        {8, 40, 180, 24}
    );

    tabs.addChildToTab(
        1,
        &inputText,
        {8, 8, 170, 28}
    );

    tabs.addChildToTab(
        1,
        &inputNumeric,
        {8, 50, 100, 28}
    );

    tabs.addChildToTab(
        2,
        &infoLabel,
        {8, 16, 320, 48}
    );

    tabs.setOnTabChanged(
        [&status](int)
        {
            status.setLeftText(
                "TabView page changed"
            );
        }
    );

    gui.addItem(&title);
    gui.addItem(&simpleTab);
    gui.addItem(&tabs);
    gui.addGlobalItem(&status);

    gui.runGUI();
    return 0;
}
