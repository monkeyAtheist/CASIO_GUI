#include "CASIO_GUI/casio.hpp"

#include <cmath>

/*
 * Example 15 - Complete dashboard
 *
 * Combines:
 *   - Pages
 *   - SoftKeyBar
 *   - Graph
 *   - Spreadsheet
 *   - controls
 *   - themes
 *   - screenshot shortcut
 *
 * This is intentionally larger than the focused examples.
 */

int main()
{
    casio::GUI gui;

    const int pageDashboard =
        gui.getCurrentPageId();

    const int pageData =
        gui.createPage(
            "Data"
        );

    const int pageSettings =
        gui.createPage(
            "Settings"
        );

    //--------------------------------------------------------------------------
    // Dashboard page
    //--------------------------------------------------------------------------

    casio::Label dashboardTitle(
        {8, 2, 380, 18},
        "Dashboard",
        casio::TextAlign::CENTER,
        true
    );

    casio::GraphAxis xAxis(
        0.0,
        10.0,
        casio::GraphScale::LINEAR,
        5,
        true,
        true,
        "t",
        true,
        1
    );

    casio::GraphAxis yAxis(
        -1.5,
        1.5,
        casio::GraphScale::LINEAR,
        6,
        true,
        true,
        "y",
        true,
        2
    );

    casio::Graph graph(
        {8, 24, 250, 150},
        xAxis,
        yAxis
    );

    const int series =
        graph.addSeries(
            casio::Color(35, 95, 200),
            true,
            false
        );

    for(int i = 0; i <= 40; ++i)
    {
        const double x =
            10.0 * i / 40.0;

        graph.addPoint(
            static_cast<unsigned int>(series),
            x,
            std::sin(x)
        );
    }

    casio::Gauge gauge(
        {266, 30, 120, 100},
        65,
        0,
        100,
        135.0f,
        405.0f,
        10,
        4,
        "%"
    );

    casio::LED led(
        {278, 140, 96, 28},
        casio::LEDState::ON,
        "RUN",
        casio::LEDShape::RECTANGLE
    );

    gui.addItemToPage(
        pageDashboard,
        &dashboardTitle
    );

    gui.addItemToPage(
        pageDashboard,
        &graph
    );

    gui.addItemToPage(
        pageDashboard,
        &gauge
    );

    gui.addItemToPage(
        pageDashboard,
        &led
    );

    //--------------------------------------------------------------------------
    // Data page
    //--------------------------------------------------------------------------

    casio::Label dataTitle(
        {8, 2, 380, 18},
        "Data",
        casio::TextAlign::CENTER,
        true
    );

    casio::Spreadsheet table(
        {8, 26, 380, 150},
        8,
        4,
        76,
        20,
        true
    );

    table.setColumnLabel(0, "Channel");
    table.setColumnLabel(1, "Min");
    table.setColumnLabel(2, "Max");
    table.setColumnLabel(3, "Value");

    table.setCell(0, 0, "A");
    table.setCell(0, 1, "0");
    table.setCell(0, 2, "100");
    table.setCell(0, 3, "65");

    table.setCell(1, 0, "B");
    table.setCell(1, 1, "-10");
    table.setCell(1, 2, "10");
    table.setCell(1, 3, "2");

    gui.addItemToPage(
        pageData,
        &dataTitle
    );

    gui.addItemToPage(
        pageData,
        &table
    );

    //--------------------------------------------------------------------------
    // Settings page
    //--------------------------------------------------------------------------

    casio::Label settingsTitle(
        {8, 2, 380, 18},
        "Settings",
        casio::TextAlign::CENTER,
        true
    );

    casio::TextBox deviceName(
        {40, 40, 220, 28},
        "Graph90",
        "Device name",
        24
    );

    casio::Slider level(
        {40, 84, 220, 24},
        65,
        0,
        100,
        5
    );

    casio::Checkbox enabled(
        {40, 126, 180, 24},
        "Enabled",
        true
    );

    level.setOnChanged(
        [&gauge](int value)
        {
            gauge.setValue(value);
        }
    );

    enabled.setOnChanged(
        [&led](bool value)
        {
            led.setState(
                value
                    ? casio::LEDState::ON
                    : casio::LEDState::OFF
            );
        }
    );

    gui.addItemToPage(
        pageSettings,
        &settingsTitle
    );

    gui.addItemToPage(
        pageSettings,
        &deviceName
    );

    gui.addItemToPage(
        pageSettings,
        &level
    );

    gui.addItemToPage(
        pageSettings,
        &enabled
    );

    //--------------------------------------------------------------------------
    // Global navigation
    //--------------------------------------------------------------------------

    casio::SoftKeyBar softKeys(
        {"Dash", "Data", "Settings", "Theme", "", ""}
    );

    softKeys.setCallback(
        0,
        [&gui, pageDashboard]()
        {
            gui.setPage(pageDashboard);
        }
    );

    softKeys.setCallback(
        1,
        [&gui, pageData]()
        {
            gui.setPage(pageData);
        }
    );

    softKeys.setCallback(
        2,
        [&gui, pageSettings]()
        {
            gui.setPage(pageSettings);
        }
    );

    softKeys.setCallback(
        3,
        [&gui]()
        {
            static int theme = 0;

            ++theme;

            if(theme > 2)
                theme = 0;

            if(theme == 0)
            {
                gui.setThemePreset(
                    casio::ThemePreset::LIGHT
                );
            }
            else if(theme == 1)
            {
                gui.setThemePreset(
                    casio::ThemePreset::DARK
                );
            }
            else
            {
                gui.setThemePreset(
                    casio::ThemePreset::HIGH_CONTRAST
                );
            }
        }
    );

    gui.enableScreenshotCapture(
        casio::key::OPTN,
        "dashboard"
    );

    gui.addGlobalItem(
        &softKeys
    );

    gui.setPage(
        pageDashboard
    );

    gui.runGUI();
    return 0;
}
