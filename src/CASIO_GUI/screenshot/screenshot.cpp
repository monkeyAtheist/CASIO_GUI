#include "screenshot.hpp"

#include "../storage/storage.hpp"

#include <gint/display.h>
#include <gint/gint.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <string>


namespace casio_gui_screenshot
{

namespace
{

struct write_context
{
    const uint16_t* pixels = nullptr;
    int width = 0;
    int height = 0;

    char directory[48] = {};
    char path[96] = {};
};


bool writeAll(
    int fd,
    const void* data,
    size_t size)
{
    const unsigned char* bytes =
        static_cast<const unsigned char*>(
            data
        );

    size_t remaining = size;

    while(remaining > 0)
    {
        ssize_t written =
            write(
                fd,
                bytes,
                remaining
            );

        if(written <= 0)
            return false;

        bytes += written;

        remaining -=
            static_cast<size_t>(
                written
            );
    }

    return true;
}


void setU16LE(
    unsigned char* dst,
    uint16_t value)
{
    dst[0] =
        static_cast<unsigned char>(
            value & 0xffu
        );

    dst[1] =
        static_cast<unsigned char>(
            (value >> 8) &
            0xffu
        );
}


void setU32LE(
    unsigned char* dst,
    uint32_t value)
{
    dst[0] =
        static_cast<unsigned char>(
            value & 0xffu
        );

    dst[1] =
        static_cast<unsigned char>(
            (value >> 8) &
            0xffu
        );

    dst[2] =
        static_cast<unsigned char>(
            (value >> 16) &
            0xffu
        );

    dst[3] =
        static_cast<unsigned char>(
            (value >> 24) &
            0xffu
        );
}


void rgb565ToRgb888(
    uint16_t color,
    unsigned char& r,
    unsigned char& g,
    unsigned char& b)
{
    unsigned int r5 =
        (color >> 11) &
        0x1fu;

    unsigned int g6 =
        (color >> 5) &
        0x3fu;

    unsigned int b5 =
        color &
        0x1fu;

    r =
        static_cast<unsigned char>(
            (r5 << 3) |
            (r5 >> 2)
        );

    g =
        static_cast<unsigned char>(
            (g6 << 2) |
            (g6 >> 4)
        );

    b =
        static_cast<unsigned char>(
            (b5 << 3) |
            (b5 >> 2)
        );
}


bool hasBmpExtension(
    const std::string& value)
{
    if(value.size() < 4)
        return false;

    size_t i =
        value.size() - 4;

    return
        value[i] == '.' &&
        (value[i + 1] == 'b' || value[i + 1] == 'B') &&
        (value[i + 2] == 'm' || value[i + 2] == 'M') &&
        (value[i + 3] == 'p' || value[i + 3] == 'P');
}


// IMPORTANT:
// This whole function runs in the OS world. It must not call dgetpixel(),
// dupdate(), getkey(), timers, or any other gint hardware API.
int writeBmpInOsWorld(
    void* opaque)
{
    write_context* context =
        static_cast<write_context*>(
            opaque
        );

    if(
        context == nullptr ||
        context->pixels == nullptr ||
        context->width <= 0 ||
        context->height <= 0)
    {
        return static_cast<int>(
            result::WRITE_ERROR
        );
    }

    if(
        mkdir(
            context->directory,
            0755) != 0 &&
        errno != EEXIST)
    {
        return static_cast<int>(
            result::STORAGE_ERROR
        );
    }

    int fd =
        open(
            context->path,
            O_WRONLY |
            O_CREAT |
            O_TRUNC,
            0644
        );

    if(fd < 0)
    {
        return static_cast<int>(
            result::STORAGE_ERROR
        );
    }

    const uint32_t rowBytes =
        static_cast<uint32_t>(
            context->width * 3
        );

    const uint32_t rowStride =
        (rowBytes + 3u) &
        ~3u;

    const uint32_t pixelBytes =
        rowStride *
        static_cast<uint32_t>(
            context->height
        );

    const uint32_t fileSize =
        54u +
        pixelBytes;

    unsigned char header[54] = {};

    header[0] = 'B';
    header[1] = 'M';

    setU32LE(
        header + 2,
        fileSize
    );

    setU32LE(
        header + 10,
        54u
    );

    setU32LE(
        header + 14,
        40u
    );

    setU32LE(
        header + 18,
        static_cast<uint32_t>(
            context->width
        )
    );

    setU32LE(
        header + 22,
        static_cast<uint32_t>(
            context->height
        )
    );

    setU16LE(
        header + 26,
        1u
    );

    setU16LE(
        header + 28,
        24u
    );

    setU32LE(
        header + 34,
        pixelBytes
    );

    if(
        !writeAll(
            fd,
            header,
            sizeof(header)))
    {
        close(fd);
        unlink(context->path);

        return static_cast<int>(
            result::WRITE_ERROR
        );
    }

    // GRAPH 90+E width = 396, therefore 1188 bytes per BMP line.
    // Keep a conservative fixed upper bound rather than allocating inside the
    // OS world. This remains tiny compared with the VRAM snapshot.
    constexpr int MaxWidth = 512;
    constexpr int MaxRowStride =
        ((MaxWidth * 3) + 3) &
        ~3;

    if(context->width > MaxWidth)
    {
        close(fd);
        unlink(context->path);

        return static_cast<int>(
            result::WRITE_ERROR
        );
    }

    unsigned char row[MaxRowStride];

    // BMP rows are bottom-up.
    for(
        int y = context->height - 1;
        y >= 0;
        --y)
    {
        unsigned int offset = 0;

        const uint16_t* source =
            context->pixels +
            static_cast<size_t>(y) *
            static_cast<size_t>(
                context->width
            );

        for(
            int x = 0;
            x < context->width;
            ++x)
        {
            unsigned char r = 0;
            unsigned char g = 0;
            unsigned char b = 0;

            rgb565ToRgb888(
                source[x],
                r,
                g,
                b
            );

            // 24-bit BMP stores B, G, R.
            row[offset++] = b;
            row[offset++] = g;
            row[offset++] = r;
        }

        while(offset < rowStride)
            row[offset++] = 0;

        if(
            !writeAll(
                fd,
                row,
                rowStride))
        {
            close(fd);
            unlink(context->path);

            return static_cast<int>(
                result::WRITE_ERROR
            );
        }
    }

    close(fd);

    return static_cast<int>(
        result::OK
    );
}


int runWorldWrite(
    write_context* context)
{
#ifdef CASIO_GUI_HOST_TEST
    return writeBmpInOsWorld(
        context
    );
#else
    // All POSIX/Fugue calls are intentionally inside one world switch.
    return static_cast<int>(
        gint_world_switch(
            GINT_CALL(
                writeBmpInOsWorld,
                static_cast<void*>(
                    context
                )
            )
        )
    );
#endif
}


bool copyText(
    char* destination,
    size_t capacity,
    const std::string& source)
{
    if(
        destination == nullptr ||
        capacity == 0 ||
        source.size() >= capacity)
    {
        return false;
    }

    memcpy(
        destination,
        source.c_str(),
        source.size() + 1
    );

    return true;
}

} // namespace


std::string sanitizeName(
    const std::string& name)
{
    std::string source =
        casio_storage::trim(name);

    if(hasBmpExtension(source))
    {
        source.resize(
            source.size() - 4
        );
    }

    std::string output;
    output.reserve(32);

    for(char c : source)
    {
        if(output.size() >= 32)
            break;

        bool alphaNumeric =
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9');

        if(
            alphaNumeric ||
            c == '_' ||
            c == '-')
        {
            output.push_back(c);
        }
        else if(
            c == ' ' ||
            c == '.')
        {
            if(
                output.empty() ||
                output.back() != '_')
            {
                output.push_back('_');
            }
        }
    }

    while(
        !output.empty() &&
        output.front() == '_')
    {
        output.erase(
            output.begin()
        );
    }

    while(
        !output.empty() &&
        output.back() == '_')
    {
        output.pop_back();
    }

    if(output.empty())
        output = "image";

    return output;
}


std::string outputPath(
    const std::string& name)
{
    std::string directory =
        CASIO_GUI_SCREENSHOT_ROOT;

    if(directory.empty())
        directory = "/Capt";

    if(directory.front() != '/')
    {
        directory.insert(
            directory.begin(),
            '/'
        );
    }

    while(
        directory.size() > 1 &&
        directory.back() == '/')
    {
        directory.pop_back();
    }

    return
        directory +
        "/" +
        sanitizeName(name) +
        ".bmp";
}


result saveBmp(
    const std::string& name,
    std::string* outputRelativePath)
{
    //--------------------------------------------------------------------------
    // 1) Snapshot VRAM while gint owns the hardware.
    //
    // Filesystem access requires the OS world, but dgetpixel()/gint_vram must
    // be used in the gint world. Keep those phases strictly separated.
    //--------------------------------------------------------------------------

    const size_t pixelCount =
        static_cast<size_t>(DWIDTH) *
        static_cast<size_t>(DHEIGHT);

    const size_t snapshotBytes =
        pixelCount *
        sizeof(uint16_t);

    uint16_t* snapshot =
        static_cast<uint16_t*>(
            malloc(
                snapshotBytes
            )
        );

    if(snapshot == nullptr)
        return result::MEMORY_ERROR;

    const void* currentVram =
        gint_vram;

    if(currentVram == nullptr)
    {
        free(snapshot);
        return result::MEMORY_ERROR;
    }

    memcpy(
        snapshot,
        currentVram,
        snapshotBytes
    );


    //--------------------------------------------------------------------------
    // 2) Build RAM-backed path/context before the world switch.
    //--------------------------------------------------------------------------

    std::string directory =
        CASIO_GUI_SCREENSHOT_ROOT;

    if(directory.empty())
        directory = "/Capt";

    if(directory.front() != '/')
    {
        directory.insert(
            directory.begin(),
            '/'
        );
    }

    while(
        directory.size() > 1 &&
        directory.back() == '/')
    {
        directory.pop_back();
    }

    std::string path =
        outputPath(name);

    write_context context;
    context.pixels = snapshot;
    context.width = DWIDTH;
    context.height = DHEIGHT;

    if(
        !copyText(
            context.directory,
            sizeof(context.directory),
            directory) ||
        !copyText(
            context.path,
            sizeof(context.path),
            path))
    {
        free(snapshot);
        return result::INVALID_NAME;
    }


    //--------------------------------------------------------------------------
    // 3) One OS world switch for mkdir/open/write/close/unlink.
    //--------------------------------------------------------------------------

    int rawResult =
        runWorldWrite(
            &context
        );

    free(snapshot);

    result finalResult =
        static_cast<result>(
            rawResult
        );

    if(
        finalResult == result::OK &&
        outputRelativePath != nullptr)
    {
        *outputRelativePath =
            path;
    }

    return finalResult;
}

} // namespace casio_gui_screenshot
