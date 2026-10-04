#include "CASIO_GUI/casio.hpp"

int main()
{
    casio::GUI gui;

    casio::Theme theme =
        casio::makeTheme(
            casio::ThemePreset::LIGHT
        );

    gui.applyTheme(theme);

    casio::Button button;
    casio::ToggleButton toggle;
    casio::Checkbox checkbox;
    casio::TextBox textbox;
    casio::Numeric numeric;
    casio::Slider slider;
    casio::VerticalSlider vslider;

    casio::ProgressBar progress;
    casio::VerticalProgressBar vprogress;
    casio::ScrollBar scrollbar;

    casio::LED led;
    casio::ColorWheel wheel;
    casio::Gauge gauge;
    casio::Dial dial;

    casio::RadioGroup radios;
    casio::ListBox list;
    casio::ComboBox combo;
    casio::Tree tree;
    casio::Table table;
    casio::Graph graph;

    casio::Panel panel;
    casio::ScrollView scrollView;
    casio::TabView tabView;

    casio::Label label;
    casio::Separator separator;
    casio::Toolbar toolbar;
    casio::StatusBar status;

    casio::Canvas canvas;
    canvas.reserveCommands(16);

    canvas.addLine(
        {0, 0},
        {20, 20},
        casio::Color(0, 0, 0)
    );

    canvas.addCircle(
        {30, 30},
        10
    );

    casio::PromptPopup prompt;
    casio::OptionsPopup options;

    casio::IniFile ini;
    casio::ObjectFile objects;
    casio::GuiStateFile state;

    // Keep objects alive and silence unused warnings in strict host checks.
    (void)button;
    (void)toggle;
    (void)checkbox;
    (void)textbox;
    (void)numeric;
    (void)slider;
    (void)vslider;
    (void)progress;
    (void)vprogress;
    (void)scrollbar;
    (void)led;
    (void)wheel;
    (void)gauge;
    (void)dial;
    (void)radios;
    (void)list;
    (void)combo;
    (void)tree;
    (void)table;
    (void)graph;
    (void)panel;
    (void)scrollView;
    (void)tabView;
    (void)label;
    (void)separator;
    (void)toolbar;
    (void)status;
    (void)canvas;
    (void)prompt;
    (void)options;
    (void)ini;
    (void)objects;
    (void)state;

    return casio::GUI_API_LEVEL == 8 ? 0 : 1;
}
