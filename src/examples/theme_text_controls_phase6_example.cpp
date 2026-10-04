#include "CASIO_GUI/casio.hpp"

int main()
{
    casio::GUI gui;

    gui.setThemePreset(
        casio::ThemePreset::DARK
    );

    auto* toolbar =
        new casio::Toolbar(
            {0, 0, DWIDTH, 28},
            10
        );

    auto* status =
        new casio::StatusBar(
            {0, DHEIGHT - 22, DWIDTH, 22},
            "Ready",
            "GRAPH 90+E",
            "ALPHA input",
            10
        );

    auto* panel =
        new casio::Panel(
            {20, 40, 250, 130},
            casio::LayoutMode::VERTICAL,
            8,
            6
        );

    panel->setContainerTitle(
        "Device configuration"
    );

    auto* title =
        new casio::Label(
            {0, 0, 180, 22},
            "Device name",
            casio::TextAlign::LEFT
        );

    auto* name =
        new casio::TextBox(
            {0, 0, 180, 28},
            "",
            "ALPHA to type...",
            32
        );

    auto* separator =
        new casio::Separator(
            {0, 0, 180, 4}
        );

    auto* enabled =
        new casio::Checkbox(
            {0, 0, 180, 24},
            "Enabled",
            true
        );

    panel->addChild(
        title,
        {0, 0, 180, 22}
    );

    panel->addChild(
        name,
        {0, 0, 180, 28}
    );

    panel->addChild(
        separator,
        {0, 0, 180, 4}
    );

    panel->addChild(
        enabled,
        {0, 0, 180, 24}
    );

    name->setOnTextChanged(
        [status](const std::string& text)
        {
            status->setLeftText(
                text.empty()
                    ? "Ready"
                    : text
            );
        }
    );

    toolbar->addAction(
        "Light",
        [&gui]()
        {
            gui.setThemePreset(
                casio::ThemePreset::LIGHT
            );
        },
        KEY_F1
    );

    toolbar->addAction(
        "Dark",
        [&gui]()
        {
            gui.setThemePreset(
                casio::ThemePreset::DARK
            );
        },
        KEY_F2
    );

    toolbar->addAction(
        "Contrast",
        [&gui]()
        {
            gui.setThemePreset(
                casio::ThemePreset::HIGH_CONTRAST
            );
        },
        KEY_F3
    );

    status->setStatus(
        casio::LEDState::ON
    );

    gui.addGlobalItem(toolbar);
    gui.item.addItem(panel);
    gui.addGlobalItem(status);

    // Re-apply once all controls exist so every custom renderer receives
    // the chosen palette.
    gui.setThemePreset(
        casio::ThemePreset::DARK
    );

    gui.runGUI();

    return 1;
}
