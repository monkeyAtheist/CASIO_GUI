#include "CASIO_GUI/casio.hpp"

/*
 * Example 01 - Pages
 *
 * Demonstrates:
 *   - createPage()
 *   - addItemToPage()
 *   - global items
 *   - SoftKeyBar page navigation
 *   - different page backgrounds
 *
 * Controls:
 *   - F1: Home
 *   - F2: Settings
 *   - F3: About
 */

int main()
{
    casio::GUI gui;

    const int pageHome =
        gui.getCurrentPageId();

    const int pageSettings =
        gui.createPage(
            "Settings",
            casio::rgb(238, 244, 255)
        );

    const int pageAbout =
        gui.createPage(
            "About",
            casio::rgb(248, 248, 238)
        );

    casio::Label homeTitle(
        {8, 20, 380, 24},
        "Page: Home",
        casio::TextAlign::CENTER,
        true
    );

    casio::Label homeText(
        {18, 70, 360, 40},
        "Each page owns its visible controls.",
        casio::TextAlign::CENTER,
        false
    );

    casio::Label settingsTitle(
        {8, 20, 380, 24},
        "Page: Settings",
        casio::TextAlign::CENTER,
        true
    );

    casio::Checkbox settingA(
        {40, 70, 180, 24},
        "Enable option",
        true
    );

    casio::Slider settingB(
        {40, 110, 220, 24},
        40,
        0,
        100,
        5
    );

    casio::Label aboutTitle(
        {8, 20, 380, 24},
        "Page: About",
        casio::TextAlign::CENTER,
        true
    );

    casio::Label aboutText(
        {20, 70, 356, 60},
        "CASIO GUI 1.0.0\nFrozen public API example.",
        casio::TextAlign::CENTER,
        false
    );

    casio::SoftKeyBar softKeys(
        {"Home", "Settings", "About", "", "", ""}
    );

    softKeys.setCallback(
        0,
        [&gui, pageHome]()
        {
            gui.setPage(pageHome);
        }
    );

    softKeys.setCallback(
        1,
        [&gui, pageSettings]()
        {
            gui.setPage(pageSettings);
        }
    );

    softKeys.setCallback(
        2,
        [&gui, pageAbout]()
        {
            gui.setPage(pageAbout);
        }
    );

    gui.addItemToPage(
        pageHome,
        &homeTitle
    );

    gui.addItemToPage(
        pageHome,
        &homeText
    );

    gui.addItemToPage(
        pageSettings,
        &settingsTitle
    );

    gui.addItemToPage(
        pageSettings,
        &settingA
    );

    gui.addItemToPage(
        pageSettings,
        &settingB
    );

    gui.addItemToPage(
        pageAbout,
        &aboutTitle
    );

    gui.addItemToPage(
        pageAbout,
        &aboutText
    );

    gui.addGlobalItem(
        &softKeys
    );

    gui.setPage(pageHome);
    gui.runGUI();

    return 0;
}
