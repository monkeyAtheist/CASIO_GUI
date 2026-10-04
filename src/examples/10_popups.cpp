#include "CASIO_GUI/casio.hpp"

/*
 * Example 10 - Popups
 *
 * Demonstrates:
 *   - PromptPopup
 *   - BoolPopup
 *   - NumericPopup
 *   - OptionsPopup
 *   - modal behavior
 *   - SoftKeyBar global shortcuts
 *
 * Controls:
 *   - F1: text prompt
 *   - F2: yes/no
 *   - F3: numeric
 *   - F4: options
 *   - EXIT closes the active popup
 */

int main()
{
    casio::GUI gui;

    casio::Label title(
        {8, 24, 380, 24},
        "Popup examples",
        casio::TextAlign::CENTER,
        true
    );

    casio::Label info(
        {30, 76, 336, 60},
        "Use F1..F4 to open a popup.\nPopups are modal and EXIT closes them.",
        casio::TextAlign::CENTER,
        false
    );

    casio::PromptPopup prompt(
        "Text input",
        "Enter text:",
        "",
        24
    );

    casio::BoolPopup confirm(
        "Confirm",
        "Continue?",
        {"Yes", "No"}
    );

    casio::NumericPopup number(
        "Numeric input",
        "Value:",
        50.0,
        0.0,
        100.0,
        1,
        1.0
    );

    casio::OptionsPopup options(
        "Options",
        "Configure:"
    );

    options.addCheckbox(
        "Grid",
        true
    );

    options.addToggle(
        "Cursor",
        false
    );

    options.addRadio(
        "Light",
        1,
        true
    );

    options.addRadio(
        "Dark",
        1,
        false
    );

    casio::StatusBar status(
        {0, casio::HEIGHT - 46, casio::WIDTH, 22},
        "Ready",
        "",
        ""
    );

    prompt.setOnSubmit(
        [&status](
            const std::string&)
        {
            status.setLeftText(
                "Prompt submitted"
            );
        }
    );

    confirm.setOnBool(
        [&status](bool result)
        {
            status.setLeftText(
                result
                    ? "Confirmed"
                    : "Cancelled"
            );
        }
    );

    number.setOnSubmit(
        [&status](double)
        {
            status.setLeftText(
                "Number submitted"
            );
        }
    );

    options.setOnApply(
        [&status](
            casio::OptionsPopup&)
        {
            status.setLeftText(
                "Options applied"
            );
        }
    );

    casio::SoftKeyBar softKeys(
        {"Text", "Bool", "Number", "Options", "", ""}
    );

    softKeys.setCallback(
        0,
        [&prompt]()
        {
            prompt.open();
        }
    );

    softKeys.setCallback(
        1,
        [&confirm]()
        {
            confirm.open();
        }
    );

    softKeys.setCallback(
        2,
        [&number]()
        {
            number.open();
        }
    );

    softKeys.setCallback(
        3,
        [&options]()
        {
            options.open();
        }
    );

    gui.addItem(&title);
    gui.addItem(&info);

    gui.addGlobalItem(&prompt);
    gui.addGlobalItem(&confirm);
    gui.addGlobalItem(&number);
    gui.addGlobalItem(&options);
    gui.addGlobalItem(&status);
    gui.addGlobalItem(&softKeys);

    gui.runGUI();
    return 0;
}
