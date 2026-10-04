#include "CASIO_GUI/casio.hpp"

static casio::Button* makeButton(
    casio::Position pos,
    const std::string& text,
    std::function<void()> callback)
{
    casio::ItemLabel labels{
        text, text, text, text, text, text
    };

    return new casio::Button(
        pos,
        casio::ItemEvent{
            casio::ItemEvent::eventType::KEY_DOWN,
            KEY_EXE,
            callback
        },
        casio::ItemStatus{
            false, false, false, false,
            false, true, false
        },
        casio::ItemColor{
            casio::RawItemColor(C_WHITE, C_BLACK, C_BLACK),
            casio::RawItemColor(C_BLACK, C_WHITE, C_WHITE),
            casio::RawItemColor(C_LIGHT, C_BLUE, C_BLACK),
            casio::RawItemColor(C_LIGHT, C_BLUE, C_BLACK),
            casio::RawItemColor(C_LIGHT, C_BLUE, C_BLACK),
            casio::RawItemColor(C_LIGHT, C_BLUE, C_BLACK),
            casio::RawItemColor(C_DARK, C_BLACK, C_DARK)
        },
        labels
    );
}

int main()
{
    casio::GUI gui;
    gui.setThemePreset(casio::ThemePreset::LIGHT);

    int pageBasics = gui.getCurrentPageId();
    int pageData = gui.createPage("Data", gui.getTheme().background.getRGB());
    int pageInstruments = gui.createPage("Instruments", gui.getTheme().background.getRGB());
    int pageLayouts = gui.createPage("Layouts", gui.getTheme().background.getRGB());
    int pageCanvas = gui.createPage("Canvas", gui.getTheme().background.getRGB());
    int pagePopups = gui.createPage("Popups", gui.getTheme().background.getRGB());

    auto* nav = new casio::SoftKeyBar(
        {"Basic", "Data", "Meter", "Layout", "Canvas", "Popup"},
        24, 100000
    );

    nav->setCallback(0, [&gui, pageBasics]() {gui.setPage(pageBasics);});
    nav->setCallback(1, [&gui, pageData]() {gui.setPage(pageData);});
    nav->setCallback(2, [&gui, pageInstruments]() {gui.setPage(pageInstruments);});
    nav->setCallback(3, [&gui, pageLayouts]() {gui.setPage(pageLayouts);});
    nav->setCallback(4, [&gui, pageCanvas]() {gui.setPage(pageCanvas);});
    nav->setCallback(5, [&gui, pagePopups]() {gui.setPage(pagePopups);});
    gui.addGlobalItem(nav);

    // Basic
    auto* basicTitle = new casio::Label(
        {10, 8, 250, 20}, "Basic controls", casio::TextAlign::LEFT);

    auto* text = new casio::TextBox(
        {10, 35, 155, 28}, "", "ALPHA text...", 24);

    auto* numeric = new casio::Numeric(
        {175, 35, 100, 28}, 25, 0, 100, 5);

    auto* check = new casio::Checkbox(
        {10, 72, 120, 24}, "Enabled", true);

    auto* toggle = new casio::ToggleButton(
        {140, 72, 110, 24});
    toggle->setOn(false);

    auto* slider = new casio::Slider(
        {10, 108, 180, 24}, 50, 0, 100, 5);

    auto* progress = new casio::ProgressBar(
        {205, 108, 150, 20}, 50);

    auto* led = new casio::LED(
        {285, 35, 95, 28}, casio::LEDState::ON, "RUN");

    slider->setOnChanged(
        [progress](int value)
        {
            progress->setValue(value);
        });

    gui.addItemToPage(pageBasics, basicTitle);
    gui.addItemToPage(pageBasics, text);
    gui.addItemToPage(pageBasics, numeric);
    gui.addItemToPage(pageBasics, check);
    gui.addItemToPage(pageBasics, toggle);
    gui.addItemToPage(pageBasics, slider);
    gui.addItemToPage(pageBasics, progress);
    gui.addItemToPage(pageBasics, led);

    // Data
    auto* radio = new casio::RadioGroup(
        {10, 10, 105, 82}, {"Auto", "Manual", "Test"}, 0);

    auto* list = new casio::ListBox(
        {125, 10, 120, 100},
        {"Channel A", "Channel B", "Channel C",
         "Channel D", "Channel E", "Channel F"});

    auto* combo = new casio::ComboBox(
        {255, 10, 125, 26}, {"9600", "19200", "115200"}, 2);

    auto* tree = new casio::Tree(
        {10, 118, 235, 72}, 17, 13, true, true);

    int root = tree->addRoot("Project", true);
    int src = tree->addNode(root, "src", true);
    tree->addNode(src, "main.cpp");
    tree->addNode(src, "gui.cpp");

    auto* table = new casio::Table();
    table->setGeometry({255, 48, 125, 142});

    gui.addItemToPage(pageData, radio);
    gui.addItemToPage(pageData, list);
    gui.addItemToPage(pageData, combo);
    gui.addItemToPage(pageData, tree);
    gui.addItemToPage(pageData, table);

    // Instruments
    auto* gauge = new casio::Gauge(
        {10, 10, 120, 105}, 35, 0, 100,
        135.0f, 405.0f, 10, 3, "%");

    gauge->addZone(0, 60, casio::Color(0, 190, 60));
    gauge->addZone(60, 80, casio::Color(240, 160, 20));
    gauge->addZone(80, 100, casio::Color(220, 40, 40));

    auto* dial = new casio::Dial(
        {140, 10, 120, 105}, 35, 0, 100, 5,
        135.0f, 405.0f, 10, 3, "%");

    dial->setOnChanged([gauge](int value) {gauge->setValue(value);});

    auto* wheel = new casio::ColorWheel(
        {275, 10, 100, 100}, 0.0f, 3.0f);

    auto* graph = new casio::Graph();
    graph->setGeometry({10, 120, 365, 70});

    gui.addItemToPage(pageInstruments, gauge);
    gui.addItemToPage(pageInstruments, dial);
    gui.addItemToPage(pageInstruments, wheel);
    gui.addItemToPage(pageInstruments, graph);

    // Layouts
    auto* tabs = new casio::TabView(
        {10, 10, 370, 180}, {"General", "Scroll", "Nested"}, 0);

    auto* generalPanel = new casio::Panel(
        {0, 0, 330, 120}, casio::LayoutMode::VERTICAL, 5, 4);
    generalPanel->setContainerTitle("Panel");

    auto* panelCheck = new casio::Checkbox(
        {0, 0, 220, 22}, "Use cached values", true);
    auto* panelSlider = new casio::Slider(
        {0, 0, 220, 22}, 40);

    generalPanel->addChild(panelCheck, {0, 0, 220, 22});
    generalPanel->addChild(panelSlider, {0, 0, 220, 22});

    tabs->addChildToTab(
        0, generalPanel, {5, 5, 340, 125}, {},
        casio::ANCHOR_LEFT | casio::ANCHOR_RIGHT |
        casio::ANCHOR_TOP | casio::ANCHOR_BOTTOM);

    auto* scroll = new casio::ScrollView(
        {0, 0, 340, 125}, casio::LayoutMode::VERTICAL, 5, 3);

    for(int i = 0; i < 10; ++i)
    {
        auto* row = new casio::Checkbox(
            {0, 0, 250, 22}, "Scrollable option", (i % 2) == 0);
        scroll->addChild(row, {0, 0, 250, 22});
    }

    tabs->addChildToTab(
        1, scroll, {5, 5, 340, 125}, {},
        casio::ANCHOR_LEFT | casio::ANCHOR_RIGHT |
        casio::ANCHOR_TOP | casio::ANCHOR_BOTTOM);

    auto* nested = new casio::GroupBox(
        {0, 0, 340, 125}, casio::LayoutMode::GRID, 5, 4);
    nested->setContainerTitle("Grid");
    nested->setGridColumns(2);

    for(int i = 0; i < 4; ++i)
    {
        auto* box = new casio::Checkbox(
            {0, 0, 120, 22}, "Grid item", i == 0);
        nested->addChild(box, {0, 0, 120, 22});
    }

    tabs->addChildToTab(
        2, nested, {5, 5, 340, 125}, {},
        casio::ANCHOR_LEFT | casio::ANCHOR_RIGHT |
        casio::ANCHOR_TOP | casio::ANCHOR_BOTTOM);

    gui.addItemToPage(pageLayouts, tabs);

    // Canvas
    auto* canvas = new casio::Canvas({10, 10, 370, 180}, true, true);
    canvas->reserveCommands(20);
    canvas->addText({8, 8}, "GUI Canvas", casio::Color(0, 0, 0), 10);

    canvas->addRectangle(
        {12, 35, 80, 45},
        casio::Color(220, 240, 255),
        casio::Color(0, 0, 0),
        casio::DrawMode::FILLED, 2, 1);

    int movingCircle = canvas->addCircle(
        {145, 58}, 28,
        casio::Color(255, 220, 100),
        casio::Color(0, 0, 0),
        casio::DrawMode::FILLED, 2, 3);

    canvas->addEllipse(
        {225, 58}, 38, 18,
        casio::Color(220, 255, 220),
        casio::Color(0, 0, 0),
        casio::DrawMode::FILLED, 1, 4);

    canvas->addPolyline(
        {{20, 140}, {60, 105}, {100, 145}, {145, 110}},
        casio::Color(180, 0, 180), 2, 5);

    canvas->addSector(
        {290, 125}, 40, 200.0f, 340.0f,
        casio::Color(255, 220, 220),
        casio::Color(160, 0, 0),
        2, 28, 6);

    canvas->setOnCanvasClick(
        [canvas, movingCircle](int x, int y)
        {
            casio::CanvasCommand* command = canvas->getCommand(movingCircle);
            if(command != nullptr)
                command->p1 = {x, y};
        });

    gui.addItemToPage(pageCanvas, canvas);

    // Popups
    auto* prompt = new casio::PromptPopup(
        "Prompt", "Enter a value:", "", 24);

    auto* numericPopup = new casio::NumericPopup(
        "Numeric", "Choose a value:",
        25.0, 0.0, 100.0, 1, 5.0);

    auto* options = new casio::OptionsPopup(
        "Options", "Display settings:");

    options->addCheckbox("Grid", true);
    options->addToggle("Cursor", true);
    options->addRadio("Linear", 1, true);
    options->addRadio("Logarithmic", 1, false);

    auto* openPrompt = makeButton(
        {20, 25, 110, 34}, "Prompt", [prompt]() {prompt->open();});

    auto* openNumeric = makeButton(
        {145, 25, 110, 34}, "Numeric",
        [numericPopup]() {numericPopup->open();});

    auto* openOptions = makeButton(
        {270, 25, 110, 34}, "Options", [options]() {options->open();});

    auto* popupInfo = new casio::Label(
        {20, 80, 360, 65},
        "EXIT closes modal before page navigation.",
        casio::TextAlign::LEFT);

    gui.addItemToPage(pagePopups, openPrompt);
    gui.addItemToPage(pagePopups, openNumeric);
    gui.addItemToPage(pagePopups, openOptions);
    gui.addItemToPage(pagePopups, popupInfo);

    gui.addGlobalItem(prompt);
    gui.addGlobalItem(numericPopup);
    gui.addGlobalItem(options);

    gui.setThemePreset(casio::ThemePreset::LIGHT);
    gui.runGUI();

    return 1;
}
