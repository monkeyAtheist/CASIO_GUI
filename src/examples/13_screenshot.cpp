#include "CASIO_GUI/casio.hpp"

/*
 * Example 13 - Screenshot
 *
 * Demonstrates:
 *   - GUI screenshot shortcut
 *   - PromptPopup filename entry
 *   - /Capt/*.bmp output
 *   - callback after save
 *
 * Controls:
 *   - OPTN: open the screenshot filename popup.
 *   - default filename: image
 *
 * If a widget already owns keyboard focus, EXIT releases it before OPTN.
 */

int main()
{
    casio::GUI gui;

    casio::Label title(
        {8, 30, 380, 28},
        "Screenshot example",
        casio::TextAlign::CENTER,
        true
    );

    casio::Rectangle card(
        {88, 78, 220, 74},
        casio::Color(225, 238, 255),
        casio::Color(35, 95, 200),
        casio::DrawMode::FILLED,
        2
    );

    casio::Label text(
        {98, 94, 200, 40},
        "Press OPTN\nSave to /Capt/image.bmp",
        casio::TextAlign::CENTER,
        false
    );

    casio::StatusBar status(
        {0, casio::HEIGHT - 22, casio::WIDTH, 22},
        "OPTN: screenshot",
        "",
        ""
    );

    gui.enableScreenshotCapture(
        casio::key::OPTN,
        "image"
    );

    gui.setScreenshotResultCallback(
        [&status](
            bool success,
            const std::string& path)
        {
            if(success)
            {
                status.setLeftText(
                    path
                );

                status.setStatus(
                    casio::LEDState::ON,
                    true
                );
            }
            else
            {
                status.setLeftText(
                    "Screenshot error"
                );

                status.setStatus(
                    casio::LEDState::ERROR,
                    true
                );
            }
        }
    );

    gui.addItem(&title);
    gui.addItem(&card);
    gui.addItem(&text);
    gui.addGlobalItem(&status);

    gui.runGUI();
    return 0;
}
