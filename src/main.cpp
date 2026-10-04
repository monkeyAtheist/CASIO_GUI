#include "CASIO_GUI/casio.hpp"

#include <string>
#include <vector>

// Optional fxconv asset used by the image validation page.
extern "C" bopti_image_t img_gui_test;



//==============================================================================
// Helpers
//==============================================================================

static casio::Button* makeButton(
    casio::Position pos,
    const std::string& text,
    std::function<void()> callback)
{
    casio::ItemLabel labels{
        text,
        text,
        text,
        text,
        text,
        text
    };

    return new casio::Button(
        pos,
        casio::ItemEvent{
            casio::ItemEvent::eventType::KEY_DOWN,
            KEY_EXE,
            callback
        },
        casio::ItemStatus{
            false,
            false,
            false,
            false,
            false,
            true,
            false
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


static bool runCoreSelfTests()
{
    bool ok = true;

    //--------------------------------------------------------------------------
    // EventTable / EventList
    //--------------------------------------------------------------------------

    casio::EventList events;
    bool eventCalled = false;

    ok = ok &&
        events.addEvent(
            casio::ItemEvent::eventType::KEY_DOWN,
            KEY_EXE,
            [&eventCalled]()
            {
                eventCalled = true;
            }
        );

    // Exact duplicate must be rejected.
    ok = ok &&
        !events.addEvent(
            casio::ItemEvent::eventType::KEY_DOWN,
            KEY_EXE,
            [](){}
        );

    ok = ok &&
        events.updateEvent(
            casio::ItemEvent::eventType::KEY_DOWN,
            KEY_EXE
        );

    ok = ok && eventCalled;


    //--------------------------------------------------------------------------
    // Transform2D
    //--------------------------------------------------------------------------

    casio::Transform2D transform;

    transform
        .translate(10.0, 20.0)
        .scale(2.0, 2.0);

    casio::Point transformed =
        transform.apply(
            casio::Point{1, 1}
        );

    ok = ok &&
        transformed.x == 12 &&
        transformed.y == 22;


    //--------------------------------------------------------------------------
    // IniFile / ConfigFile / SaveFile
    //--------------------------------------------------------------------------

    casio::IniFile ini;

    ini.setString(
        "APP",
        "name",
        "GUI"
    );

    ini.setInt(
        "APP",
        "value",
        42
    );

    ini.setBool(
        "APP",
        "enabled",
        true
    );

    std::string encodedIni =
        ini.serialize();

    casio::ConfigFile restoredIni;

    ok = ok &&
        restoredIni.parse(
            encodedIni
        );

    ok = ok &&
        restoredIni.getStringOr(
            "APP",
            "name",
            ""
        ) == "GUI";

    ok = ok &&
        restoredIni.getIntOr(
            "APP",
            "value",
            0
        ) == 42;

    ok = ok &&
        restoredIni.getBoolOr(
            "APP",
            "enabled",
            false
        );

    // SaveFile is an alias of IniFile; instantiate it to validate the public API.
    casio::SaveFile saveAlias;
    saveAlias.setInt(
        "TEST",
        "value",
        7
    );

    ok = ok &&
        saveAlias.getIntOr(
            "TEST",
            "value",
            0
        ) == 7;


    //--------------------------------------------------------------------------
    // DataFile
    //--------------------------------------------------------------------------

    casio::DataFile data;

    data.setString(
        "device",
        "GRAPH90E"
    );

    data.setInt(
        "counter",
        123
    );

    data.setBool(
        "ready",
        true
    );

    casio::DataFile restoredData;

    ok = ok &&
        restoredData.parse(
            data.serialize()
        );

    ok = ok &&
        restoredData.getStringOr(
            "device",
            ""
        ) == "GRAPH90E";

    ok = ok &&
        restoredData.getIntOr(
            "counter",
            0
        ) == 123;

    ok = ok &&
        restoredData.getBoolOr(
            "ready",
            false
        );


    //--------------------------------------------------------------------------
    // NamedObjectFile / ObjectFile / ObjectSerializer
    //--------------------------------------------------------------------------

    casio::ObjectFile objects;

    ok = ok &&
        objects.beginObject(
            "motor1",
            "MotorState",
            1
        );

    ok = ok &&
        objects.set(
            "speed",
            1200
        );

    ok = ok &&
        objects.set(
            "enabled",
            true
        );

    ok = ok &&
        objects.set(
            "position",
            casio::Point{12, 34}
        );

    objects.endObject();

    casio::ObjectSerializer restoredObjects;

    ok = ok &&
        restoredObjects.parse(
            objects.serialize()
        );

    ok = ok &&
        restoredObjects.selectObject(
            "motor1"
        );

    ok = ok &&
        restoredObjects.getIntOr(
            "speed",
            0
        ) == 1200;

    ok = ok &&
        restoredObjects.getBoolOr(
            "enabled",
            false
        );

    casio::Point objectPosition{0, 0};

    ok = ok &&
        restoredObjects.get(
            "position",
            objectPosition
        );

    ok = ok &&
        objectPosition.x == 12 &&
        objectPosition.y == 34;


    //--------------------------------------------------------------------------
    // GuiStateFile
    //--------------------------------------------------------------------------

    casio::Checkbox sourceCheckbox(
        {0, 0, 100, 22},
        "State",
        true
    );

    casio::Checkbox restoredCheckbox(
        {0, 0, 100, 22},
        "State",
        false
    );

    casio::GuiStateFile state;

    state.capture(
        "checkbox",
        sourceCheckbox
    );

    std::string stateText =
        state.ini().serialize();

    casio::GuiStateFile restoredState;

    ok = ok &&
        restoredState.ini().parse(
            stateText
        );

    restoredState.apply(
        "checkbox",
        restoredCheckbox
    );

    ok = ok &&
        restoredCheckbox.isChecked();


    return ok;
}


//==============================================================================
// Main validation catalogue
//==============================================================================

int main()
{
    casio::GUI gui;

    gui.cursor.setStyle(
        casio::Cursor::cursorStyle::CROSS
    );

    gui.cursor.setSpeed(6);

    gui.setThemePreset(
        casio::defaultThemePreset()
    );


    //--------------------------------------------------------------------------
    // Top-level pages
    //--------------------------------------------------------------------------

    const int pageControls =
        gui.getCurrentPageId();

    const int pageShapes =
        gui.createPage(
            "Shapes",
            gui.getTheme().background.getRGB()
        );

    const int pageData =
        gui.createPage(
            "Data",
            gui.getTheme().background.getRGB()
        );

    const int pageInstruments =
        gui.createPage(
            "Instruments",
            gui.getTheme().background.getRGB()
        );

    const int pageLayouts =
        gui.createPage(
            "Layouts",
            gui.getTheme().background.getRGB()
        );

    const int pageSystem =
        gui.createPage(
            "System",
            gui.getTheme().background.getRGB()
        );


    //--------------------------------------------------------------------------
    // Global status + global F1..F6 navigation
    //--------------------------------------------------------------------------

    auto* status =
        new casio::StatusBar(
            {
                0,
                DHEIGHT - 46,
                DWIDTH,
                20
            },
            "Phase 11",
            "runtime stable",
            "tick",
            900
        );

    status->setStatus(
        casio::LEDState::ON
    );

    gui.addGlobalItem(status);

    // OPTN -> screenshot naming popup (default name: image).
    gui.enableScreenshotCapture(
        KEY_OPTN,
        "image"
    );

    gui.setScreenshotResultCallback(
        [status](
            bool success,
            const std::string& path)
        {
            if(success)
            {
                status->setLeftText(
                    std::string("Saved: ") +
                    path
                );
            }
            else
            {
                status->setLeftText(
                    "Screenshot: ERROR"
                );
            }
        }
    );


    auto* navigation =
        new casio::SoftKeyBar(
            {
                "Control",
                "Shape",
                "Data",
                "Meter",
                "Layout",
                "System"
            },
            24,
            1000
        );

    navigation->setCallback(
        0,
        [&gui, pageControls]()
        {
            gui.setPage(pageControls);
        }
    );

    navigation->setCallback(
        1,
        [&gui, pageShapes]()
        {
            gui.setPage(pageShapes);
        }
    );

    navigation->setCallback(
        2,
        [&gui, pageData]()
        {
            gui.setPage(pageData);
        }
    );

    navigation->setCallback(
        3,
        [&gui, pageInstruments]()
        {
            gui.setPage(pageInstruments);
        }
    );

    navigation->setCallback(
        4,
        [&gui, pageLayouts]()
        {
            gui.setPage(pageLayouts);
        }
    );

    navigation->setCallback(
        5,
        [&gui, pageSystem]()
        {
            gui.setPage(pageSystem);
        }
    );

    gui.addGlobalItem(navigation);


    //--------------------------------------------------------------------------
    // Invisible timer / TimeItem / TimerItem
    //--------------------------------------------------------------------------

    auto* timer =
        new casio::TimerItem(
            1000,
            [status]()
            {
                static bool tick = false;

                tick = !tick;

                status->setRightText(
                    tick
                        ? "tick"
                        : "tock"
                );
            },
            true,
            true
        );

    gui.addGlobalItem(timer);


    //==========================================================================
    // F1 — Controls
    //==========================================================================

    auto* controlsTitle =
        new casio::Label(
            {8, 4, 200, 18},
            "Controls / menu / text",
            casio::TextAlign::LEFT
        );

    gui.addItemToPage(
        pageControls,
        controlsTitle
    );


    auto* menu =
        new casio::MenuBar(
            {8, 24, 380, 22},
            {
                "File",
                "Edit",
                "View"
            },
            0,
            20
        );

    menu->setCallback(
        0,
        [status]()
        {
            status->setLeftText(
                "Menu: File"
            );
        }
    );

    menu->setCallback(
        1,
        [status]()
        {
            status->setLeftText(
                "Menu: Edit"
            );
        }
    );

    menu->setCallback(
        2,
        [status]()
        {
            status->setLeftText(
                "Menu: View"
            );
        }
    );

    gui.addItemToPage(
        pageControls,
        menu
    );


    auto* pushButton =
        makeButton(
            {8, 54, 80, 28},
            "Button",
            [status]()
            {
                status->setLeftText(
                    "Button OK"
                );
            }
        );

    gui.addItemToPage(
        pageControls,
        pushButton
    );


    auto* toggle =
        new casio::ToggleButton(
            {96, 54, 80, 28}
        );

    toggle->setOn(false);

    gui.addItemToPage(
        pageControls,
        toggle
    );


    auto* checkbox =
        new casio::Checkbox(
            {184, 54, 100, 28},
            "Checkbox",
            true
        );

    gui.addItemToPage(
        pageControls,
        checkbox
    );


    auto* standaloneRadio =
        new casio::RadioButton(
            {292, 54, 94, 28},
            "Radio",
            false
        );

    standaloneRadio->setAllowUncheck(
        true
    );

    gui.addItemToPage(
        pageControls,
        standaloneRadio
    );


    auto* textbox =
        new casio::TextBox(
            {8, 90, 150, 28},
            "",
            "ALPHA abc/123",
            24,
            0,
            casio::TextBoxInputMode::MIXED
        );

    textbox->setOnTextChanged(
        [status](const std::string& text)
        {
            status->setCenterText(
                text.empty()
                    ? "TextBox"
                    : text
            );
        }
    );

    gui.addItemToPage(
        pageControls,
        textbox
    );


    auto* numeric =
        new casio::Numeric(
            {166, 90, 90, 28},
            25,
            0,
            100,
            5
        );

    gui.addItemToPage(
        pageControls,
        numeric
    );


    auto* simpleTab =
        new casio::Tab(
            {264, 90, 122, 28},
            {
                "A",
                "B",
                "C"
            },
            0
        );

    simpleTab->setOnTabChanged(
        [status](int index)
        {
            status->setLeftText(
                index == 0
                    ? "Tab A"
                    : (
                        index == 1
                            ? "Tab B"
                            : "Tab C"
                    )
            );
        }
    );

    gui.addItemToPage(
        pageControls,
        simpleTab
    );


    auto* radioGroup =
        new casio::RadioGroup(
            {8, 126, 120, 46},
            {
                "Auto",
                "Manual"
            },
            0,
            22
        );

    gui.addItemToPage(
        pageControls,
        radioGroup
    );


    auto* separator =
        new casio::Separator(
            {140, 137, 110, 4},
            casio::Orientation::HORIZONTAL,
            1
        );

    gui.addItemToPage(
        pageControls,
        separator
    );


    auto* centeredLabel =
        new casio::Label(
            {258, 126, 128, 26},
            "Label + Separator",
            casio::TextAlign::CENTER,
            true
        );

    gui.addItemToPage(
        pageControls,
        centeredLabel
    );


    //==========================================================================
    // F2 — All drawing primitives
    //==========================================================================

    gui.addItemToPage(
        pageShapes,
        new casio::Label(
            {8, 3, 250, 18},
            "All primitive item classes",
            casio::TextAlign::LEFT
        )
    );


    auto* rectangle =
        new casio::Rectangle(
            {8, 26, 42, 28},
            casio::Color(220, 235, 255),
            casio::Color(0, 0, 0),
            casio::DrawMode::FILLED,
            1
        );

    gui.addItemToPage(
        pageShapes,
        rectangle
    );


    auto* square =
        new casio::Square(
            58,
            26,
            28,
            casio::Color(240, 220, 255),
            casio::Color(0, 0, 0)
        );

    gui.addItemToPage(
        pageShapes,
        square
    );


    auto* circle =
        new casio::Circle(
            108,
            40,
            14,
            casio::Color(255, 225, 160),
            casio::Color(0, 0, 0)
        );

    gui.addItemToPage(
        pageShapes,
        circle
    );


    auto* triangle =
        new casio::Triangle(
            {136, 54},
            {168, 54},
            {152, 25},
            casio::Color(220, 255, 220),
            casio::Color(0, 0, 0)
        );

    casio::Transform2D triangleTransform;

    triangleTransform.rotate(
        7.0,
        152.0,
        40.0
    );

    triangle->applyTransform(
        triangleTransform
    );

    gui.addItemToPage(
        pageShapes,
        triangle
    );


    auto* rightTriangle =
        new casio::RightTriangle(
            {180, 26, 38, 28},
            casio::RightTriangleCorner::TOP_LEFT,
            casio::Color(255, 225, 225),
            casio::Color(0, 0, 0)
        );

    gui.addItemToPage(
        pageShapes,
        rightTriangle
    );


    auto* trapezoid =
        new casio::Trapezoid(
            {226, 26, 48, 28},
            28,
            48,
            casio::Color(225, 255, 245),
            casio::Color(0, 0, 0)
        );

    gui.addItemToPage(
        pageShapes,
        trapezoid
    );


    auto* regular =
        new casio::RegularPolygon(
            302,
            40,
            16,
            6,
            -90.0f,
            casio::Color(235, 235, 255),
            casio::Color(0, 0, 0)
        );

    gui.addItemToPage(
        pageShapes,
        regular
    );


    auto* ellipse =
        new casio::Ellipse(
            355,
            40,
            24,
            13,
            casio::Color(225, 245, 255),
            casio::Color(0, 0, 0)
        );

    gui.addItemToPage(
        pageShapes,
        ellipse
    );


    auto* line =
        new casio::Line(
            {8, 78},
            {48, 112},
            casio::Color(0, 0, 180),
            2
        );

    line->translate(
        2,
        0
    );

    gui.addItemToPage(
        pageShapes,
        line
    );


    auto* polyline =
        new casio::Polyline(
            {
                {60, 110},
                {72, 78},
                {88, 112},
                {102, 82}
            },
            casio::Color(180, 0, 180),
            2
        );

    gui.addItemToPage(
        pageShapes,
        polyline
    );


    auto* arc =
        new casio::Arc(
            132,
            96,
            22,
            200.0f,
            340.0f,
            casio::Color(200, 30, 30),
            2,
            28
        );

    gui.addItemToPage(
        pageShapes,
        arc
    );


    auto* sector =
        new casio::Sector(
            188,
            96,
            22,
            -55.0f,
            55.0f,
            casio::Color(255, 220, 220),
            casio::Color(150, 0, 0),
            casio::DrawMode::FILLED,
            1,
            24
        );

    gui.addItemToPage(
        pageShapes,
        sector
    );


    auto* pie =
        new casio::Pie(
            244,
            96,
            22,
            25.0f,
            150.0f,
            casio::Color(220, 255, 225),
            casio::Color(0, 110, 20),
            casio::DrawMode::FILLED,
            1,
            24
        );

    gui.addItemToPage(
        pageShapes,
        pie
    );


    auto* rounded =
        new casio::RoundedRectangle(
            {278, 78, 52, 36},
            8,
            casio::Color(245, 235, 215),
            casio::Color(0, 0, 0)
        );

    gui.addItemToPage(
        pageShapes,
        rounded
    );


    auto* polygon =
        new casio::Polygon(
            {
                {344, 80},
                {381, 84},
                {374, 112},
                {348, 116},
                {338, 98}
            },
            casio::Color(235, 220, 255),
            casio::Color(70, 0, 100)
        );

    gui.addItemToPage(
        pageShapes,
        polygon
    );


    gui.addItemToPage(
        pageShapes,
        new casio::Label(
            {8, 130, 380, 32},
            "Primitive item classes",
            casio::TextAlign::LEFT
        )
    );


    //==========================================================================
    // F3 — Data / Graph / Table / Tree / Lists
    //==========================================================================

    auto* list =
        new casio::ListBox(
            {8, 8, 102, 74},
            {
                "Channel A",
                "Channel B",
                "Channel C",
                "Channel D",
                "Channel E"
            },
            0,
            18,
            true
        );

    list->setOnSelected(
        [status](
            int index,
            const std::string& value)
        {
            (void)index;

            status->setLeftText(
                std::string("Channel: ") +
                value
            );
        }
    );

    list->setOnActivated(
        [status](
            int index,
            const std::string& value)
        {
            (void)index;

            status->setCenterText(
                std::string("EXE: ") +
                value
            );
        }
    );

    gui.addItemToPage(
        pageData,
        list
    );


    auto* combo =
        new casio::ComboBox(
            {118, 8, 116, 24},
            {
                "9600",
                "19200",
                "115200"
            },
            2,
            3,
            20
        );

    combo->setOnChanged(
        [status](
            int index,
            const std::string& value)
        {
            (void)index;

            status->setLeftText(
                value
            );
        }
    );

    gui.addItemToPage(
        pageData,
        combo
    );


    auto* tree =
        new casio::Tree(
            {8, 90, 170, 78},
            17,
            13,
            true,
            true
        );

    int project =
        tree->addRoot(
            "Project",
            true
        );

    int src =
        tree->addNode(
            project,
            "src",
            true
        );

    tree->addNode(
        src,
        "main.cpp"
    );

    tree->addNode(
        src,
        "gui.cpp"
    );

    int assets =
        tree->addNode(
            project,
            "assets",
            false
        );

    tree->addNode(
        assets,
        "icon.png"
    );

    tree->setOnSelected(
        [status](
            int id,
            const std::string& label)
        {
            (void)id;

            status->setLeftText(
                std::string("Tree: ") +
                label
            );
        }
    );

    tree->setOnActivated(
        [status](
            int id,
            const std::string& label)
        {
            (void)id;

            status->setCenterText(
                std::string("EXE: ") +
                label
            );
        }
    );

    gui.addItemToPage(
        pageData,
        tree
    );


    auto* dataView =
        new casio::TabView(
            {182, 52, 206, 116},
            {
                "Table",
                "Graph"
            },
            0,
            20,
            4
        );


    auto* table =
        new casio::Table(
            {0, 0, 188, 82},
            4,
            3,
            54,
            19,
            true
        );

    table->setColumnLabel(0, "A");
    table->setColumnLabel(1, "B");
    table->setColumnLabel(2, "C");
    table->setCell(0, 0, "10");
    table->setCell(0, 1, "20");
    table->setCell(1, 0, "30");
    table->setCell(1, 1, "40");

    dataView->addChildToTab(
        0,
        table,
        {3, 3, 188, 82},
        {},
        casio::ANCHOR_LEFT |
        casio::ANCHOR_RIGHT |
        casio::ANCHOR_TOP |
        casio::ANCHOR_BOTTOM
    );


    auto* graph =
        new casio::Graph(
            {0, 0, 188, 82},
            casio::GraphAxis{
                -5.0,
                5.0,
                casio::GraphScale::LINEAR,
                5,
                true,
                true,
                "X"
            },
            casio::GraphAxis{
                -2.0,
                10.0,
                casio::GraphScale::LINEAR,
                5,
                true,
                true,
                "Y"
            }
        );

    int series =
        graph->addSeries(
            casio::Color(0, 70, 200),
            true,
            true
        );

    graph->addPoint(static_cast<unsigned int>(series), -4.0, 1.0);
    graph->addPoint(static_cast<unsigned int>(series), -2.0, 4.0);
    graph->addPoint(static_cast<unsigned int>(series),  0.0, 2.0);
    graph->addPoint(static_cast<unsigned int>(series),  2.0, 7.0);
    graph->addPoint(static_cast<unsigned int>(series),  4.0, 5.0);

    graph->addCursor(
        0.0,
        2.0,
        casio::Color(220, 0, 0),
        "C1",
        casio::GraphCursorStyle::CROSSHAIR
    );

    dataView->addChildToTab(
        1,
        graph,
        {3, 3, 188, 82},
        {},
        casio::ANCHOR_LEFT |
        casio::ANCHOR_RIGHT |
        casio::ANCHOR_TOP |
        casio::ANCHOR_BOTTOM
    );

    gui.addItemToPage(
        pageData,
        dataView
    );


    auto* dataRadio =
        new casio::RadioButton(
            {244, 8, 68, 24},
            "One",
            false
        );

    dataRadio->setAllowUncheck(
        true
    );

    dataRadio->setOnChanged(
        [status](bool checked)
        {
            status->setLeftText(
                checked
                    ? "Radio One: ON"
                    : "Radio One: OFF"
            );
        }
    );

    gui.addItemToPage(
        pageData,
        dataRadio
    );


    // A RadioGroup intentionally takes keyboard focus; A/B makes the behavior
    // visible and useful instead of trapping focus on a single entry.
    auto* dataRadioGroup =
        new casio::RadioGroup(
            {318, 4, 70, 42},
            {
                "A",
                "B"
            },
            0,
            20
        );

    dataRadioGroup->setOnChanged(
        [status](
            int index,
            const std::string& value)
        {
            (void)index;

            status->setCenterText(
                std::string("Radio: ") +
                value
            );
        }
    );

    gui.addItemToPage(
        pageData,
        dataRadioGroup
    );


    //==========================================================================
    // F4 — Instruments / orientation controls
    //==========================================================================

    auto* gauge =
        new casio::Gauge(
            {4, 4, 104, 92},
            45,
            0,
            100,
            135.0f,
            405.0f,
            10,
            3,
            "%"
        );

    gauge->addZone(
        0,
        60,
        casio::Color(0, 180, 60)
    );

    gauge->addZone(
        60,
        80,
        casio::Color(240, 160, 20)
    );

    gauge->addZone(
        80,
        100,
        casio::Color(220, 40, 40)
    );

    gui.addItemToPage(
        pageInstruments,
        gauge
    );


    auto* dial =
        new casio::Dial(
            {110, 4, 104, 92},
            45,
            0,
            100,
            5,
            135.0f,
            405.0f,
            10,
            3,
            "%"
        );

    gui.addItemToPage(
        pageInstruments,
        dial
    );


    auto* wheel =
        new casio::ColorWheel(
            {220, 4, 88, 88},
            210.0f,
            3.0f
        );

    auto* hueValue =
        new casio::Numeric(
            {220, 94, 88, 26},
            210,
            0,
            359,
            1
        );

    auto* colorLed =
        new casio::LED(
            {316, 90, 70, 28},
            casio::LEDState::ON,
            "COLOR",
            casio::LEDShape::CIRCLE
        );

    colorLed->setOnColor(
        wheel->getColor()
    );

    wheel->setOnChanged(
        [status, hueValue, colorLed](
            float hue,
            casio::Color color)
        {
            int hueInt =
                static_cast<int>(
                    hue + 0.5f
                );

            if(hueInt >= 360)
                hueInt = 0;

            hueValue->setValue(
                hueInt
            );

            colorLed->setOnColor(
                color
            );

            colorLed->setState(
                casio::LEDState::ON
            );

            status->setLeftText(
                std::string("Hue: ") +
                casio::storage::formatInt(hueInt)
            );
        }
    );

    hueValue->setOnChanged(
        [wheel, status](int hue)
        {
            wheel->setHue(
                static_cast<float>(hue)
            );

            status->setLeftText(
                std::string("Hue set: ") +
                casio::storage::formatInt(hue)
            );
        }
    );

    gui.addItemToPage(
        pageInstruments,
        wheel
    );

    gui.addItemToPage(
        pageInstruments,
        hueValue
    );

    gui.addItemToPage(
        pageInstruments,
        colorLed
    );


    auto* ledOn =
        new casio::LED(
            {316, 4, 70, 20},
            casio::LEDState::ON,
            "ON",
            casio::LEDShape::CIRCLE
        );

    gui.addItemToPage(
        pageInstruments,
        ledOn
    );


    auto* ledWarning =
        new casio::LED(
            {316, 28, 70, 20},
            casio::LEDState::WARNING,
            "WARN",
            casio::LEDShape::RECTANGLE
        );

    ledWarning->setBlink(
        true,
        600
    );

    gui.addItemToPage(
        pageInstruments,
        ledWarning
    );


    auto* ledError =
        new casio::LED(
            {316, 52, 70, 20},
            casio::LEDState::ERROR,
            "ERR"
        );

    gui.addItemToPage(
        pageInstruments,
        ledError
    );


    auto* hSlider =
        new casio::HorizontalSlider(
            {8, 106, 150, 20},
            40,
            0,
            100,
            5
        );

    gui.addItemToPage(
        pageInstruments,
        hSlider
    );

    gui.addItemToPage(
        pageInstruments,
        new casio::Label(
            {8, 126, 150, 12},
            "Slider -> Dial/Gauge",
            casio::TextAlign::LEFT
        )
    );


    auto* vSlider =
        new casio::VerticalSlider(
            {166, 100, 22, 66},
            60,
            0,
            100,
            5
        );

    gui.addItemToPage(
        pageInstruments,
        vSlider
    );


    auto* hProgress =
        new casio::HorizontalProgressBar(
            {198, 132, 124, 18},
            40,
            0,
            100
        );

    gui.addItemToPage(
        pageInstruments,
        hProgress
    );


    auto* vProgress =
        new casio::VerticalProgressBar(
            {330, 122, 18, 44},
            60,
            0,
            100,
            false
        );

    gui.addItemToPage(
        pageInstruments,
        vProgress
    );


    auto* hScroll =
        new casio::HorizontalScrollBar(
            {8, 146, 150, 16},
            30,
            0,
            100,
            20,
            5
        );

    gui.addItemToPage(
        pageInstruments,
        hScroll
    );


    auto* vScroll =
        new casio::VerticalScrollBar(
            {360, 122, 16, 44},
            20,
            0,
            100,
            20,
            5
        );

    gui.addItemToPage(
        pageInstruments,
        vScroll
    );


    hSlider->setOnChanged(
        [hProgress, dial, gauge, status](int value)
        {
            hProgress->setValue(value);
            dial->setValue(value);
            gauge->setValue(value);

            status->setCenterText(
                std::string("Dial/Gauge: ") +
                casio::storage::formatInt(value)
            );
        }
    );

    dial->setOnChanged(
        [hSlider, hProgress, gauge, status](int value)
        {
            hSlider->setValue(value);
            hProgress->setValue(value);
            gauge->setValue(value);

            status->setCenterText(
                std::string("Dial/Gauge: ") +
                casio::storage::formatInt(value)
            );
        }
    );

    vSlider->setOnChanged(
        [vProgress](int value)
        {
            vProgress->setValue(
                value
            );
        }
    );


    //==========================================================================
    // F5 — Layouts / media / Canvas
    //==========================================================================

    auto* toolbar =
        new casio::Toolbar(
            {4, 4, 388, 24},
            50
        );

    toolbar->addAction(
        "Light",
        [&gui]()
        {
            gui.setThemePreset(
                casio::ThemePreset::LIGHT
            );
        }
    );

    toolbar->addAction(
        "Dark",
        [&gui]()
        {
            gui.setThemePreset(
                casio::ThemePreset::DARK
            );
        }
    );

    toolbar->addAction(
        "HC",
        [&gui]()
        {
            gui.setThemePreset(
                casio::ThemePreset::HIGH_CONTRAST
            );
        }
    );

    gui.addItemToPage(
        pageLayouts,
        toolbar
    );


    auto* tabView =
        new casio::TabView(
            {4, 34, 260, 136},
            {
                "Panel",
                "Scroll",
                "Grid"
            },
            0,
            22,
            5
        );


    auto* panel =
        new casio::Panel(
            {0, 0, 220, 90},
            casio::LayoutMode::VERTICAL,
            5,
            4
        );

    panel->setContainerTitle(
        "Panel"
    );

    auto* panelLabel =
        new casio::Label(
            {0, 0, 180, 20},
            "Nested controls"
        );

    auto* panelCheck =
        new casio::Checkbox(
            {0, 0, 180, 22},
            "Inside panel",
            true
        );

    auto* panelSlider =
        new casio::Slider(
            {0, 0, 180, 20},
            35
        );

    panel->addChild(
        panelLabel,
        {0, 0, 180, 20}
    );

    panel->addChild(
        panelCheck,
        {0, 0, 180, 22}
    );

    panel->addChild(
        panelSlider,
        {0, 0, 180, 20}
    );

    tabView->addChildToTab(
        0,
        panel,
        {4, 4, 235, 100},
        {},
        casio::ANCHOR_LEFT |
        casio::ANCHOR_RIGHT |
        casio::ANCHOR_TOP |
        casio::ANCHOR_BOTTOM
    );


    auto* scrollView =
        new casio::ScrollView(
            {0, 0, 235, 100},
            casio::LayoutMode::VERTICAL,
            5,
            3,
            true
        );

    for(int i = 0;
        i < 8;
        ++i)
    {
        auto* item =
            new casio::Checkbox(
                {0, 0, 190, 20},
                "Scroll item",
                (i % 2) == 0
            );

        scrollView->addChild(
            item,
            {0, 0, 190, 20}
        );
    }

    tabView->addChildToTab(
        1,
        scrollView,
        {4, 4, 235, 100},
        {},
        casio::ANCHOR_LEFT |
        casio::ANCHOR_RIGHT |
        casio::ANCHOR_TOP |
        casio::ANCHOR_BOTTOM
    );


    auto* groupBox =
        new casio::GroupBox(
            {0, 0, 235, 100},
            casio::LayoutMode::GRID,
            5,
            3
        );

    groupBox->setContainerTitle(
        "GroupBox / Grid"
    );

    groupBox->setGridColumns(
        2
    );

    for(int i = 0;
        i < 4;
        ++i)
    {
        auto* gridItem =
            new casio::Checkbox(
                {0, 0, 90, 20},
                "Grid",
                i == 0
            );

        groupBox->addChild(
            gridItem,
            {0, 0, 90, 20}
        );
    }

    tabView->addChildToTab(
        2,
        groupBox,
        {4, 4, 235, 100},
        {},
        casio::ANCHOR_LEFT |
        casio::ANCHOR_RIGHT |
        casio::ANCHOR_TOP |
        casio::ANCHOR_BOTTOM
    );

    gui.addItemToPage(
        pageLayouts,
        tabView
    );


    // Explicit Container alias test (same implementation as Panel/GroupBox).
    auto* absoluteContainer =
        new casio::Container(
            {270, 34, 118, 38},
            casio::LayoutMode::ABSOLUTE,
            3,
            2
        );

    auto* containerLabel =
        new casio::Label(
            {2, 2, 106, 18},
            "Container",
            casio::TextAlign::CENTER,
            true
        );

    absoluteContainer->addChild(
        containerLabel,
        {2, 2, 100, 18},
        {},
        casio::ANCHOR_LEFT |
        casio::ANCHOR_RIGHT |
        casio::ANCHOR_TOP
    );

    gui.addItemToPage(
        pageLayouts,
        absoluteContainer
    );


    bopti_image_t const* validationImage =
        &img_gui_test;

    auto* imageAssetStatus =
        new casio::Label(
            {270, 74, 118, 12},
            "Image asset: OK",
            casio::TextAlign::CENTER
        );

    gui.addItemToPage(
        pageLayouts,
        imageAssetStatus
    );

    // If the status above says MISSING, only the two themed image-control
    // borders are expected. With the fxconv asset linked, both show the image.
    auto* imageItem =
        new casio::ImageItem(
            {270, 88, 50, 30},
            validationImage,
            0,
            0,
            true,
            casio::Color(0, 0, 0),
            1,
            casio::ImageMode::FIT
        );

    imageItem->setUseThemeBorder(true);

    gui.addItemToPage(
        pageLayouts,
        imageItem
    );


    auto* imageButton =
        new casio::ImageButton(
            {328, 88, 58, 30},
            validationImage,
            [status]()
            {
                status->setLeftText(
                    "ImageButton"
                );
            },
            0,
            0,
            true,
            1,
            casio::ImageMode::CENTER
        );

    imageButton->setUseThemeBorder(true);

    gui.addItemToPage(
        pageLayouts,
        imageButton
    );


    auto* canvas =
        new casio::Canvas(
            {270, 124, 116, 46},
            true,
            true,
            1
        );

    canvas->reserveCommands(
        12
    );

    canvas->addText(
        {4, 2},
        "Canvas",
        gui.getTheme().text,
        10
    );

    canvas->addLine(
        {5, 8},
        {105, 34},
        casio::Color(0, 80, 200),
        2,
        1
    );

    canvas->addCircle(
        {35, 24},
        12,
        casio::Color(255, 220, 100),
        casio::Color(0, 0, 0),
        casio::DrawMode::FILLED,
        1,
        2
    );

    canvas->addRectangle(
        {65, 8, 38, 28},
        casio::Color(220, 255, 225),
        casio::Color(0, 0, 0),
        casio::DrawMode::FILLED,
        1,
        3
    );

    gui.addItemToPage(
        pageLayouts,
        canvas
    );


    //==========================================================================
    // F6 — Popups / storage / system APIs
    //==========================================================================

    const bool selfTestsOk =
        runCoreSelfTests();


    auto* selfTestLabel =
        new casio::Label(
            {8, 5, 380, 20},
            selfTestsOk
                ? "Core self-tests: PASS"
                : "Core self-tests: FAIL",
            casio::TextAlign::CENTER,
            true
        );

    gui.addItemToPage(
        pageSystem,
        selfTestLabel
    );


    auto* genericPopup =
        new casio::Popup(
            "Popup",
            "Generic modal popup",
            250,
            120,
            false
        );

    auto* promptPopup =
        new casio::PromptPopup(
            "Prompt",
            "ALPHA / SHIFT input:",
            "",
            24
        );

    auto* boolPopup =
        new casio::BoolPopup(
            "Confirm",
            "Continue?",
            {
                "Yes",
                "No"
            }
        );

    auto* numericPopup =
        new casio::NumericPopup(
            "Numeric",
            "Choose value:",
            25.0,
            0.0,
            100.0,
            2,
            5.0
        );

    auto* optionsPopup =
        new casio::OptionsPopup(
            "Options",
            "Checkbox / Toggle / Radio"
        );

    optionsPopup->addCheckbox(
        "Grid",
        true
    );

    optionsPopup->addToggle(
        "Cursor",
        true
    );

    optionsPopup->addRadio(
        "Linear",
        1,
        true
    );

    optionsPopup->addRadio(
        "Logarithmic",
        1,
        false
    );


    gui.addGlobalItem(
        genericPopup
    );

    gui.addGlobalItem(
        promptPopup
    );

    gui.addGlobalItem(
        boolPopup
    );

    gui.addGlobalItem(
        numericPopup
    );

    gui.addGlobalItem(
        optionsPopup
    );


    gui.addItemToPage(
        pageSystem,
        makeButton(
            {8, 34, 72, 28},
            "Popup",
            [genericPopup]()
            {
                genericPopup->open();
            }
        )
    );


    gui.addItemToPage(
        pageSystem,
        makeButton(
            {84, 34, 72, 28},
            "Prompt",
            [promptPopup]()
            {
                promptPopup->open();
            }
        )
    );


    gui.addItemToPage(
        pageSystem,
        makeButton(
            {160, 34, 72, 28},
            "Bool",
            [boolPopup]()
            {
                boolPopup->open();
            }
        )
    );


    gui.addItemToPage(
        pageSystem,
        makeButton(
            {236, 34, 72, 28},
            "Numeric",
            [numericPopup]()
            {
                numericPopup->open();
            }
        )
    );


    gui.addItemToPage(
        pageSystem,
        makeButton(
            {312, 34, 76, 28},
            "Options",
            [optionsPopup]()
            {
                optionsPopup->open();
            }
        )
    );


    auto* fileTestLabel =
        new casio::Label(
            {8, 72, 250, 22},
            "Safe storage: /personalised"
        );

    gui.addItemToPage(
        pageSystem,
        fileTestLabel
    );


    gui.addItemToPage(
        pageSystem,
        makeButton(
            {268, 70, 120, 28},
            "Test file I/O",
            [fileTestLabel]()
            {
                casio::ConfigFile output;

                output.setString(
                    "TEST",
                    "name",
                    "phase11"
                );

                output.setInt(
                    "TEST",
                    "value",
                    1234
                );

                bool ok =
                    output.saveSafe(
                        "phase13.ini"
                    );

                casio::ConfigFile input;

                ok = ok &&
                    input.loadSafe(
                        "phase13.ini"
                    );

                ok = ok &&
                    input.getStringOr(
                        "TEST",
                        "name",
                        ""
                    ) == "phase11";

                ok = ok &&
                    input.getIntOr(
                        "TEST",
                        "value",
                        0
                    ) == 1234;

                fileTestLabel->setText(
                    ok
                        ? "Filesystem /personalised: PASS"
                        : "Filesystem /personalised: FAIL"
                );
            }
        )
    );


    gui.addItemToPage(
        pageSystem,
        new casio::Label(
            {8, 108, 380, 52},
            "Self-test",
            casio::TextAlign::LEFT
        )
    );


    gui.addItemToPage(
        pageSystem,
        new casio::Label(
            {8, 160, 380, 14},
            "OPTN -> Screenshot -> /Capt/image.bmp",
            casio::TextAlign::CENTER
        )
    );


    //--------------------------------------------------------------------------
    // Final theme propagation after every control exists.
    //--------------------------------------------------------------------------

    gui.setThemePreset(
        casio::defaultThemePreset()
    );

    status->setLeftText(
        selfTestsOk
            ? "Self-test PASS"
            : "Self-test FAIL"
    );


    // MENU remains the system way to leave/switch away from the add-in.
    // EXIT is reserved for widget-level back / close / defocus.
    gui.runGUI();

    return 1;
}
