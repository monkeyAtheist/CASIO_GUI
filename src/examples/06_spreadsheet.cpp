#include "CASIO_GUI/casio.hpp"

/*
 * Example 06 - Table / Spreadsheet
 *
 * Demonstrates:
 *   - Spreadsheet alias
 *   - column labels
 *   - initial cell data
 *   - selection callback
 *   - cell edit callback
 *
 * Controls:
 *   - EXE: focus / toggle cell edit mode
 *   - arrows: move selected cell when not editing
 *   - DEL: delete last character while editing
 *   - EXIT: leave table focus
 */

int main()
{
    casio::GUI gui;

    casio::Label title(
        {8, 2, 380, 18},
        "Spreadsheet",
        casio::TextAlign::CENTER,
        true
    );

    casio::Spreadsheet sheet(
        {8, 24, 380, 164},
        10,
        5,
        64,
        20,
        true
    );

    sheet.setColumnLabel(0, "Name");
    sheet.setColumnLabel(1, "Q1");
    sheet.setColumnLabel(2, "Q2");
    sheet.setColumnLabel(3, "Q3");
    sheet.setColumnLabel(4, "Total");

    sheet.setCell(0, 0, "A");
    sheet.setCell(0, 1, "10");
    sheet.setCell(0, 2, "12");
    sheet.setCell(0, 3, "14");
    sheet.setCell(0, 4, "36");

    sheet.setCell(1, 0, "B");
    sheet.setCell(1, 1, "8");
    sheet.setCell(1, 2, "11");
    sheet.setCell(1, 3, "15");
    sheet.setCell(1, 4, "34");

    sheet.setCell(2, 0, "C");
    sheet.setCell(2, 1, "9");
    sheet.setCell(2, 2, "13");
    sheet.setCell(2, 3, "16");
    sheet.setCell(2, 4, "38");

    casio::StatusBar status(
        {0, casio::HEIGHT - 22, casio::WIDTH, 22},
        "EXE: edit cell",
        "",
        ""
    );

    sheet.setOnSelectionChanged(
        [&status](
            unsigned int,
            unsigned int)
        {
            status.setLeftText(
                "Selection changed"
            );
        }
    );

    sheet.setOnCellChanged(
        [&status](
            unsigned int,
            unsigned int,
            const std::string&)
        {
            status.setLeftText(
                "Cell edited"
            );
        }
    );

    gui.addItem(&title);
    gui.addItem(&sheet);
    gui.addGlobalItem(&status);

    gui.runGUI();
    return 0;
}
