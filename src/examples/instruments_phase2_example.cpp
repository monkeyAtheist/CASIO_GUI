#include "CASIO_GUI/casio.hpp"

int main()
{
    casio::GUI gui;

    auto* gauge = new casio::Gauge(
        {20, 35, 145, 115},
        35,
        0,
        100,
        135.0f,
        405.0f,
        10,
        4,
        "%",
        2
    );

    gauge->addZone(
        0,
        60,
        casio::Color(0, 200, 0)
    );

    gauge->addZone(
        60,
        80,
        casio::Color(255, 180, 0)
    );

    gauge->addZone(
        80,
        100,
        casio::Color(255, 0, 0)
    );


    auto* dial = new casio::Dial(
        {200, 35, 145, 115},
        35,
        0,
        100,
        5,
        135.0f,
        405.0f,
        10,
        4,
        "%",
        2
    );

    dial->addZone(
        0,
        60,
        casio::Color(0, 200, 0)
    );

    dial->addZone(
        60,
        80,
        casio::Color(255, 180, 0)
    );

    dial->addZone(
        80,
        100,
        casio::Color(255, 0, 0)
    );

    dial->setOnChanged(
        [gauge](int value)
        {
            gauge->setValue(value);
        }
    );

    gui.item.addItem(gauge);
    gui.item.addItem(dial);

    gui.runGUI();

    return 1;
}
