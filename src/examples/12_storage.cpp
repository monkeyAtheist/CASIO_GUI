#include "CASIO_GUI/casio.hpp"

/*
 * Example 12 - Safe storage
 *
 * Demonstrates:
 *   - ConfigFile / IniFile
 *   - saveSafe()
 *   - loadSafe()
 *   - /personalised application storage
 *   - world-switch-safe file I/O
 *
 * Controls:
 *   - F1: save
 *   - F2: load
 *   - F3: remove
 */

int main()
{
    casio::GUI gui;

    casio::ConfigFile config;

    casio::Label title(
        {8, 20, 380, 24},
        "Safe configuration storage",
        casio::TextAlign::CENTER,
        true
    );

    casio::TextBox name(
        {70, 64, 250, 28},
        "demo",
        "Name",
        24,
        0,
        casio::TextBoxInputMode::MIXED
    );

    casio::Numeric value(
        {145, 106, 100, 28},
        42,
        0,
        999,
        1
    );

    casio::StatusBar status(
        {0, casio::HEIGHT - 46, casio::WIDTH, 22},
        "File: examples/storage.ini",
        "",
        ""
    );

    casio::SoftKeyBar softKeys(
        {"Save", "Load", "Delete", "", "", ""}
    );

    softKeys.setCallback(
        0,
        [&config, &name, &value, &status]()
        {
            config.clear();

            config.setString(
                "EXAMPLE",
                "name",
                name.getText()
            );

            config.setInt(
                "EXAMPLE",
                "value",
                value.getValue()
            );

            const bool ok =
                config.saveSafe(
                    "examples/storage.ini"
                );

            status.setLeftText(
                ok
                    ? "Saved"
                    : "Save error"
            );
        }
    );

    softKeys.setCallback(
        1,
        [&config, &name, &value, &status]()
        {
            casio::ConfigFile loaded;

            if(
                !loaded.loadSafe(
                    "examples/storage.ini"
                ))
            {
                status.setLeftText(
                    "Load error"
                );

                return;
            }

            std::string restoredName;

            if(
                loaded.getString(
                    "EXAMPLE",
                    "name",
                    restoredName))
            {
                name.setText(
                    restoredName
                );
            }

            int restoredValue = 0;

            if(
                loaded.getInt(
                    "EXAMPLE",
                    "value",
                    restoredValue))
            {
                value.setValue(
                    restoredValue
                );
            }

            status.setLeftText(
                "Loaded"
            );
        }
    );

    softKeys.setCallback(
        2,
        [&status]()
        {
            const bool ok =
                casio::safeStorage::remove(
                    "examples/storage.ini"
                );

            status.setLeftText(
                ok
                    ? "Deleted"
                    : "Nothing to delete"
            );
        }
    );

    gui.addItem(&title);
    gui.addItem(&name);
    gui.addItem(&value);
    gui.addGlobalItem(&status);
    gui.addGlobalItem(&softKeys);

    gui.runGUI();
    return 0;
}
