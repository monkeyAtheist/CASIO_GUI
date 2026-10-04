#include "CASIO_GUI/casio.hpp"

/*
 * Example 04 - Layouts and containers
 *
 * Demonstrates:
 *   - Panel / Container
 *   - VERTICAL layout
 *   - GRID layout
 *   - ScrollView
 *   - nested focus rules
 *
 * Important:
 *   Children are non-owning and must outlive their container.
 */

int main()
{
    casio::GUI gui;

    casio::Label title(
        {8, 3, 380, 18},
        "Containers / layouts",
        casio::TextAlign::CENTER,
        true
    );

    casio::Panel verticalPanel(
        {8, 28, 118, 150},
        casio::LayoutMode::VERTICAL,
        5,
        4
    );

    verticalPanel.setContainerTitle(
        "Vertical"
    );

    casio::Checkbox vCheck1(
        {0, 0, 100, 22},
        "Option 1",
        false
    );

    casio::Checkbox vCheck2(
        {0, 0, 100, 22},
        "Option 2",
        true
    );

    casio::Slider vSlider(
        {0, 0, 100, 22},
        40,
        0,
        100,
        10
    );

    verticalPanel.addChild(
        &vCheck1,
        {0, 0, 100, 22}
    );

    verticalPanel.addChild(
        &vCheck2,
        {0, 0, 100, 22}
    );

    verticalPanel.addChild(
        &vSlider,
        {0, 0, 100, 22}
    );

    casio::Panel gridPanel(
        {136, 28, 118, 150},
        casio::LayoutMode::GRID,
        5,
        4
    );

    gridPanel.setContainerTitle(
        "Grid"
    );

    gridPanel.setGridColumns(2);

    casio::TextBox gridText1(
        {0, 0, 48, 26},
        "",
        "A",
        8
    );

    casio::TextBox gridText2(
        {0, 0, 48, 26},
        "",
        "B",
        8
    );

    casio::Checkbox gridCheck1(
        {0, 0, 48, 22},
        "C1",
        false
    );

    casio::Checkbox gridCheck2(
        {0, 0, 48, 22},
        "C2",
        false
    );

    gridPanel.addChild(
        &gridText1,
        {0, 0, 48, 26}
    );

    gridPanel.addChild(
        &gridText2,
        {0, 0, 48, 26}
    );

    gridPanel.addChild(
        &gridCheck1,
        {0, 0, 48, 22}
    );

    gridPanel.addChild(
        &gridCheck2,
        {0, 0, 48, 22}
    );

    casio::ScrollView scroll(
        {264, 28, 124, 150},
        casio::LayoutMode::VERTICAL,
        5,
        4,
        true
    );

    scroll.setContainerTitle(
        "Scroll"
    );

    scroll.setScrollStep(14);

    casio::Checkbox s1(
        {0, 0, 92, 22},
        "Item 1",
        false
    );

    casio::Checkbox s2(
        {0, 0, 92, 22},
        "Item 2",
        false
    );

    casio::TextBox s3(
        {0, 0, 92, 26},
        "",
        "Text",
        10
    );

    casio::Slider s4(
        {0, 0, 92, 22},
        50,
        0,
        100,
        10
    );

    casio::Checkbox s5(
        {0, 0, 92, 22},
        "Item 5",
        true
    );

    casio::Checkbox s6(
        {0, 0, 92, 22},
        "Item 6",
        false
    );

    scroll.addChild(&s1, {0, 0, 92, 22});
    scroll.addChild(&s2, {0, 0, 92, 22});
    scroll.addChild(&s3, {0, 0, 92, 26});
    scroll.addChild(&s4, {0, 0, 92, 22});
    scroll.addChild(&s5, {0, 0, 92, 22});
    scroll.addChild(&s6, {0, 0, 92, 22});

    casio::Label hint(
        {8, 184, 380, 18},
        "Checkbox: no focus | Text/Slider: focus | Scrollbar: scroll focus",
        casio::TextAlign::CENTER,
        false
    );

    gui.addItem(&title);
    gui.addItem(&verticalPanel);
    gui.addItem(&gridPanel);
    gui.addItem(&scroll);
    gui.addItem(&hint);

    gui.runGUI();
    return 0;
}
