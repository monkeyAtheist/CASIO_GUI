#ifndef CASIO_GUI_SCREENSHOT_HPP
#define CASIO_GUI_SCREENSHOT_HPP

#include <string>

#ifndef CASIO_GUI_SCREENSHOT_ROOT
#define CASIO_GUI_SCREENSHOT_ROOT "/Capt"
#endif

namespace casio_gui_screenshot
{

enum class result
{
    OK,
    INVALID_NAME,
    MEMORY_ERROR,
    STORAGE_ERROR,
    WORLD_SWITCH_ERROR,
    WRITE_ERROR
};

std::string sanitizeName(const std::string& name);
// Absolute storage-memory path. Default: /Capt/<name>.bmp
std::string outputPath(const std::string& name);

result saveBmp(
    const std::string& name,
    std::string* outputRelativePath = nullptr);

} // namespace casio_gui_screenshot

#endif
