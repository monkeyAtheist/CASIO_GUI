#include "CASIO_GUI/casio.hpp"

int main()
{
    casio::GUI gui;

    auto* name = new casio::TextBox(
        casio::Position{20, 30, 150, 28},
        "Jerry",
        "Name",
        32,
        1
    );

    auto* value = new casio::Numeric(
        casio::Position{20, 70, 100, 28},
        25,
        0,
        100,
        5,
        1
    );

    auto* enabled = new casio::ToggleButton(
        casio::Position{20, 110, 100, 32}
    );

    gui.addItem(name);
    gui.addItem(value);
    gui.addItem(enabled);

    // ---------------------------------------------------------------------
    // 1) Simple INI configuration
    // ---------------------------------------------------------------------

    casio::IniFile config;

    if(config.load("/app.ini"))
    {
        int brightness =
            config.getIntOr(
                "DISPLAY",
                "brightness",
                3
            );

        bool sound =
            config.getBoolOr(
                "AUDIO",
                "sound",
                true
            );

        (void)brightness;
        (void)sound;
    }
    else
    {
        config.setInt(
            "DISPLAY",
            "brightness",
            3
        );

        config.setBool(
            "AUDIO",
            "sound",
            true
        );

        config.setString(
            "USER",
            "name",
            "Jerry"
        );

        config.save("/app.ini");
    }

    // ---------------------------------------------------------------------
    // 2) Flat save file
    // ---------------------------------------------------------------------

    casio::DataFile save;

    save.setInt("level", 4);
    save.setDouble("score", 125.5);
    save.setBool("first_run", false);
    save.setString("profile", "default");

    save.save("/save.dat");

    // ---------------------------------------------------------------------
    // 3) Save / restore GUI item state
    // ---------------------------------------------------------------------

    casio::GuiStateFile state;

    // Restore if a previous state exists.
    if(state.load("/gui_state.ini"))
    {
        state.applyGUI("GUI", gui);

        state.apply(
            "name_box",
            *name
        );

        state.apply(
            "value_numeric",
            *value
        );

        state.apply(
            "enabled_toggle",
            *enabled
        );
    }

    // Example: modify values during the program.
    value->setValue(50);
    enabled->setOn(true);

    // Capture by stable application names, not runtime item IDs.
    state.captureGUI("GUI", gui);

    state.capture(
        "name_box",
        *name
    );

    state.capture(
        "value_numeric",
        *value
    );

    state.capture(
        "enabled_toggle",
        *enabled
    );

    state.save("/gui_state.ini");

    gui.runGUI();

    return 1;
}
