#ifndef CASIO_GUI_VERSION_HPP
#define CASIO_GUI_VERSION_HPP

// Frozen public API baseline.
#define CASIO_GUI_VERSION_MAJOR 1
#define CASIO_GUI_VERSION_MINOR 0
#define CASIO_GUI_VERSION_PATCH 0

// Feature level accumulated during stabilization. Keep this monotonic.
#define CASIO_GUI_API_LEVEL 21

// 1 once the public v1 API is frozen.
#define CASIO_GUI_API_FROZEN 1

namespace casio_gui_version
{

inline constexpr int major = CASIO_GUI_VERSION_MAJOR;
inline constexpr int minor = CASIO_GUI_VERSION_MINOR;
inline constexpr int patch = CASIO_GUI_VERSION_PATCH;

inline constexpr int api_level = CASIO_GUI_API_LEVEL;
inline constexpr bool api_frozen = CASIO_GUI_API_FROZEN != 0;

inline constexpr const char* string = "1.0.0";
inline constexpr const char* stage = "stable";

} // namespace casio_gui_version

#endif // CASIO_GUI_VERSION_HPP
