#include "CASIO_GUI/casio.hpp"

int main()
{
    casio::GUI gui;

    auto* canvas =
        new casio::Canvas(
            {20, 25, 340, 165},
            true,
            true,
            2
        );

    canvas->reserveCommands(20);

    canvas->addText(
        {8, 8},
        "Retained Canvas",
        casio::Color(0, 0, 0),
        10
    );

    canvas->addRectangle(
        {10, 30, 85, 48},
        casio::Color(220, 240, 255),
        casio::Color(0, 0, 0),
        casio::DrawMode::FILLED,
        2,
        1
    );

    int circle =
        canvas->addCircle(
            {145, 55},
            30,
            casio::Color(255, 220, 100),
            casio::Color(0, 0, 0),
            casio::DrawMode::FILLED,
            2,
            2
        );

    canvas->addEllipse(
        {225, 55},
        38,
        20,
        casio::Color(220, 255, 220),
        casio::Color(0, 0, 0),
        casio::DrawMode::FILLED,
        1,
        3
    );

    canvas->addPolyline(
        {
            {15, 130},
            {55, 100},
            {95, 135},
            {135, 105}
        },
        casio::Color(180, 0, 180),
        2,
        4
    );

    canvas->addPolygon(
        {
            {165, 105},
            {205, 90},
            {250, 110},
            {220, 140},
            {175, 135}
        },
        casio::Color(235, 220, 255),
        casio::Color(70, 0, 100),
        casio::DrawMode::FILLED,
        2,
        5
    );

    canvas->addArc(
        {292, 112},
        35,
        190.0f,
        345.0f,
        casio::Color(220, 30, 30),
        2,
        28,
        6
    );

    canvas->setOnCanvasClick(
        [canvas, circle](int x, int y)
        {
            casio::CanvasCommand* command =
                canvas->getCommand(circle);

            if(command == nullptr)
                return;

            command->p1 =
                casio::Point{
                    x,
                    y
                };
        }
    );

    gui.item.addItem(canvas);
    gui.runGUI();

    return 1;
}
