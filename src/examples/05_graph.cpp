#include "CASIO_GUI/casio.hpp"

#include <cmath>

/*
 * Example 05 - Graph
 *
 * Demonstrates:
 *   - linear axes
 *   - two data series
 *   - points + connected curves
 *   - measurement cursors
 *   - pan / zoom / reset
 *
 * Controls while Graph has focus:
 *   - arrows: pan
 *   - + / -: zoom in / out
 *   - 0: reset view
 *   - OPTN: toggle cursor-control mode
 *   - arrows in cursor mode: move active cursor
 *   - EXIT: release graph focus
 */

int main()
{
    casio::GUI gui;

    casio::Label title(
        {8, 2, 380, 18},
        "Graph: sin(x) and cos(x)",
        casio::TextAlign::CENTER,
        true
    );

    casio::GraphAxis xAxis(
        -6.4,
        6.4,
        casio::GraphScale::LINEAR,
        4,
        true,
        true,
        "x",
        true,
        2
    );

    casio::GraphAxis yAxis(
        -1.5,
        1.5,
        casio::GraphScale::LINEAR,
        6,
        true,
        true,
        "y",
        true,
        2
    );

    casio::Graph graph(
        {8, 24, 380, 164},
        xAxis,
        yAxis
    );

    const int sinSeries =
        graph.addSeries(
            casio::Color(35, 95, 200),
            true,
            true
        );

    const int cosSeries =
        graph.addSeries(
            casio::Color(210, 50, 50),
            true,
            false
        );

    const double pi =
        3.14159265358979323846;

    for(int i = 0; i <= 48; ++i)
    {
        const double x =
            -2.0 * pi +
            (4.0 * pi * i) / 48.0;

        graph.addPoint(
            static_cast<unsigned int>(sinSeries),
            x,
            std::sin(x)
        );

        graph.addPoint(
            static_cast<unsigned int>(cosSeries),
            x,
            std::cos(x)
        );
    }

    graph.setPanStep(0.12);
    graph.setZoomFactor(0.80);
    graph.setCursorStep(0.10);

    graph.addCursor(
        0.0,
        0.0,
        casio::Color(220, 40, 40),
        "A",
        casio::GraphCursorStyle::CROSSHAIR
    );

    graph.addCursor(
        pi / 2.0,
        1.0,
        casio::Color(20, 150, 70),
        "B",
        casio::GraphCursorStyle::VERTICAL
    );

    graph.setActiveCursor(0);
    graph.setShowCursorDelta(true);

    casio::Label hint(
        {8, 190, 380, 14},
        "EXE focus | arrows pan | +/- zoom | 0 reset | OPTN cursor",
        casio::TextAlign::CENTER,
        false
    );

    gui.addItem(&title);
    gui.addItem(&graph);
    gui.addItem(&hint);

    gui.runGUI();
    return 0;
}
