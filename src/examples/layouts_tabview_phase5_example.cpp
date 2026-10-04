#include "CASIO_GUI/casio.hpp"

int main()
{
    casio::GUI gui;

    auto* tabs = new casio::TabView(
        {12, 20, 360, 175},
        {
            "General",
            "Controls",
            "Status"
        },
        0
    );

    auto* name = new casio::TextBox(
        {0, 0, 150, 28},
        "Device"
    );

    auto* enabled = new casio::Checkbox(
        {0, 0, 150, 24},
        "Enabled",
        true
    );

    tabs->addChildToTab(
        0,
        name,
        {10, 10, 160, 28}
    );

    tabs->addChildToTab(
        0,
        enabled,
        {10, 48, 160, 24}
    );

    auto* controls = new casio::Panel(
        {0, 0, 300, 115},
        casio::LayoutMode::VERTICAL,
        6,
        5
    );

    controls->setContainerTitle(
        "Control values"
    );

    auto* slider = new casio::Slider(
        {0, 0, 180, 24},
        50
    );

    auto* progress = new casio::ProgressBar(
        {0, 0, 180, 20},
        50
    );

    controls->addChild(
        slider,
        {0, 0, 180, 24}
    );

    controls->addChild(
        progress,
        {0, 0, 180, 20}
    );

    tabs->addChildToTab(
        1,
        controls,
        {8, 8, 320, 120},
        {},
        casio::ANCHOR_LEFT |
        casio::ANCHOR_RIGHT |
        casio::ANCHOR_TOP |
        casio::ANCHOR_BOTTOM
    );

    auto* statusScroll =
        new casio::ScrollView(
            {0, 0, 320, 120},
            casio::LayoutMode::VERTICAL,
            5,
            3
        );

    for(int i = 0; i < 10; ++i)
    {
        auto* check =
            new casio::Checkbox(
                {0, 0, 200, 22},
                "Status line",
                (i % 2) == 0
            );

        statusScroll->addChild(
            check,
            {0, 0, 200, 22}
        );
    }

    tabs->addChildToTab(
        2,
        statusScroll,
        {8, 8, 320, 120},
        {},
        casio::ANCHOR_LEFT |
        casio::ANCHOR_RIGHT |
        casio::ANCHOR_TOP |
        casio::ANCHOR_BOTTOM
    );

    gui.item.addItem(tabs);
    gui.runGUI();

    return 1;
}
