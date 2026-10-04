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
    assert(relativePath("image") == "screenshots/image.bmp");

    std::string path;

    result status =
        saveBmp(
            "phase19_test",
            &path
        );

    assert(status == result::OK);
    assert(path == "screenshots/phase19_test.bmp");

    std::string resolved;

    assert(
        casio::storage::safe_file::resolve(
            path,
            resolved
        )
    );

    std::ifstream file(
        resolved,
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

    const std::streamoff expected =
        54 +
        static_cast<std::streamoff>(
            rowStride
        ) *
        DHEIGHT;

    assert(file.tellg() == expected);

    file.close();

    casio::storage::safe_file::remove(
        path
    );

    return 0;
}
