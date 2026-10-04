#include "CASIO_GUI/casio.hpp"

#include <vector>

/*
 * Example 08 - Canvas and shapes
 *
 * Demonstrates:
 *   - retained-mode Canvas commands
 *   - line, rectangle, circle, ellipse
 *   - polygon
 *   - text
 *   - standalone shape item
 */

int main()
{
    casio::GUI gui;

    casio::Label title(
        {8, 2, 380, 18},
        "Canvas / shapes",
        casio::TextAlign::CENTER,
        true
    );

    casio::Canvas canvas(
        {8, 24, 280, 166},
        true,
        true
    );

    canvas.reserveCommands(16);

    canvas.addText(
        {8, 6},
        "Retained Canvas",
        casio::Color(0, 0, 0)
    );

    canvas.addLine(
        {12, 32},
        {180, 110},
        casio::Color(35, 95, 200),
        2
    );

    canvas.addRectangle(
        {30, 52, 70, 46},
        casio::Color(235, 240, 255),
        casio::Color(30, 30, 30),
        casio::DrawMode::FILLED,
        1
    );

    canvas.addCircle(
        {150, 70},
        28,
        casio::Color(245, 220, 180),
        casio::Color(160, 90, 20),
        casio::DrawMode::FILLED,
        1
    );

    canvas.addEllipse(
        {220, 112},
        35,
        18,
        casio::Color(210, 245, 220),
        casio::Color(20, 130, 60),
        casio::DrawMode::FILLED,
        1
    );

    std::vector<casio::Point> polygon{
        {30, 128},
        {60, 112},
        {92, 130},
        {78, 152},
        {42, 154}
    };

    canvas.addPolygon(
        polygon,
        casio::Color(245, 210, 220),
        casio::Color(150, 30, 70),
        casio::DrawMode::FILLED,
        1
    );

    canvas.setOnCanvasClick(
        [&canvas](
            int localX,
            int localY)
        {
            canvas.addCircle(
                {localX, localY},
                3,
                casio::Color(220, 40, 40),
                casio::Color(220, 40, 40),
                casio::DrawMode::FILLED,
                1,
                20
            );
        }
    );

    casio::Circle standaloneCircle(
        340,
        80,
        30,
        casio::Color(220, 235, 255),
        casio::Color(35, 95, 200),
        casio::DrawMode::FILLED,
        2
    );

    casio::Label hint(
        {294, 122, 96, 54},
        "EXE on\nCanvas adds\na point",
        casio::TextAlign::CENTER,
        false
    );

    gui.addItem(&title);
    gui.addItem(&canvas);
    gui.addItem(&standaloneCircle);
    gui.addItem(&hint);

    gui.runGUI();
    return 0;
}
