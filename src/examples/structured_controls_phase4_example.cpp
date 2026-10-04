#include "CASIO_GUI/casio.hpp"

int main()
{
    casio::GUI gui;

    auto* radioGroup = new casio::RadioGroup(
        {15, 20, 145, 82},
        {"Auto", "Manual", "Test"},
        0
    );

    auto* list = new casio::ListBox(
        {175, 20, 150, 105},
        {
            "Channel A",
            "Channel B",
            "Channel C",
            "Channel D",
            "Channel E",
            "Channel F"
        }
    );

    auto* combo = new casio::ComboBox(
        {15, 120, 145, 26},
        {"9600", "19200", "115200"},
        2
    );

    auto* options = new casio::OptionsPopup(
        "Display",
        "Choose display options:"
    );

    int grid = options->addCheckbox("Grid", true);
    int cursor = options->addToggle("Cursor", true);
    int linear = options->addRadio("Linear", 1, true);
    int log = options->addRadio("Logarithmic", 1, false);

    options->setOnApply(
        [grid, cursor, linear, log]
        (casio::OptionsPopup& popup)
        {
            bool gridEnabled = popup.getValue(grid);
            bool cursorEnabled = popup.getValue(cursor);
            bool linearMode = popup.getValue(linear);
            bool logMode = popup.getValue(log);

            (void)gridEnabled;
            (void)cursorEnabled;
            (void)linearMode;
            (void)logMode;
        }
    );

    gui.item.addItem(radioGroup);
    gui.item.addItem(list);
    gui.item.addItem(combo);
    gui.addGlobalItem(options);

    gui.runGUI();

    return 1;
}
