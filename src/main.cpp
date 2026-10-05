#include "CASIO_GUI/casio.hpp"

#include <string>
#include <vector>

// Optional fxconv asset used by the image validation page.
extern "C" bopti_image_t img_gui_test;

// executer dans le simulateur : ~/casio-emu/build/calcemu ~/casio/${pathTo.g3a}
// Clean : rm -rf build-cg
// build : fxsdk build-cg


//==============================================================================
// Helpers
//==============================================================================



//==============================================================================
// Main 
//==============================================================================

int main()
{
    casio::GUI gui;

    gui.runGUI();

    return 1;
}
