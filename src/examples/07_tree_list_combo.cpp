#include "CASIO_GUI/casio.hpp"

/*
 * Example 07 - Structured selection controls
 *
 * Demonstrates:
 *   - Tree
 *   - ListBox
 *   - ComboBox
 *
 * All three use keyboard focus so selection follows the keyboard, not a cursor
 * left parked on another row.
 */

int main()
{
    casio::GUI gui;

    casio::Label title(
        {8, 2, 380, 18},
        "Tree / ListBox / ComboBox",
        casio::TextAlign::CENTER,
        true
    );

    casio::Tree tree(
        {8, 26, 150, 154},
        18,
        14,
        true,
        true
    );

    const int rootProject =
        tree.addRoot(
            "Project",
            true
        );

    const int rootSystem =
        tree.addRoot(
            "System",
            true
        );

    tree.addNode(
        rootProject,
        "Source"
    );

    tree.addNode(
        rootProject,
        "Assets"
    );

    tree.addNode(
        rootProject,
        "Tests"
    );

    tree.addNode(
        rootSystem,
        "Display"
    );

    tree.addNode(
        rootSystem,
        "Storage"
    );

    casio::ListBox list(
        {170, 26, 112, 116},
        {
            "Alpha",
            "Bravo",
            "Charlie",
            "Delta",
            "Echo",
            "Foxtrot"
        },
        0,
        20,
        true
    );

    casio::ComboBox combo(
        {170, 152, 112, 26},
        {
            "Light",
            "Dark",
            "High contrast"
        },
        0,
        3,
        22
    );

    casio::StatusBar status(
        {0, casio::HEIGHT - 22, casio::WIDTH, 22},
        "Select a control",
        "",
        ""
    );

    tree.setOnSelected(
        [&status](
            int,
            const std::string&)
        {
            status.setLeftText(
                "Tree selection changed"
            );
        }
    );

    tree.setOnActivated(
        [&status](
            int,
            const std::string&)
        {
            status.setLeftText(
                "Tree node activated"
            );
        }
    );

    list.setOnSelected(
        [&status](
            int,
            const std::string&)
        {
            status.setLeftText(
                "List selection changed"
            );
        }
    );

    list.setOnActivated(
        [&status](
            int,
            const std::string&)
        {
            status.setLeftText(
                "List item activated"
            );
        }
    );

    combo.setOnChanged(
        [&gui, &status](
            int index,
            const std::string&)
        {
            if(index == 0)
            {
                gui.setThemePreset(
                    casio::ThemePreset::LIGHT
                );
            }
            else if(index == 1)
            {
                gui.setThemePreset(
                    casio::ThemePreset::DARK
                );
            }
            else
            {
                gui.setThemePreset(
                    casio::ThemePreset::HIGH_CONTRAST
                );
            }

            status.setLeftText(
                "Theme changed"
            );
        }
    );

    gui.addItem(&title);
    gui.addItem(&tree);
    gui.addItem(&list);
    gui.addItem(&combo);
    gui.addGlobalItem(&status);

    gui.runGUI();
    return 0;
}
