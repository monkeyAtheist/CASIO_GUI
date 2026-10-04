#include "CASIO_GUI/casio.hpp"

#include <cassert>
#include <string>

static_assert(casio::GUI_VERSION_MAJOR == 1);
static_assert(casio::GUI_VERSION_MINOR == 0);
static_assert(casio::GUI_VERSION_PATCH == 0);
static_assert(casio::GUI_API_LEVEL == 21);
static_assert(casio::GUI_API_FROZEN);

int main()
{
    assert(std::string(casio::GUI_VERSION) == "1.0.0");
    assert(std::string(casio::GUI_API_STAGE) == "stable");

    casio::Button button({0, 0, 80, 24});
    casio::TextBox textbox(
        {0, 0, 100, 24},
        "",
        "",
        16,
        0,
        casio::TextBoxInputMode::MIXED
    );
    casio::Checkbox checkbox({0, 0, 100, 24}, "Check", false);
    casio::Slider slider({0, 0, 100, 24}, 50);
    casio::TabView tabs({0, 0, 120, 80}, {"A", "B"}, 0);

    (void)button;
    (void)textbox;
    (void)checkbox;
    (void)slider;
    (void)tabs;

    return 0;
}
