#include "CASIO_GUI/casio.hpp"

/*
 * Example 11 - Images
 *
 * Uses the supplied assets-cg/gui_test_image.png converted by fxconv.
 *
 * Demonstrates:
 *   - ImageItem
 *   - ImageButton
 *   - FIT / FILL
 *   - mirror
 *   - rotation
 *   - theme-aware image border
 */

extern "C" bopti_image_t img_gui_test;

int main()
{
    casio::GUI gui;

    casio::Label title(
        {8, 2, 380, 18},
        "Image controls",
        casio::TextAlign::CENTER,
        true
    );

    casio::ImageItem fitImage(
        {20, 34, 100, 80},
        &img_gui_test,
        0,
        0,
        true,
        casio::Color(0, 0, 0),
        0,
        casio::ImageMode::FIT
    );

    fitImage.setUseThemeBorder(true);

    casio::ImageItem rotatedImage(
        {148, 34, 100, 80},
        &img_gui_test,
        0,
        0,
        true,
        casio::Color(0, 0, 0),
        0,
        casio::ImageMode::FIT
    );

    rotatedImage.setUseThemeBorder(true);
    rotatedImage.setRotation(20.0f);

    casio::StatusBar status(
        {0, casio::HEIGHT - 22, casio::WIDTH, 22},
        "ImageButton: press EXE",
        "",
        ""
    );

    casio::ImageButton imageButton(
        {276, 34, 100, 80},
        &img_gui_test,
        [&status]()
        {
            status.setLeftText(
                "ImageButton activated"
            );
        },
        0,
        0,
        true,
        0,
        casio::ImageMode::FILL
    );

    imageButton.setUseThemeBorder(true);

    casio::Label fitLabel(
        {20, 120, 100, 20},
        "FIT",
        casio::TextAlign::CENTER,
        false
    );

    casio::Label rotateLabel(
        {148, 120, 100, 20},
        "FIT + rotation",
        casio::TextAlign::CENTER,
        false
    );

    casio::Label buttonLabel(
        {276, 120, 100, 20},
        "ImageButton",
        casio::TextAlign::CENTER,
        false
    );

    gui.addItem(&title);
    gui.addItem(&fitImage);
    gui.addItem(&rotatedImage);
    gui.addItem(&imageButton);
    gui.addItem(&fitLabel);
    gui.addItem(&rotateLabel);
    gui.addItem(&buttonLabel);
    gui.addGlobalItem(&status);

    gui.runGUI();
    return 0;
}
