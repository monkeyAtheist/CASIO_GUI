#include "CASIO_GUI/casio.hpp"

#include <cassert>
#include <fstream>
#include <string>

int main()
{
    using namespace casio::Screenshot;

    assert(sanitizeName("image") == "image");
    assert(sanitizeName("my capture.bmp") == "my_capture");
    assert(sanitizeName("../bad:name") == "badname");

    std::string expectedPath =
        std::string(CASIO_GUI_SCREENSHOT_ROOT) +
        "/image.bmp";

    assert(outputPath("image") == expectedPath);

    std::string path;

    result status =
        saveBmp(
            "phase20_test",
            &path
        );

    assert(status == result::OK);

    std::string expected =
        std::string(CASIO_GUI_SCREENSHOT_ROOT) +
        "/phase20_test.bmp";

    assert(path == expected);

    std::ifstream file(
        path,
        std::ios::binary
    );

    assert(file.good());

    unsigned char header[54] = {};

    file.read(
        reinterpret_cast<char*>(header),
        sizeof(header)
    );

    assert(file.gcount() == 54);
    assert(header[0] == 'B');
    assert(header[1] == 'M');
    assert(header[28] == 24);
    assert(header[29] == 0);

    file.seekg(0, std::ios::end);

    const int rowStride =
        ((DWIDTH * 3) + 3) &
        ~3;

    const std::streamoff expectedSize =
        54 +
        static_cast<std::streamoff>(
            rowStride
        ) *
        DHEIGHT;

    assert(file.tellg() == expectedSize);

    file.close();

    unlink(path.c_str());

    return 0;
}
