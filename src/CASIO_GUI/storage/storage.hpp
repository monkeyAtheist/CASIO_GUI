#ifndef CASIO_STORAGE_HPP
#define CASIO_STORAGE_HPP

#include <string>
#include <vector>
#include <map>
#include <stdint.h>
#include <stddef.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <gint/gint.h>

#include "../gui/gui.hpp"

#ifndef CASIO_GUI_STORAGE_ROOT
#define CASIO_GUI_STORAGE_ROOT "/personalised"
#endif

#ifndef CASIO_GUI_SAFE_STORAGE_MAX_BYTES
#define CASIO_GUI_SAFE_STORAGE_MAX_BYTES 131072
#endif


namespace casio_storage
{

//==============================================================================
// Small parsing / formatting helpers
//==============================================================================

inline bool isSpace(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

inline std::string trim(std::string value)
{
    size_t first = 0;

    while(first < value.size() && isSpace(value[first]))
        ++first;

    size_t last = value.size();

    while(last > first && isSpace(value[last - 1]))
        --last;

    return value.substr(first, last - first);
}

inline std::string toLower(std::string value)
{
    for(char& c : value)
    {
        if(c >= 'A' && c <= 'Z')
            c = static_cast<char>(c - 'A' + 'a');
    }

    return value;
}

inline bool parseInt(const std::string& text, int& output)
{
    std::string value = trim(text);

    if(value.empty())
        return false;

    size_t i = 0;
    bool negative = false;

    if(value[i] == '+' || value[i] == '-')
    {
        negative = value[i] == '-';
        ++i;
    }

    if(i >= value.size())
        return false;

    long result = 0;
    bool digit = false;

    for(; i < value.size(); ++i)
    {
        char c = value[i];

        if(c < '0' || c > '9')
            return false;

        digit = true;
        result = result * 10 + static_cast<long>(c - '0');
    }

    if(!digit)
        return false;

    output = static_cast<int>(negative ? -result : result);
    return true;
}

inline bool parseUInt(const std::string& text, unsigned int& output)
{
    int value = 0;

    if(!parseInt(text, value) || value < 0)
        return false;

    output = static_cast<unsigned int>(value);
    return true;
}

inline bool parseBool(const std::string& text, bool& output)
{
    std::string value = toLower(trim(text));

    if(value == "1" || value == "true" || value == "yes" || value == "on")
    {
        output = true;
        return true;
    }

    if(value == "0" || value == "false" || value == "no" || value == "off")
    {
        output = false;
        return true;
    }

    return false;
}

inline bool parseDouble(const std::string& text, double& output)
{
    std::string value = trim(text);

    if(value.empty())
        return false;

    size_t i = 0;
    bool negative = false;

    if(value[i] == '+' || value[i] == '-')
    {
        negative = value[i] == '-';
        ++i;
    }

    if(i >= value.size())
        return false;

    double integerPart = 0.0;
    bool hasDigit = false;

    while(i < value.size() && value[i] >= '0' && value[i] <= '9')
    {
        hasDigit = true;
        integerPart = integerPart * 10.0 + static_cast<double>(value[i] - '0');
        ++i;
    }

    double fraction = 0.0;
    double divisor = 1.0;

    if(i < value.size() && value[i] == '.')
    {
        ++i;

        while(i < value.size() && value[i] >= '0' && value[i] <= '9')
        {
            hasDigit = true;
            fraction = fraction * 10.0 + static_cast<double>(value[i] - '0');
            divisor *= 10.0;
            ++i;
        }
    }

    if(!hasDigit)
        return false;

    double result = integerPart + fraction / divisor;

    if(i < value.size() && (value[i] == 'e' || value[i] == 'E'))
    {
        ++i;

        bool expNegative = false;

        if(i < value.size() && (value[i] == '+' || value[i] == '-'))
        {
            expNegative = value[i] == '-';
            ++i;
        }

        if(i >= value.size())
            return false;

        int exponent = 0;
        bool expDigit = false;

        while(i < value.size() && value[i] >= '0' && value[i] <= '9')
        {
            expDigit = true;
            exponent = exponent * 10 + (value[i] - '0');

            if(exponent > 308)
                exponent = 308;

            ++i;
        }

        if(!expDigit)
            return false;

        double factor = 1.0;

        for(int n = 0; n < exponent; ++n)
            factor *= 10.0;

        result = expNegative ? result / factor : result * factor;
    }

    if(i != value.size())
        return false;

    output = negative ? -result : result;
    return true;
}

inline std::string formatInt(int value)
{
    char buffer[24];
    int pos = 0;

    long magnitude = value;

    if(magnitude < 0)
    {
        buffer[pos++] = '-';
        magnitude = -magnitude;
    }

    char digits[20];
    int count = 0;

    do
    {
        digits[count++] = static_cast<char>('0' + magnitude % 10);
        magnitude /= 10;
    }
    while(magnitude > 0 && count < 20);

    while(count > 0)
        buffer[pos++] = digits[--count];

    buffer[pos] = '\0';
    return std::string(buffer);
}

inline std::string formatUInt(unsigned int value)
{
    char digits[20];
    int count = 0;

    do
    {
        digits[count++] = static_cast<char>('0' + value % 10u);
        value /= 10u;
    }
    while(value > 0u && count < 20);

    std::string result;

    while(count > 0)
        result.push_back(digits[--count]);

    return result;
}

inline std::string formatDouble(double value, int precision = 6)
{
    if(precision < 0)
        precision = 0;

    if(precision > 9)
        precision = 9;

    bool negative = value < 0.0;

    if(negative)
        value = -value;

    // Storage values are intentionally emitted as a compact fixed decimal.
    // This avoids snprintf/iostream dependencies in fxlibc.
    unsigned long long integerPart =
        static_cast<unsigned long long>(value);

    double fraction = value - static_cast<double>(integerPart);

    unsigned long long scale = 1;

    for(int i = 0; i < precision; ++i)
        scale *= 10ull;

    unsigned long long fractionPart =
        static_cast<unsigned long long>(fraction * static_cast<double>(scale) + 0.5);

    if(fractionPart >= scale && scale > 0)
    {
        ++integerPart;
        fractionPart = 0;
    }

    std::string result;

    if(negative)
        result.push_back('-');

    {
        char digits[32];
        int count = 0;

        do
        {
            digits[count++] =
                static_cast<char>('0' + (integerPart % 10ull));

            integerPart /= 10ull;
        }
        while(integerPart > 0ull && count < 32);

        while(count > 0)
            result.push_back(digits[--count]);
    }

    if(precision > 0)
    {
        result.push_back('.');

        std::string frac(static_cast<size_t>(precision), '0');

        for(int i = precision - 1; i >= 0; --i)
        {
            frac[static_cast<size_t>(i)] =
                static_cast<char>('0' + (fractionPart % 10ull));

            fractionPart /= 10ull;
        }

        while(!frac.empty() && frac.back() == '0')
            frac.pop_back();

        if(frac.empty())
        {
            result.pop_back();
        }
        else
        {
            result += frac;
        }
    }

    return result;
}

inline std::string escapeString(const std::string& value)
{
    std::string result;

    for(char c : value)
    {
        switch(c)
        {
            case '\\': result += "\\\\"; break;
            case '"':  result += "\\\""; break;
            case '\n': result += "\\n";  break;
            case '\r': result += "\\r";  break;
            case '\t': result += "\\t";  break;
            default:   result.push_back(c); break;
        }
    }

    return result;
}

inline std::string unescapeString(const std::string& value)
{
    std::string result;
    bool escape = false;

    for(char c : value)
    {
        if(!escape)
        {
            if(c == '\\')
                escape = true;
            else
                result.push_back(c);

            continue;
        }

        switch(c)
        {
            case 'n':  result.push_back('\n'); break;
            case 'r':  result.push_back('\r'); break;
            case 't':  result.push_back('\t'); break;
            case '\\': result.push_back('\\'); break;
            case '"':  result.push_back('"');  break;
            default:   result.push_back(c);    break;
        }

        escape = false;
    }

    if(escape)
        result.push_back('\\');

    return result;
}


//==============================================================================
// Files
//==============================================================================

namespace file
{

namespace detail
{

struct path_context
{
    const char* path = nullptr;
};

struct text_read_context
{
    const char* path = nullptr;
    std::string* output = nullptr;
};

struct raw_write_context
{
    const char* path = nullptr;
    const void* data = nullptr;
    size_t size = 0;
    bool append = false;
};


inline int runWorld(
    int (*function)(void*),
    void* context)
{
#ifdef CASIO_GUI_HOST_TEST
    return function(context);
#else
    return static_cast<int>(
        gint_world_switch(
            GINT_CALL(
                function,
                context
            )
        )
    );
#endif
}


inline int existsWorld(void* opaque)
{
    path_context* context =
        static_cast<path_context*>(
            opaque
        );

    int fd =
        open(
            context->path,
            O_RDONLY
        );

    if(fd < 0)
        return 0;

    close(fd);
    return 1;
}


inline int readTextWorld(void* opaque)
{
    text_read_context* context =
        static_cast<text_read_context*>(
            opaque
        );

    context->output->clear();

    int fd =
        open(
            context->path,
            O_RDONLY
        );

    if(fd < 0)
        return 0;

    char buffer[512];

    while(true)
    {
        ssize_t count =
            read(
                fd,
                buffer,
                sizeof(buffer)
            );

        if(count < 0)
        {
            close(fd);
            context->output->clear();
            return 0;
        }

        if(count == 0)
            break;

        context->output->append(
            buffer,
            static_cast<size_t>(
                count
            )
        );
    }

    close(fd);
    return 1;
}


inline int writeRawWorld(void* opaque)
{
    raw_write_context* context =
        static_cast<raw_write_context*>(
            opaque
        );

    int flags =
        O_WRONLY |
        O_CREAT;

    if(context->append)
        flags |= O_APPEND;
    else
        flags |= O_TRUNC;

    int fd =
        open(
            context->path,
            flags,
            0644
        );

    if(fd < 0)
        return 0;

    const unsigned char* bytes =
        static_cast<const unsigned char*>(
            context->data
        );

    size_t remaining =
        context->size;

    while(remaining > 0)
    {
        ssize_t written =
            write(
                fd,
                bytes,
                remaining
            );

        if(written <= 0)
        {
            close(fd);
            return 0;
        }

        bytes += written;

        remaining -=
            static_cast<size_t>(
                written
            );
    }

    close(fd);
    return 1;
}


inline int removeWorld(void* opaque)
{
    path_context* context =
        static_cast<path_context*>(
            opaque
        );

    return
        unlink(
            context->path
        ) == 0;
}


inline int createDirectoryWorld(void* opaque)
{
    path_context* context =
        static_cast<path_context*>(
            opaque
        );

    if(
        context->path == nullptr ||
        context->path[0] == '\0')
    {
        return 0;
    }

    if(
        context->path[0] == '/' &&
        context->path[1] == '\0')
    {
        return 1;
    }

    if(
        mkdir(
            context->path,
            0755) == 0)
    {
        return 1;
    }

    return errno == EEXIST;
}

} // namespace detail


inline bool exists(
    const std::string& path)
{
    detail::path_context context{
        path.c_str()
    };

    return
        detail::runWorld(
            detail::existsWorld,
            &context
        ) != 0;
}


inline bool readText(
    const std::string& path,
    std::string& output)
{
    detail::text_read_context context{
        path.c_str(),
        &output
    };

    return
        detail::runWorld(
            detail::readTextWorld,
            &context
        ) != 0;
}


inline bool writeRaw(
    const std::string& path,
    const void* data,
    size_t size,
    bool append)
{
    detail::raw_write_context context{
        path.c_str(),
        data,
        size,
        append
    };

    return
        detail::runWorld(
            detail::writeRawWorld,
            &context
        ) != 0;
}


inline bool writeText(
    const std::string& path,
    const std::string& contents)
{
    return writeRaw(
        path,
        contents.data(),
        contents.size(),
        false
    );
}


inline bool appendText(
    const std::string& path,
    const std::string& contents)
{
    return writeRaw(
        path,
        contents.data(),
        contents.size(),
        true
    );
}


inline bool readBinary(
    const std::string& path,
    std::vector<unsigned char>& output)
{
    std::string temporary;

    if(
        !readText(
            path,
            temporary))
    {
        output.clear();
        return false;
    }

    output.assign(
        temporary.begin(),
        temporary.end()
    );

    return true;
}


inline bool writeBinary(
    const std::string& path,
    const std::vector<unsigned char>& data)
{
    if(data.empty())
    {
        return writeRaw(
            path,
            nullptr,
            0,
            false
        );
    }

    return writeRaw(
        path,
        data.data(),
        data.size(),
        false
    );
}


inline bool appendBinary(
    const std::string& path,
    const std::vector<unsigned char>& data)
{
    if(data.empty())
        return true;

    return writeRaw(
        path,
        data.data(),
        data.size(),
        true
    );
}


inline bool remove(
    const std::string& path)
{
    detail::path_context context{
        path.c_str()
    };

    return
        detail::runWorld(
            detail::removeWorld,
            &context
        ) != 0;
}


inline bool createDirectory(
    const std::string& path)
{
    detail::path_context context{
        path.c_str()
    };

    return
        detail::runWorld(
            detail::createDirectoryWorld,
            &context
        ) != 0;
}


inline bool ensureDirectories(
    const std::string& path)
{
    if(path.empty())
        return false;

    std::string current;
    size_t start = 0;

    if(path[0] == '/')
    {
        current = "/";
        start = 1;
    }

    for(
        size_t i = start;
        i <= path.size();
        ++i)
    {
        if(
            i != path.size() &&
            path[i] != '/')
        {
            continue;
        }

        std::string part =
            path.substr(
                start,
                i - start
            );

        if(!part.empty())
        {
            if(
                !current.empty() &&
                current.back() != '/')
            {
                current.push_back('/');
            }

            current += part;

            if(
                !createDirectory(
                    current))
            {
                return false;
            }
        }

        start = i + 1;
    }

    return true;
}


} // namespace file


//==============================================================================
// Safe application storage
//==============================================================================

namespace safe_file
{

inline const std::string& root()
{
    static const std::string value =
        CASIO_GUI_STORAGE_ROOT;

    return value;
}

inline bool isSafeRelativePath(
    const std::string& path)
{
    if(path.empty())
        return false;

    if(path[0] == '/' || path[0] == '\\')
        return false;

    if(path.size() > 96)
        return false;

    size_t start = 0;

    while(start <= path.size())
    {
        size_t end =
            path.find('/', start);

        if(end == std::string::npos)
            end = path.size();

        std::string part =
            path.substr(
                start,
                end - start
            );

        if(
            part.empty() ||
            part == "." ||
            part == "..")
        {
            return false;
        }

        for(char c : part)
        {
            if(
                c == '\\' ||
                c == ':' ||
                static_cast<unsigned char>(c) < 32)
            {
                return false;
            }
        }

        if(end == path.size())
            break;

        start = end + 1;
    }

    return true;
}

inline bool resolve(
    const std::string& relativePath,
    std::string& output)
{
    if(!isSafeRelativePath(relativePath))
        return false;

    output = root();

    if(
        !output.empty() &&
        output.back() != '/')
    {
        output.push_back('/');
    }

    output += relativePath;
    return true;
}

inline bool ensureRoot()
{
    return file::ensureDirectories(
        root()
    );
}

inline bool ensureParent(
    const std::string& resolvedPath)
{
    size_t slash =
        resolvedPath.rfind('/');

    if(slash == std::string::npos)
        return true;

    std::string parent =
        resolvedPath.substr(
            0,
            slash
        );

    if(parent.empty())
        return true;

    return file::ensureDirectories(
        parent
    );
}

inline bool exists(
    const std::string& relativePath)
{
    std::string resolved;

    return
        resolve(relativePath, resolved) &&
        file::exists(resolved);
}

inline bool readText(
    const std::string& relativePath,
    std::string& output)
{
    std::string resolved;

    return
        resolve(relativePath, resolved) &&
        file::readText(
            resolved,
            output
        );
}

inline bool writeText(
    const std::string& relativePath,
    const std::string& contents)
{
    if(
        contents.size() >
        static_cast<size_t>(
            CASIO_GUI_SAFE_STORAGE_MAX_BYTES))
    {
        return false;
    }

    std::string resolved;

    if(!resolve(relativePath, resolved))
        return false;

    if(
        !ensureRoot() ||
        !ensureParent(resolved))
    {
        return false;
    }

    return file::writeText(
        resolved,
        contents
    );
}

inline bool readBinary(
    const std::string& relativePath,
    std::vector<unsigned char>& output)
{
    std::string resolved;

    return
        resolve(relativePath, resolved) &&
        file::readBinary(
            resolved,
            output
        );
}

inline bool writeBinary(
    const std::string& relativePath,
    const std::vector<unsigned char>& data)
{
    if(
        data.size() >
        static_cast<size_t>(
            CASIO_GUI_SAFE_STORAGE_MAX_BYTES))
    {
        return false;
    }

    std::string resolved;

    if(!resolve(relativePath, resolved))
        return false;

    if(
        !ensureRoot() ||
        !ensureParent(resolved))
    {
        return false;
    }

    return file::writeBinary(
        resolved,
        data
    );
}

inline bool remove(
    const std::string& relativePath)
{
    std::string resolved;

    return
        resolve(relativePath, resolved) &&
        file::remove(resolved);
}

} // namespace safe_file


//==============================================================================
// INI parser / writer
//==============================================================================

class IniFile
{
public:
    using Section = std::map<std::string, std::string>;
    using Data = std::map<std::string, Section>;

    bool load(const std::string& path)
    {
        std::string text;

        if(!file::readText(path, text))
            return false;

        return parse(text);
    }

    bool save(const std::string& path) const
    {
        return file::writeText(path, serialize());
    }

    bool loadSafe(
        const std::string& relativePath)
    {
        std::string text;

        if(
            !safe_file::readText(
                relativePath,
                text))
        {
            return false;
        }

        return parse(text);
    }

    bool saveSafe(
        const std::string& relativePath) const
    {
        return safe_file::writeText(
            relativePath,
            serialize()
        );
    }

    bool parse(const std::string& text)
    {
        data.clear();

        std::string currentSection;
        size_t start = 0;

        while(start <= text.size())
        {
            size_t end = text.find('\n', start);

            if(end == std::string::npos)
                end = text.size();

            std::string line = text.substr(start, end - start);

            if(!line.empty() && line.back() == '\r')
                line.pop_back();

            line = trim(line);

            if(!line.empty() && line[0] != ';' && line[0] != '#')
            {
                if(line.front() == '[' && line.back() == ']')
                {
                    currentSection =
                        trim(line.substr(1, line.size() - 2));
                }
                else
                {
                    size_t separator = findSeparator(line);

                    if(separator != std::string::npos)
                    {
                        std::string key =
                            trim(line.substr(0, separator));

                        std::string rawValue =
                            stripInlineComment(
                                line.substr(separator + 1)
                            );

                        if(!key.empty())
                        {
                            data[currentSection][key] =
                                decodeValue(rawValue);
                        }
                    }
                }
            }

            if(end == text.size())
                break;

            start = end + 1;
        }

        return true;
    }

    std::string serialize() const
    {
        std::string output;

        auto root = data.find("");

        if(root != data.end())
        {
            appendSectionValues(output, root->second);

            if(data.size() > 1)
                output += "\n";
        }

        for(const auto& sectionPair : data)
        {
            if(sectionPair.first.empty())
                continue;

            output += "[";
            output += sectionPair.first;
            output += "]\n";

            appendSectionValues(
                output,
                sectionPair.second
            );

            output += "\n";
        }

        return output;
    }

    void clear()
    {
        data.clear();
    }

    bool hasSection(const std::string& section) const
    {
        return data.find(section) != data.end();
    }

    bool has(
        const std::string& section,
        const std::string& key) const
    {
        const std::string* value =
            find(section, key);

        return value != nullptr;
    }

    bool removeKey(
        const std::string& section,
        const std::string& key)
    {
        auto sectionIt = data.find(section);

        if(sectionIt == data.end())
            return false;

        return sectionIt->second.erase(key) > 0;
    }

    bool removeSection(const std::string& section)
    {
        return data.erase(section) > 0;
    }

    const Section* getSection(
        const std::string& section) const
    {
        auto it = data.find(section);

        if(it == data.end())
            return nullptr;

        return &it->second;
    }

    std::vector<std::string> getSections() const
    {
        std::vector<std::string> result;

        for(const auto& current : data)
            result.push_back(current.first);

        return result;
    }

    std::vector<std::string> getKeys(
        const std::string& section) const
    {
        std::vector<std::string> result;

        const Section* values =
            getSection(section);

        if(values == nullptr)
            return result;

        for(const auto& current : *values)
            result.push_back(current.first);

        return result;
    }

    void setString(
        const std::string& section,
        const std::string& key,
        const std::string& value)
    {
        data[section][key] = value;
    }

    void setInt(
        const std::string& section,
        const std::string& key,
        int value)
    {
        setString(
            section,
            key,
            formatInt(value)
        );
    }

    void setUInt(
        const std::string& section,
        const std::string& key,
        unsigned int value)
    {
        setString(
            section,
            key,
            formatUInt(value)
        );
    }

    void setDouble(
        const std::string& section,
        const std::string& key,
        double value,
        int precision = 6)
    {
        setString(
            section,
            key,
            formatDouble(value, precision)
        );
    }

    void setBool(
        const std::string& section,
        const std::string& key,
        bool value)
    {
        setString(
            section,
            key,
            value ? "true" : "false"
        );
    }

    bool getString(
        const std::string& section,
        const std::string& key,
        std::string& output) const
    {
        const std::string* value =
            find(section, key);

        if(value == nullptr)
            return false;

        output = *value;
        return true;
    }

    bool getInt(
        const std::string& section,
        const std::string& key,
        int& output) const
    {
        const std::string* value =
            find(section, key);

        return value != nullptr &&
            parseInt(*value, output);
    }

    bool getUInt(
        const std::string& section,
        const std::string& key,
        unsigned int& output) const
    {
        const std::string* value =
            find(section, key);

        return value != nullptr &&
            parseUInt(*value, output);
    }

    bool getDouble(
        const std::string& section,
        const std::string& key,
        double& output) const
    {
        const std::string* value =
            find(section, key);

        return value != nullptr &&
            parseDouble(*value, output);
    }

    bool getBool(
        const std::string& section,
        const std::string& key,
        bool& output) const
    {
        const std::string* value =
            find(section, key);

        return value != nullptr &&
            parseBool(*value, output);
    }

    std::string getStringOr(
        const std::string& section,
        const std::string& key,
        const std::string& fallback = "") const
    {
        std::string value;

        return getString(section, key, value)
            ? value
            : fallback;
    }

    int getIntOr(
        const std::string& section,
        const std::string& key,
        int fallback = 0) const
    {
        int value = 0;

        return getInt(section, key, value)
            ? value
            : fallback;
    }

    unsigned int getUIntOr(
        const std::string& section,
        const std::string& key,
        unsigned int fallback = 0) const
    {
        unsigned int value = 0;

        return getUInt(section, key, value)
            ? value
            : fallback;
    }

    double getDoubleOr(
        const std::string& section,
        const std::string& key,
        double fallback = 0.0) const
    {
        double value = 0.0;

        return getDouble(section, key, value)
            ? value
            : fallback;
    }

    bool getBoolOr(
        const std::string& section,
        const std::string& key,
        bool fallback = false) const
    {
        bool value = false;

        return getBool(section, key, value)
            ? value
            : fallback;
    }

    void setStringList(
        const std::string& section,
        const std::string& key,
        const std::vector<std::string>& values)
    {
        std::string encoded;

        for(size_t i = 0; i < values.size(); ++i)
        {
            if(i != 0)
                encoded.push_back('|');

            for(char c : values[i])
            {
                if(c == '\\' || c == '|')
                    encoded.push_back('\\');

                encoded.push_back(c);
            }
        }

        setString(section, key, encoded);
    }

    bool getStringList(
        const std::string& section,
        const std::string& key,
        std::vector<std::string>& output) const
    {
        std::string encoded;

        if(!getString(section, key, encoded))
            return false;

        output.clear();

        std::string current;
        bool escape = false;

        for(char c : encoded)
        {
            if(escape)
            {
                current.push_back(c);
                escape = false;
                continue;
            }

            if(c == '\\')
            {
                escape = true;
                continue;
            }

            if(c == '|')
            {
                output.push_back(current);
                current.clear();
                continue;
            }

            current.push_back(c);
        }

        if(escape)
            current.push_back('\\');

        output.push_back(current);
        return true;
    }

    const Data& raw() const
    {
        return data;
    }

private:
    Data data;

    const std::string* find(
        const std::string& section,
        const std::string& key) const
    {
        auto sectionIt = data.find(section);

        if(sectionIt == data.end())
            return nullptr;

        auto keyIt = sectionIt->second.find(key);

        if(keyIt == sectionIt->second.end())
            return nullptr;

        return &keyIt->second;
    }

    static size_t findSeparator(
        const std::string& line)
    {
        bool quoted = false;
        bool escaped = false;

        for(size_t i = 0; i < line.size(); ++i)
        {
            char c = line[i];

            if(escaped)
            {
                escaped = false;
                continue;
            }

            if(c == '\\')
            {
                escaped = true;
                continue;
            }

            if(c == '"')
            {
                quoted = !quoted;
                continue;
            }

            if(c == '=' && !quoted)
                return i;
        }

        return std::string::npos;
    }

    static std::string stripInlineComment(
        const std::string& value)
    {
        bool quoted = false;
        bool escaped = false;

        for(size_t i = 0; i < value.size(); ++i)
        {
            char c = value[i];

            if(escaped)
            {
                escaped = false;
                continue;
            }

            if(c == '\\')
            {
                escaped = true;
                continue;
            }

            if(c == '"')
            {
                quoted = !quoted;
                continue;
            }

            if(!quoted && (c == ';' || c == '#'))
            {
                if(i == 0 || isSpace(value[i - 1]))
                    return trim(value.substr(0, i));
            }
        }

        return trim(value);
    }

    static std::string decodeValue(
        const std::string& value)
    {
        std::string result = trim(value);

        if(result.size() >= 2 &&
           result.front() == '"' &&
           result.back() == '"')
        {
            result =
                result.substr(1, result.size() - 2);
        }

        return unescapeString(result);
    }

    static std::string encodeValue(
        const std::string& value)
    {
        bool quote = value.empty();

        if(!value.empty() &&
           (isSpace(value.front()) ||
            isSpace(value.back())))
        {
            quote = true;
        }

        for(char c : value)
        {
            if(c == ';' || c == '#' ||
               c == '=' || c == '"' ||
               c == '\\' || c == '\n' ||
               c == '\r' || c == '\t')
            {
                quote = true;
                break;
            }
        }

        std::string encoded =
            escapeString(value);

        if(quote)
            return "\"" + encoded + "\"";

        return encoded;
    }

    static void appendSectionValues(
        std::string& output,
        const Section& section)
    {
        for(const auto& current : section)
        {
            output += current.first;
            output += "=";
            output += encodeValue(current.second);
            output += "\n";
        }
    }
};


// Flat key/value save file. This is useful for application state where a full
// INI section hierarchy is unnecessary.
class DataFile
{
public:
    bool load(const std::string& path)
    {
        return ini.load(path);
    }

    bool save(const std::string& path) const
    {
        return ini.save(path);
    }

    bool parse(const std::string& text)
    {
        return ini.parse(text);
    }

    std::string serialize() const
    {
        return ini.serialize();
    }

    void clear()
    {
        ini.clear();
    }

    bool has(const std::string& key) const
    {
        return ini.has("", key);
    }

    void setString(
        const std::string& key,
        const std::string& value)
    {
        ini.setString("", key, value);
    }

    void setInt(
        const std::string& key,
        int value)
    {
        ini.setInt("", key, value);
    }

    void setUInt(
        const std::string& key,
        unsigned int value)
    {
        ini.setUInt("", key, value);
    }

    void setDouble(
        const std::string& key,
        double value,
        int precision = 6)
    {
        ini.setDouble("", key, value, precision);
    }

    void setBool(
        const std::string& key,
        bool value)
    {
        ini.setBool("", key, value);
    }

    std::string getStringOr(
        const std::string& key,
        const std::string& fallback = "") const
    {
        return ini.getStringOr("", key, fallback);
    }

    int getIntOr(
        const std::string& key,
        int fallback = 0) const
    {
        return ini.getIntOr("", key, fallback);
    }

    unsigned int getUIntOr(
        const std::string& key,
        unsigned int fallback = 0) const
    {
        return ini.getUIntOr("", key, fallback);
    }

    double getDoubleOr(
        const std::string& key,
        double fallback = 0.0) const
    {
        return ini.getDoubleOr("", key, fallback);
    }

    bool getBoolOr(
        const std::string& key,
        bool fallback = false) const
    {
        return ini.getBoolOr("", key, fallback);
    }

    IniFile& getIni()
    {
        return ini;
    }

    const IniFile& getIni() const
    {
        return ini;
    }

private:
    IniFile ini;
};



//==============================================================================
// Named object serialization
//==============================================================================
//
// File format:
//     [META]
//     format=CASIO_OBJECTS
//     version=1
//
//     [OBJECT.motor1]
//     _type=Motor
//     _version=1
//     speed=1200
//     enabled=true
//
// The object name is the stable identifier. Runtime pointers/IDs are not stored.
//==============================================================================

class NamedObjectFile
{
public:
    static constexpr int CURRENT_FORMAT_VERSION = 1;

    NamedObjectFile()
    {
        resetDocument();
    }

    bool load(const std::string& path)
    {
        if(!data.load(path))
            return false;

        currentObject.clear();

        return isValidFormat();
    }

    bool save(const std::string& path) const
    {
        return data.save(path);
    }

    bool parse(const std::string& text)
    {
        if(!data.parse(text))
            return false;

        currentObject.clear();

        return isValidFormat();
    }

    std::string serialize() const
    {
        return data.serialize();
    }

    void clear()
    {
        resetDocument();
    }

    bool isValidFormat() const
    {
        return data.getStringOr(
            "META",
            "format",
            ""
        ) == "CASIO_OBJECTS";
    }

    int getFormatVersion() const
    {
        return data.getIntOr(
            "META",
            "version",
            0
        );
    }

    void setFormatVersion(int version)
    {
        if(version < 1)
            version = 1;

        data.setInt(
            "META",
            "version",
            version
        );
    }

    const std::string& getCurrentObjectName() const
    {
        return currentObject;
    }

    bool hasCurrentObject() const
    {
        return !currentObject.empty();
    }

    bool beginObject(
        const std::string& name,
        const std::string& type = "Object",
        int version = 1)
    {
        if(name.empty())
            return false;

        currentObject = name;

        const std::string section =
            objectSection(name);

        // Preserve an existing type when no explicit type is supplied.
        if(!type.empty())
        {
            data.setString(
                section,
                "_type",
                type
            );
        }
        else if(!data.has(section, "_type"))
        {
            data.setString(
                section,
                "_type",
                "Object"
            );
        }

        if(version < 1)
            version = 1;

        data.setInt(
            section,
            "_version",
            version
        );

        return true;
    }

    bool selectObject(const std::string& name)
    {
        if(!hasObject(name))
            return false;

        currentObject = name;
        return true;
    }

    void endObject()
    {
        currentObject.clear();
    }

    bool hasObject(const std::string& name) const
    {
        return data.hasSection(
            objectSection(name)
        );
    }

    bool removeObject(const std::string& name)
    {
        if(currentObject == name)
            currentObject.clear();

        return data.removeSection(
            objectSection(name)
        );
    }

    std::vector<std::string> getObjects() const
    {
        std::vector<std::string> result;
        std::vector<std::string> sections =
            data.getSections();

        const std::string prefix = "OBJECT.";

        for(const std::string& section : sections)
        {
            if(section.size() <= prefix.size())
                continue;

            if(section.compare(
                0,
                prefix.size(),
                prefix) != 0)
            {
                continue;
            }

            result.push_back(
                section.substr(prefix.size())
            );
        }

        return result;
    }

    std::string getObjectType(
        const std::string& name) const
    {
        return data.getStringOr(
            objectSection(name),
            "_type",
            ""
        );
    }

    int getObjectVersion(
        const std::string& name) const
    {
        return data.getIntOr(
            objectSection(name),
            "_version",
            0
        );
    }

    bool has(const std::string& key) const
    {
        if(!hasCurrentObject())
            return false;

        return data.has(
            currentSection(),
            key
        );
    }

    //-------------------------------------------------------------------------
    // Typed writers for the current object
    //-------------------------------------------------------------------------

    bool set(
        const std::string& key,
        const std::string& value)
    {
        if(!canWrite(key))
            return false;

        data.setString(
            currentSection(),
            key,
            value
        );

        return true;
    }

    bool set(
        const std::string& key,
        const char* value)
    {
        return set(
            key,
            value != nullptr
                ? std::string(value)
                : std::string()
        );
    }

    bool set(
        const std::string& key,
        int value)
    {
        if(!canWrite(key))
            return false;

        data.setInt(
            currentSection(),
            key,
            value
        );

        return true;
    }

    bool set(
        const std::string& key,
        unsigned int value)
    {
        if(!canWrite(key))
            return false;

        data.setUInt(
            currentSection(),
            key,
            value
        );

        return true;
    }

    bool set(
        const std::string& key,
        double value,
        int precision = 6)
    {
        if(!canWrite(key))
            return false;

        data.setDouble(
            currentSection(),
            key,
            value,
            precision
        );

        return true;
    }

    bool set(
        const std::string& key,
        bool value)
    {
        if(!canWrite(key))
            return false;

        data.setBool(
            currentSection(),
            key,
            value
        );

        return true;
    }

    bool set(
        const std::string& key,
        const std::vector<std::string>& values)
    {
        if(!canWrite(key))
            return false;

        data.setStringList(
            currentSection(),
            key,
            values
        );

        return true;
    }

    bool set(
        const std::string& key,
        const STRUCT_point& value)
    {
        if(!canWrite(key))
            return false;

        data.setInt(
            currentSection(),
            key + ".x",
            value.x
        );

        data.setInt(
            currentSection(),
            key + ".y",
            value.y
        );

        return true;
    }

    bool set(
        const std::string& key,
        const STRUCT_pos& value)
    {
        if(!canWrite(key))
            return false;

        data.setInt(
            currentSection(),
            key + ".x",
            value.x
        );

        data.setInt(
            currentSection(),
            key + ".y",
            value.y
        );

        data.setInt(
            currentSection(),
            key + ".w",
            value.w
        );

        data.setInt(
            currentSection(),
            key + ".h",
            value.h
        );

        return true;
    }

    bool set(
        const std::string& key,
        const Class_color& value)
    {
        if(!canWrite(key))
            return false;

        data.setInt(
            currentSection(),
            key + ".r",
            value.getR()
        );

        data.setInt(
            currentSection(),
            key + ".g",
            value.getG()
        );

        data.setInt(
            currentSection(),
            key + ".b",
            value.getB()
        );

        return true;
    }

    //-------------------------------------------------------------------------
    // Typed readers for the current object
    //-------------------------------------------------------------------------

    bool get(
        const std::string& key,
        std::string& output) const
    {
        return canRead(key) &&
            data.getString(
                currentSection(),
                key,
                output
            );
    }

    bool get(
        const std::string& key,
        int& output) const
    {
        return canRead(key) &&
            data.getInt(
                currentSection(),
                key,
                output
            );
    }

    bool get(
        const std::string& key,
        unsigned int& output) const
    {
        return canRead(key) &&
            data.getUInt(
                currentSection(),
                key,
                output
            );
    }

    bool get(
        const std::string& key,
        double& output) const
    {
        return canRead(key) &&
            data.getDouble(
                currentSection(),
                key,
                output
            );
    }

    bool get(
        const std::string& key,
        bool& output) const
    {
        return canRead(key) &&
            data.getBool(
                currentSection(),
                key,
                output
            );
    }

    bool get(
        const std::string& key,
        std::vector<std::string>& output) const
    {
        return canRead(key) &&
            data.getStringList(
                currentSection(),
                key,
                output
            );
    }

    bool get(
        const std::string& key,
        STRUCT_point& output) const
    {
        if(!hasCurrentObject())
            return false;

        int x = output.x;
        int y = output.y;

        bool hasX = data.getInt(
            currentSection(),
            key + ".x",
            x
        );

        bool hasY = data.getInt(
            currentSection(),
            key + ".y",
            y
        );

        if(!hasX || !hasY)
            return false;

        output.x = x;
        output.y = y;
        return true;
    }

    bool get(
        const std::string& key,
        STRUCT_pos& output) const
    {
        if(!hasCurrentObject())
            return false;

        int x = output.x;
        int y = output.y;
        int w = output.w;
        int h = output.h;

        bool ok =
            data.getInt(currentSection(), key + ".x", x) &&
            data.getInt(currentSection(), key + ".y", y) &&
            data.getInt(currentSection(), key + ".w", w) &&
            data.getInt(currentSection(), key + ".h", h);

        if(!ok)
            return false;

        output.x = x;
        output.y = y;
        output.w = w;
        output.h = h;
        return true;
    }

    bool get(
        const std::string& key,
        Class_color& output) const
    {
        if(!hasCurrentObject())
            return false;

        int r = output.getR();
        int g = output.getG();
        int b = output.getB();

        bool ok =
            data.getInt(currentSection(), key + ".r", r) &&
            data.getInt(currentSection(), key + ".g", g) &&
            data.getInt(currentSection(), key + ".b", b);

        if(!ok)
            return false;

        output.setRGB(r, g, b);
        return true;
    }

    //-------------------------------------------------------------------------
    // Convenience readers with fallback
    //-------------------------------------------------------------------------

    std::string getStringOr(
        const std::string& key,
        const std::string& fallback = "") const
    {
        if(!hasCurrentObject())
            return fallback;

        return data.getStringOr(
            currentSection(),
            key,
            fallback
        );
    }

    int getIntOr(
        const std::string& key,
        int fallback = 0) const
    {
        if(!hasCurrentObject())
            return fallback;

        return data.getIntOr(
            currentSection(),
            key,
            fallback
        );
    }

    unsigned int getUIntOr(
        const std::string& key,
        unsigned int fallback = 0) const
    {
        if(!hasCurrentObject())
            return fallback;

        return data.getUIntOr(
            currentSection(),
            key,
            fallback
        );
    }

    double getDoubleOr(
        const std::string& key,
        double fallback = 0.0) const
    {
        if(!hasCurrentObject())
            return fallback;

        return data.getDoubleOr(
            currentSection(),
            key,
            fallback
        );
    }

    bool getBoolOr(
        const std::string& key,
        bool fallback = false) const
    {
        if(!hasCurrentObject())
            return fallback;

        return data.getBoolOr(
            currentSection(),
            key,
            fallback
        );
    }

    //-------------------------------------------------------------------------
    // Explicit object access without changing currentObject
    //-------------------------------------------------------------------------

    bool set(
        const std::string& object,
        const std::string& key,
        const std::string& value)
    {
        if(object.empty() || key.empty())
            return false;

        data.setString(
            objectSection(object),
            key,
            value
        );

        return true;
    }

    std::string getStringOr(
        const std::string& object,
        const std::string& key,
        const std::string& fallback) const
    {
        return data.getStringOr(
            objectSection(object),
            key,
            fallback
        );
    }

    IniFile& ini()
    {
        return data;
    }

    const IniFile& ini() const
    {
        return data;
    }

private:
    IniFile data;
    std::string currentObject;

    static std::string objectSection(
        const std::string& name)
    {
        return "OBJECT." + name;
    }

    std::string currentSection() const
    {
        return objectSection(
            currentObject
        );
    }

    bool canWrite(
        const std::string& key) const
    {
        return hasCurrentObject() &&
            !key.empty() &&
            key[0] != '_';
    }

    bool canRead(
        const std::string& key) const
    {
        return hasCurrentObject() &&
            !key.empty();
    }

    void resetDocument()
    {
        data.clear();
        currentObject.clear();

        data.setString(
            "META",
            "format",
            "CASIO_OBJECTS"
        );

        data.setInt(
            "META",
            "version",
            CURRENT_FORMAT_VERSION
        );
    }
};

using ObjectFile = NamedObjectFile;
using ObjectSerializer = NamedObjectFile;
using NamedSaveFile = NamedObjectFile;


//==============================================================================
// GUI state persistence
//==============================================================================
//
// Callbacks and raw image pointers are intentionally not serialized.
// Runtime item IDs are also not restored: the stable identifier is the section
// name chosen by the application ("volume_slider", "username", ...).
//==============================================================================

class GuiStateFile
{
public:
    IniFile& ini()
    {
        return data;
    }

    const IniFile& ini() const
    {
        return data;
    }

    bool load(const std::string& path)
    {
        return data.load(path);
    }

    bool save(const std::string& path) const
    {
        return data.save(path);
    }

    void clear()
    {
        data.clear();
    }

    void captureGUI(
        const std::string& section,
        const GUI& gui)
    {
        data.setInt(
            section,
            "page",
            gui.getCurrentPageId()
        );

        data.setInt(
            section,
            "cursor_x",
            gui.cursor.x()
        );

        data.setInt(
            section,
            "cursor_y",
            gui.cursor.y()
        );

        data.setInt(
            section,
            "cursor_speed",
            gui.cursor.getSpeed()
        );

        data.setInt(
            section,
            "cursor_style",
            static_cast<int>(
                gui.cursor.getStyle()
            )
        );

        data.setBool(
            section,
            "cursor_visible",
            gui.cursor.isVisible()
        );
    }

    void applyGUI(
        const std::string& section,
        GUI& gui) const
    {
        gui.cursor.setPos(
            data.getIntOr(
                section,
                "cursor_x",
                gui.cursor.x()
            ),
            data.getIntOr(
                section,
                "cursor_y",
                gui.cursor.y()
            )
        );

        gui.cursor.setSpeed(
            data.getIntOr(
                section,
                "cursor_speed",
                gui.cursor.getSpeed()
            )
        );

        int style =
            data.getIntOr(
                section,
                "cursor_style",
                static_cast<int>(
                    gui.cursor.getStyle()
                )
            );

        gui.cursor.setStyle(
            style == static_cast<int>(
                Cursor::cursorStyle::ARROW)
                ? Cursor::cursorStyle::ARROW
                : Cursor::cursorStyle::CROSS
        );

        gui.setCursorVisible(
            data.getBoolOr(
                section,
                "cursor_visible",
                gui.cursor.isVisible()
            )
        );

        int page =
            data.getIntOr(
                section,
                "page",
                gui.getCurrentPageId()
            );

        gui.setPage(page);
    }

    void capture(
        const std::string& section,
        const item& value)
    {
        captureCommon(section, value);
    }

    void apply(
        const std::string& section,
        item& value) const
    {
        applyCommon(section, value);
    }

    void capture(
        const std::string& section,
        const item_toggle_button& value)
    {
        captureCommon(section, value);
        data.setBool(section, "on", value.isOn());
    }

    void apply(
        const std::string& section,
        item_toggle_button& value) const
    {
        applyCommon(section, value);

        value.setOn(
            data.getBoolOr(
                section,
                "on",
                value.isOn()
            )
        );
    }

    void capture(
        const std::string& section,
        const item_textbox& value)
    {
        captureCommon(section, value);

        data.setString(
            section,
            "text",
            value.getText()
        );

        data.setUInt(
            section,
            "max_length",
            value.getMaxLength()
        );

        data.setInt(
            section,
            "input_mode",
            static_cast<int>(
                value.getInputMode()
            )
        );

        data.setBool(
            section,
            "auto_alpha_on_focus",
            value.getAutoAlphaOnFocus()
        );
    }

    void apply(
        const std::string& section,
        item_textbox& value) const
    {
        applyCommon(section, value);

        value.setMaxLength(
            data.getUIntOr(
                section,
                "max_length",
                value.getMaxLength()
            )
        );

        int inputMode =
            data.getIntOr(
                section,
                "input_mode",
                static_cast<int>(
                    value.getInputMode()
                )
            );

        if(
            inputMode < static_cast<int>(
                item_textbox_input_mode::MIXED
            ) ||
            inputMode > static_cast<int>(
                item_textbox_input_mode::NUMERIC_ONLY
            ))
        {
            inputMode =
                static_cast<int>(
                    item_textbox_input_mode::MIXED
                );
        }

        value.setInputMode(
            static_cast<item_textbox_input_mode>(
                inputMode
            )
        );

        value.setAutoAlphaOnFocus(
            data.getBoolOr(
                section,
                "auto_alpha_on_focus",
                value.getAutoAlphaOnFocus()
            )
        );

        value.setText(
            data.getStringOr(
                section,
                "text",
                value.getText()
            )
        );
    }

    void capture(
        const std::string& section,
        const item_numeric& value)
    {
        captureCommon(section, value);

        data.setInt(section, "value", value.getValue());
        data.setInt(section, "min", value.getMinValue());
        data.setInt(section, "max", value.getMaxValue());
        data.setInt(section, "step", value.getStep());
    }

    void apply(
        const std::string& section,
        item_numeric& value) const
    {
        applyCommon(section, value);

        value.setRange(
            data.getIntOr(
                section,
                "min",
                value.getMinValue()
            ),
            data.getIntOr(
                section,
                "max",
                value.getMaxValue()
            )
        );

        value.setStep(
            data.getIntOr(
                section,
                "step",
                value.getStep()
            )
        );

        value.setValue(
            data.getIntOr(
                section,
                "value",
                value.getValue()
            )
        );
    }

    void capture(
        const std::string& section,
        const item_slider& value)
    {
        captureCommon(section, value);

        data.setInt(section, "value", value.getValue());
        data.setInt(section, "min", value.getMinValue());
        data.setInt(section, "max", value.getMaxValue());
        data.setInt(section, "step", value.getStep());
        data.setInt(
            section,
            "orientation",
            static_cast<int>(value.getOrientation())
        );
    }

    void apply(
        const std::string& section,
        item_slider& value) const
    {
        applyCommon(section, value);

        value.setRange(
            data.getIntOr(
                section,
                "min",
                value.getMinValue()
            ),
            data.getIntOr(
                section,
                "max",
                value.getMaxValue()
            )
        );

        value.setStep(
            data.getIntOr(
                section,
                "step",
                value.getStep()
            )
        );

        value.setOrientation(
            data.getIntOr(
                section,
                "orientation",
                static_cast<int>(value.getOrientation())
            ) == static_cast<int>(item_orientation::VERTICAL)
                ? item_orientation::VERTICAL
                : item_orientation::HORIZONTAL
        );

        value.setValue(
            data.getIntOr(
                section,
                "value",
                value.getValue()
            )
        );
    }

    void capture(
        const std::string& section,
        const item_checkbox& value)
    {
        captureCommon(section, value);

        data.setBool(
            section,
            "checked",
            value.isChecked()
        );

        data.setString(
            section,
            "checkbox_label",
            value.getCheckboxLabel()
        );
    }

    void apply(
        const std::string& section,
        item_checkbox& value) const
    {
        applyCommon(section, value);

        value.setLabel(
            data.getStringOr(
                section,
                "checkbox_label",
                value.getCheckboxLabel()
            )
        );

        value.setChecked(
            data.getBoolOr(
                section,
                "checked",
                value.isChecked()
            )
        );
    }

    void capture(
        const std::string& section,
        const item_progress_bar& value)
    {
        captureCommon(section, value);

        data.setInt(section, "value", value.getValue());
        data.setInt(section, "min", value.getMinValue());
        data.setInt(section, "max", value.getMaxValue());

        data.setInt(
            section,
            "orientation",
            static_cast<int>(value.getOrientation())
        );

        data.setBool(
            section,
            "show_percent",
            value.getShowPercent()
        );
    }

    void apply(
        const std::string& section,
        item_progress_bar& value) const
    {
        applyCommon(section, value);

        value.setRange(
            data.getIntOr(
                section,
                "min",
                value.getMinValue()
            ),
            data.getIntOr(
                section,
                "max",
                value.getMaxValue()
            )
        );

        value.setOrientation(
            data.getIntOr(
                section,
                "orientation",
                static_cast<int>(value.getOrientation())
            ) == static_cast<int>(item_orientation::VERTICAL)
                ? item_orientation::VERTICAL
                : item_orientation::HORIZONTAL
        );

        value.setShowPercent(
            data.getBoolOr(
                section,
                "show_percent",
                value.getShowPercent()
            )
        );

        value.setValue(
            data.getIntOr(
                section,
                "value",
                value.getValue()
            )
        );
    }

    void capture(
        const std::string& section,
        const item_scrollbar& value)
    {
        captureCommon(section, value);

        data.setInt(section, "position", value.getPosition());
        data.setInt(section, "min", value.getMinValue());
        data.setInt(section, "max", value.getMaxValue());
        data.setInt(section, "page_size", value.getPageSize());
        data.setInt(section, "step", value.getStep());

        data.setInt(
            section,
            "orientation",
            static_cast<int>(value.getOrientation())
        );
    }

    void apply(
        const std::string& section,
        item_scrollbar& value) const
    {
        applyCommon(section, value);

        value.setRange(
            data.getIntOr(
                section,
                "min",
                value.getMinValue()
            ),
            data.getIntOr(
                section,
                "max",
                value.getMaxValue()
            )
        );

        value.setPageSize(
            data.getIntOr(
                section,
                "page_size",
                value.getPageSize()
            )
        );

        value.setStep(
            data.getIntOr(
                section,
                "step",
                value.getStep()
            )
        );

        value.setOrientation(
            data.getIntOr(
                section,
                "orientation",
                static_cast<int>(value.getOrientation())
            ) == static_cast<int>(item_orientation::VERTICAL)
                ? item_orientation::VERTICAL
                : item_orientation::HORIZONTAL
        );

        value.setPosition(
            data.getIntOr(
                section,
                "position",
                value.getPosition()
            )
        );
    }

    void capture(
        const std::string& section,
        const item_led& value)
    {
        captureCommon(section, value);

        data.setInt(
            section,
            "state",
            static_cast<int>(value.getState())
        );

        data.setInt(
            section,
            "shape",
            static_cast<int>(value.getShape())
        );

        data.setString(
            section,
            "led_label",
            value.getLedLabel()
        );
    }

    void apply(
        const std::string& section,
        item_led& value) const
    {
        applyCommon(section, value);

        int state =
            data.getIntOr(
                section,
                "state",
                static_cast<int>(value.getState())
            );

        if(state < static_cast<int>(item_led_state::OFF))
            state = static_cast<int>(item_led_state::OFF);

        if(state > static_cast<int>(item_led_state::ERROR))
            state = static_cast<int>(item_led_state::ERROR);

        value.setState(
            static_cast<item_led_state>(state)
        );

        int shape =
            data.getIntOr(
                section,
                "shape",
                static_cast<int>(value.getShape())
            );

        value.setShape(
            shape == static_cast<int>(item_led_shape::RECTANGLE)
                ? item_led_shape::RECTANGLE
                : item_led_shape::CIRCLE
        );

        value.setLabel(
            data.getStringOr(
                section,
                "led_label",
                value.getLedLabel()
            )
        );
    }

    void capture(
        const std::string& section,
        const item_color_wheel& value)
    {
        captureCommon(section, value);

        data.setDouble(
            section,
            "hue",
            value.getHue(),
            3
        );

        data.setDouble(
            section,
            "step",
            value.getStep(),
            3
        );

        data.setDouble(
            section,
            "ring_ratio",
            value.getRingRatio(),
            3
        );
    }

    void apply(
        const std::string& section,
        item_color_wheel& value) const
    {
        applyCommon(section, value);

        value.setStep(
            static_cast<float>(
                data.getDoubleOr(
                    section,
                    "step",
                    value.getStep()
                )
            )
        );

        value.setRingRatio(
            static_cast<float>(
                data.getDoubleOr(
                    section,
                    "ring_ratio",
                    value.getRingRatio()
                )
            )
        );

        value.setHue(
            static_cast<float>(
                data.getDoubleOr(
                    section,
                    "hue",
                    value.getHue()
                )
            )
        );
    }

    void capture(
        const std::string& section,
        const item_gauge& value)
    {
        captureCommon(section, value);

        data.setInt(section, "value", value.getValue());
        data.setInt(section, "min", value.getMinValue());
        data.setInt(section, "max", value.getMaxValue());

        data.setDouble(
            section,
            "start_angle",
            value.getStartAngle(),
            3
        );

        data.setDouble(
            section,
            "end_angle",
            value.getEndAngle(),
            3
        );

        data.setUInt(
            section,
            "major_divisions",
            value.getMajorDivisions()
        );

        data.setUInt(
            section,
            "minor_divisions",
            value.getMinorDivisions()
        );

        data.setString(
            section,
            "unit",
            value.getUnit()
        );

        data.setBool(
            section,
            "show_value",
            value.getShowValue()
        );

        data.setBool(
            section,
            "show_min_max",
            value.getShowMinMax()
        );

        const std::vector<gauge_zone>& zones =
            value.getZones();

        data.setUInt(
            section,
            "zone_count",
            static_cast<unsigned int>(
                zones.size()
            )
        );

        for(unsigned int i = 0;
            i < zones.size();
            ++i)
        {
            std::string prefix =
                "zone." +
                formatUInt(i) +
                ".";

            data.setInt(
                section,
                prefix + "min",
                zones[i].minValue
            );

            data.setInt(
                section,
                prefix + "max",
                zones[i].maxValue
            );

            data.setInt(
                section,
                prefix + "r",
                zones[i].color.getR()
            );

            data.setInt(
                section,
                prefix + "g",
                zones[i].color.getG()
            );

            data.setInt(
                section,
                prefix + "b",
                zones[i].color.getB()
            );
        }
    }

    void apply(
        const std::string& section,
        item_gauge& value) const
    {
        applyCommon(section, value);

        value.setRange(
            data.getIntOr(
                section,
                "min",
                value.getMinValue()
            ),
            data.getIntOr(
                section,
                "max",
                value.getMaxValue()
            )
        );

        value.setAngles(
            static_cast<float>(
                data.getDoubleOr(
                    section,
                    "start_angle",
                    value.getStartAngle()
                )
            ),
            static_cast<float>(
                data.getDoubleOr(
                    section,
                    "end_angle",
                    value.getEndAngle()
                )
            )
        );

        value.setGraduations(
            data.getUIntOr(
                section,
                "major_divisions",
                value.getMajorDivisions()
            ),
            data.getUIntOr(
                section,
                "minor_divisions",
                value.getMinorDivisions()
            )
        );

        value.setUnit(
            data.getStringOr(
                section,
                "unit",
                value.getUnit()
            )
        );

        value.setShowValue(
            data.getBoolOr(
                section,
                "show_value",
                value.getShowValue()
            )
        );

        value.setShowMinMax(
            data.getBoolOr(
                section,
                "show_min_max",
                value.getShowMinMax()
            )
        );

        value.clearZones();

        unsigned int zoneCount =
            data.getUIntOr(
                section,
                "zone_count",
                0
            );

        for(unsigned int i = 0;
            i < zoneCount;
            ++i)
        {
            std::string prefix =
                "zone." +
                formatUInt(i) +
                ".";

            int zoneMin =
                data.getIntOr(
                    section,
                    prefix + "min",
                    value.getMinValue()
                );

            int zoneMax =
                data.getIntOr(
                    section,
                    prefix + "max",
                    value.getMaxValue()
                );

            int r =
                data.getIntOr(
                    section,
                    prefix + "r",
                    0
                );

            int g =
                data.getIntOr(
                    section,
                    prefix + "g",
                    180
                );

            int b =
                data.getIntOr(
                    section,
                    prefix + "b",
                    0
                );

            value.addZone(
                zoneMin,
                zoneMax,
                Class_color(r, g, b)
            );
        }

        value.setValue(
            data.getIntOr(
                section,
                "value",
                value.getValue()
            )
        );
    }

    void capture(
        const std::string& section,
        const item_dial& value)
    {
        capture(
            section,
            static_cast<const item_gauge&>(
                value
            )
        );

        data.setInt(
            section,
            "step",
            value.getStep()
        );
    }

    void apply(
        const std::string& section,
        item_dial& value) const
    {
        apply(
            section,
            static_cast<item_gauge&>(
                value
            )
        );

        value.setStep(
            data.getIntOr(
                section,
                "step",
                value.getStep()
            )
        );
    }

    void capture(
        const std::string& section,
        const item_tab& value)
    {
        captureCommon(section, value);

        data.setInt(
            section,
            "selected",
            value.getSelectedTab()
        );
    }

    void apply(
        const std::string& section,
        item_tab& value) const
    {
        applyCommon(section, value);

        value.setSelectedTab(
            data.getIntOr(
                section,
                "selected",
                value.getSelectedTab()
            )
        );
    }

    void capture(
        const std::string& section,
        const item_menu_bar& value)
    {
        captureCommon(section, value);

        data.setInt(
            section,
            "selected",
            value.getSelectedMenu()
        );
    }

    void apply(
        const std::string& section,
        item_menu_bar& value) const
    {
        applyCommon(section, value);

        value.setSelectedMenu(
            data.getIntOr(
                section,
                "selected",
                value.getSelectedMenu()
            )
        );
    }

    void capture(
        const std::string& section,
        const item_canvas& value)
    {
        captureCommon(section, value);

        data.setBool(
            section,
            "background_enabled",
            value.getBackgroundEnabled()
        );

        data.setBool(
            section,
            "border_enabled",
            value.getBorderEnabled()
        );

        data.setInt(
            section,
            "border_size",
            value.getCanvasBorderSize()
        );

        data.setInt(
            section,
            "background_r",
            value.getBackgroundColor().getR()
        );

        data.setInt(
            section,
            "background_g",
            value.getBackgroundColor().getG()
        );

        data.setInt(
            section,
            "background_b",
            value.getBackgroundColor().getB()
        );

        data.setInt(
            section,
            "border_r",
            value.getCanvasBorderColor().getR()
        );

        data.setInt(
            section,
            "border_g",
            value.getCanvasBorderColor().getG()
        );

        data.setInt(
            section,
            "border_b",
            value.getCanvasBorderColor().getB()
        );
    }

    void apply(
        const std::string& section,
        item_canvas& value) const
    {
        applyCommon(section, value);

        value.setBackgroundEnabled(
            data.getBoolOr(
                section,
                "background_enabled",
                value.getBackgroundEnabled()
            )
        );

        value.setBorderEnabled(
            data.getBoolOr(
                section,
                "border_enabled",
                value.getBorderEnabled()
            )
        );

        value.setCanvasBorderSize(
            data.getIntOr(
                section,
                "border_size",
                value.getCanvasBorderSize()
            )
        );

        value.setBackgroundColor(
            Class_color(
                data.getIntOr(
                    section,
                    "background_r",
                    value.getBackgroundColor().getR()
                ),
                data.getIntOr(
                    section,
                    "background_g",
                    value.getBackgroundColor().getG()
                ),
                data.getIntOr(
                    section,
                    "background_b",
                    value.getBackgroundColor().getB()
                )
            )
        );

        value.setCanvasBorderColor(
            Class_color(
                data.getIntOr(
                    section,
                    "border_r",
                    value.getCanvasBorderColor().getR()
                ),
                data.getIntOr(
                    section,
                    "border_g",
                    value.getCanvasBorderColor().getG()
                ),
                data.getIntOr(
                    section,
                    "border_b",
                    value.getCanvasBorderColor().getB()
                )
            )
        );
    }

    void capture(
        const std::string& section,
        const item_text_label& value)
    {
        captureCommon(section, value);

        data.setString(
            section,
            "text",
            value.getText()
        );

        data.setInt(
            section,
            "alignment",
            static_cast<int>(
                value.getAlignment()
            )
        );

        data.setBool(
            section,
            "opaque",
            value.isOpaque()
        );
    }

    void apply(
        const std::string& section,
        item_text_label& value) const
    {
        applyCommon(section, value);

        value.setText(
            data.getStringOr(
                section,
                "text",
                value.getText()
            )
        );

        value.setAlignment(
            static_cast<item_text_align>(
                data.getIntOr(
                    section,
                    "alignment",
                    static_cast<int>(
                        value.getAlignment()
                    )
                )
            )
        );

        value.setOpaque(
            data.getBoolOr(
                section,
                "opaque",
                value.isOpaque()
            )
        );
    }

    void capture(
        const std::string& section,
        const item_separator& value)
    {
        captureCommon(section, value);

        data.setInt(
            section,
            "orientation",
            static_cast<int>(
                value.getOrientation()
            )
        );

        data.setInt(
            section,
            "thickness",
            value.getThickness()
        );
    }

    void apply(
        const std::string& section,
        item_separator& value) const
    {
        applyCommon(section, value);

        value.setOrientation(
            static_cast<item_orientation>(
                data.getIntOr(
                    section,
                    "orientation",
                    static_cast<int>(
                        value.getOrientation()
                    )
                )
            )
        );

        value.setThickness(
            data.getIntOr(
                section,
                "thickness",
                value.getThickness()
            )
        );
    }

    void capture(
        const std::string& section,
        const item_status_bar& value)
    {
        captureCommon(section, value);

        data.setString(
            section,
            "left",
            value.getLeftText()
        );

        data.setString(
            section,
            "center",
            value.getCenterText()
        );

        data.setString(
            section,
            "right",
            value.getRightText()
        );

        data.setInt(
            section,
            "status",
            static_cast<int>(
                value.getStatus()
            )
        );
    }

    void apply(
        const std::string& section,
        item_status_bar& value) const
    {
        applyCommon(section, value);

        value.setLeftText(
            data.getStringOr(
                section,
                "left",
                value.getLeftText()
            )
        );

        value.setCenterText(
            data.getStringOr(
                section,
                "center",
                value.getCenterText()
            )
        );

        value.setRightText(
            data.getStringOr(
                section,
                "right",
                value.getRightText()
            )
        );

        value.setStatus(
            static_cast<item_led_state>(
                data.getIntOr(
                    section,
                    "status",
                    static_cast<int>(
                        value.getStatus()
                    )
                )
            ),
            value.getStatus() != item_led_state::OFF
        );
    }

    void capture(
        const std::string& section,
        const item_container& value)
    {
        captureCommon(section, value);

        data.setInt(
            section,
            "layout_mode",
            static_cast<int>(
                value.getLayoutMode()
            )
        );

        data.setInt(
            section,
            "padding",
            value.getPadding()
        );

        data.setInt(
            section,
            "gap",
            value.getGap()
        );

        data.setInt(
            section,
            "grid_columns",
            value.getGridColumns()
        );

        data.setString(
            section,
            "title",
            value.getContainerTitle()
        );
    }

    void apply(
        const std::string& section,
        item_container& value) const
    {
        applyCommon(section, value);

        value.setLayoutMode(
            static_cast<item_layout_mode>(
                data.getIntOr(
                    section,
                    "layout_mode",
                    static_cast<int>(
                        value.getLayoutMode()
                    )
                )
            )
        );

        value.setPadding(
            data.getIntOr(
                section,
                "padding",
                value.getPadding()
            )
        );

        value.setGap(
            data.getIntOr(
                section,
                "gap",
                value.getGap()
            )
        );

        value.setGridColumns(
            data.getIntOr(
                section,
                "grid_columns",
                value.getGridColumns()
            )
        );

        value.setContainerTitle(
            data.getStringOr(
                section,
                "title",
                value.getContainerTitle()
            )
        );
    }

    void capture(
        const std::string& section,
        const item_scroll_view& value)
    {
        capture(
            section,
            static_cast<const item_container&>(
                value
            )
        );

        data.setInt(
            section,
            "scroll_x",
            value.getScrollX()
        );

        data.setInt(
            section,
            "scroll_y",
            value.getScrollY()
        );

        data.setInt(
            section,
            "scroll_step",
            value.getScrollStep()
        );

        data.setBool(
            section,
            "show_scrollbars",
            value.getShowScrollbars()
        );
    }

    void apply(
        const std::string& section,
        item_scroll_view& value) const
    {
        apply(
            section,
            static_cast<item_container&>(
                value
            )
        );

        value.setScrollStep(
            data.getIntOr(
                section,
                "scroll_step",
                value.getScrollStep()
            )
        );

        value.setShowScrollbars(
            data.getBoolOr(
                section,
                "show_scrollbars",
                value.getShowScrollbars()
            )
        );

        value.setScroll(
            data.getIntOr(
                section,
                "scroll_x",
                value.getScrollX()
            ),
            data.getIntOr(
                section,
                "scroll_y",
                value.getScrollY()
            )
        );
    }

    void capture(
        const std::string& section,
        const item_tab_view& value)
    {
        capture(
            section,
            static_cast<const item_container&>(
                value
            )
        );

        data.setStringList(
            section,
            "tabs",
            value.getTabs()
        );

        data.setInt(
            section,
            "selected_tab",
            value.getSelectedTab()
        );

        data.setInt(
            section,
            "header_height",
            value.getHeaderHeight()
        );
    }

    void apply(
        const std::string& section,
        item_tab_view& value) const
    {
        apply(
            section,
            static_cast<item_container&>(
                value
            )
        );

        std::vector<std::string> tabs;

        if(data.getStringList(
            section,
            "tabs",
            tabs))
        {
            value.setTabs(tabs);
        }

        value.setHeaderHeight(
            data.getIntOr(
                section,
                "header_height",
                value.getHeaderHeight()
            )
        );

        value.setSelectedTab(
            data.getIntOr(
                section,
                "selected_tab",
                value.getSelectedTab()
            ),
            false
        );
    }

    void capture(
        const std::string& section,
        const item_radio_button& value)
    {
        captureCommon(section, value);

        data.setBool(section, "checked", value.isChecked());
        data.setString(section, "label", value.getRadioLabel());
    }

    void apply(
        const std::string& section,
        item_radio_button& value) const
    {
        applyCommon(section, value);

        value.setLabel(
            data.getStringOr(
                section,
                "label",
                value.getRadioLabel()
            )
        );

        value.setChecked(
            data.getBoolOr(
                section,
                "checked",
                value.isChecked()
            ),
            false
        );
    }

    void capture(
        const std::string& section,
        const item_radio_group& value)
    {
        captureCommon(section, value);

        data.setStringList(section, "items", value.getItems());
        data.setInt(section, "selected", value.getSelectedIndex());
        data.setInt(section, "row_height", value.getRowHeight());
    }

    void apply(
        const std::string& section,
        item_radio_group& value) const
    {
        applyCommon(section, value);

        std::vector<std::string> items;

        if(data.getStringList(section, "items", items))
            value.setItems(items);

        value.setRowHeight(
            data.getIntOr(
                section,
                "row_height",
                value.getRowHeight()
            )
        );

        value.setSelectedIndex(
            data.getIntOr(
                section,
                "selected",
                value.getSelectedIndex()
            ),
            false
        );
    }

    void capture(
        const std::string& section,
        const item_list_box& value)
    {
        captureCommon(section, value);

        data.setStringList(section, "items", value.getItems());
        data.setInt(section, "selected", value.getSelectedIndex());
        data.setInt(section, "scroll_offset", value.getScrollOffset());
        data.setInt(section, "row_height", value.getRowHeight());
        data.setBool(section, "show_scrollbar", value.getShowScrollbar());
    }

    void apply(
        const std::string& section,
        item_list_box& value) const
    {
        applyCommon(section, value);

        std::vector<std::string> items;

        if(data.getStringList(section, "items", items))
            value.setItems(items);

        value.setRowHeight(
            data.getIntOr(
                section,
                "row_height",
                value.getRowHeight()
            )
        );

        value.setShowScrollbar(
            data.getBoolOr(
                section,
                "show_scrollbar",
                value.getShowScrollbar()
            )
        );

        value.setSelectedIndex(
            data.getIntOr(
                section,
                "selected",
                value.getSelectedIndex()
            ),
            false
        );

        value.setScrollOffset(
            data.getIntOr(
                section,
                "scroll_offset",
                value.getScrollOffset()
            )
        );
    }

    void capture(
        const std::string& section,
        const item_combo_box& value)
    {
        captureCommon(section, value);

        data.setStringList(section, "items", value.getItems());
        data.setInt(section, "selected", value.getSelectedIndex());
        data.setInt(section, "max_visible_rows", value.getMaxVisibleRows());
    }

    void apply(
        const std::string& section,
        item_combo_box& value) const
    {
        applyCommon(section, value);

        std::vector<std::string> items;

        if(data.getStringList(section, "items", items))
            value.setItems(items);

        value.setMaxVisibleRows(
            data.getIntOr(
                section,
                "max_visible_rows",
                value.getMaxVisibleRows()
            )
        );

        value.setSelectedIndex(
            data.getIntOr(
                section,
                "selected",
                value.getSelectedIndex()
            ),
            false
        );

        value.close();
    }

    void capture(
        const std::string& section,
        const item_tree& value)
    {
        captureCommon(section, value);

        data.setInt(
            section,
            "row_height",
            value.getRowHeight()
        );

        data.setInt(
            section,
            "indent_width",
            value.getIndentWidth()
        );

        data.setBool(
            section,
            "show_lines",
            value.getShowLines()
        );

        data.setBool(
            section,
            "show_scrollbar",
            value.getShowScrollbar()
        );

        data.setInt(
            section,
            "selected",
            value.getSelectedNodeId()
        );

        data.setInt(
            section,
            "scroll_offset",
            value.getScrollOffset()
        );

        std::vector<int> ids =
            value.getAllNodeIds();

        data.setUInt(
            section,
            "node_count",
            static_cast<unsigned int>(
                ids.size()
            )
        );

        for(unsigned int i = 0;
            i < ids.size();
            ++i)
        {
            const tree_node* node =
                value.getNode(ids[i]);

            if(node == nullptr)
                continue;

            std::string prefix =
                "node." +
                formatUInt(i) +
                ".";

            data.setInt(
                section,
                prefix + "id",
                node->id
            );

            data.setInt(
                section,
                prefix + "parent",
                node->parentId
            );

            data.setString(
                section,
                prefix + "label",
                node->label
            );

            data.setBool(
                section,
                prefix + "expanded",
                node->expanded
            );

            data.setBool(
                section,
                prefix + "enabled",
                node->enabled
            );

            data.setBool(
                section,
                prefix + "visible",
                node->visible
            );
        }
    }

    void apply(
        const std::string& section,
        item_tree& value) const
    {
        applyCommon(section, value);

        value.setRowHeight(
            data.getIntOr(
                section,
                "row_height",
                value.getRowHeight()
            )
        );

        value.setIndentWidth(
            data.getIntOr(
                section,
                "indent_width",
                value.getIndentWidth()
            )
        );

        value.setShowLines(
            data.getBoolOr(
                section,
                "show_lines",
                value.getShowLines()
            )
        );

        value.setShowScrollbar(
            data.getBoolOr(
                section,
                "show_scrollbar",
                value.getShowScrollbar()
            )
        );

        value.clear();

        unsigned int nodeCount =
            data.getUIntOr(
                section,
                "node_count",
                0
            );

        std::vector<bool> restored(
            nodeCount,
            false
        );

        unsigned int remaining =
            nodeCount;

        for(unsigned int pass = 0;
            pass < nodeCount + 1 &&
            remaining > 0;
            ++pass)
        {
            bool progress = false;

            for(unsigned int i = 0;
                i < nodeCount;
                ++i)
            {
                if(restored[i])
                    continue;

                std::string prefix =
                    "node." +
                    formatUInt(i) +
                    ".";

                int id =
                    data.getIntOr(
                        section,
                        prefix + "id",
                        -1
                    );

                int parent =
                    data.getIntOr(
                        section,
                        prefix + "parent",
                        -1
                    );

                if(
                    id < 0 ||
                    (parent >= 0 &&
                     !value.hasNode(parent)))
                {
                    continue;
                }

                bool ok =
                    value.addNodeWithId(
                        id,
                        parent,
                        data.getStringOr(
                            section,
                            prefix + "label",
                            ""
                        ),
                        data.getBoolOr(
                            section,
                            prefix + "expanded",
                            false
                        ),
                        nullptr
                    );

                if(!ok)
                    continue;

                value.setNodeEnabled(
                    id,
                    data.getBoolOr(
                        section,
                        prefix + "enabled",
                        true
                    )
                );

                value.setNodeVisible(
                    id,
                    data.getBoolOr(
                        section,
                        prefix + "visible",
                        true
                    )
                );

                restored[i] = true;
                --remaining;
                progress = true;
            }

            if(!progress)
                break;
        }

        int selected =
            data.getIntOr(
                section,
                "selected",
                -1
            );

        if(selected >= 0)
            value.setSelectedNode(selected);

        value.setScrollOffset(
            data.getIntOr(
                section,
                "scroll_offset",
                0
            )
        );
    }

    void capture(
        const std::string& section,
        const item_table& value)
    {
        captureCommon(section, value);

        data.setUInt(
            section,
            "rows",
            value.getRowCount()
        );

        data.setUInt(
            section,
            "columns",
            value.getColumnCount()
        );

        data.setInt(
            section,
            "cell_width",
            value.getCellWidth()
        );

        data.setInt(
            section,
            "cell_height",
            value.getCellHeight()
        );

        data.setBool(
            section,
            "headers",
            value.getShowHeaders()
        );

        data.setUInt(
            section,
            "selected_row",
            value.getSelectedRow()
        );

        data.setUInt(
            section,
            "selected_column",
            value.getSelectedColumn()
        );

        for(unsigned int column = 0;
            column < value.getColumnCount();
            ++column)
        {
            data.setString(
                section,
                "column." +
                    formatUInt(column) +
                    ".label",
                value.getColumnLabel(column)
            );
        }

        for(unsigned int row = 0;
            row < value.getRowCount();
            ++row)
        {
            for(unsigned int column = 0;
                column < value.getColumnCount();
                ++column)
            {
                data.setString(
                    section,
                    "cell." +
                        formatUInt(row) +
                        "." +
                        formatUInt(column),
                    value.getCell(row, column)
                );
            }
        }
    }

    void apply(
        const std::string& section,
        item_table& value) const
    {
        applyCommon(section, value);

        unsigned int rows =
            data.getUIntOr(
                section,
                "rows",
                value.getRowCount()
            );

        unsigned int columns =
            data.getUIntOr(
                section,
                "columns",
                value.getColumnCount()
            );

        value.resize(rows, columns);

        value.setCellSize(
            data.getIntOr(
                section,
                "cell_width",
                value.getCellWidth()
            ),
            data.getIntOr(
                section,
                "cell_height",
                value.getCellHeight()
            )
        );

        value.setShowHeaders(
            data.getBoolOr(
                section,
                "headers",
                value.getShowHeaders()
            )
        );

        for(unsigned int column = 0;
            column < columns;
            ++column)
        {
            std::string key =
                "column." +
                formatUInt(column) +
                ".label";

            if(data.has(section, key))
            {
                value.setColumnLabel(
                    column,
                    data.getStringOr(
                        section,
                        key,
                        ""
                    )
                );
            }
        }

        for(unsigned int row = 0;
            row < rows;
            ++row)
        {
            for(unsigned int column = 0;
                column < columns;
                ++column)
            {
                std::string key =
                    "cell." +
                    formatUInt(row) +
                    "." +
                    formatUInt(column);

                if(data.has(section, key))
                {
                    value.setCell(
                        row,
                        column,
                        data.getStringOr(
                            section,
                            key,
                            ""
                        )
                    );
                }
            }
        }

        value.setSelectedCell(
            data.getUIntOr(
                section,
                "selected_row",
                0
            ),
            data.getUIntOr(
                section,
                "selected_column",
                0
            )
        );
    }

    void capture(
        const std::string& section,
        const item_graph& value)
    {
        captureCommon(section, value);

        captureAxis(
            section,
            "x",
            value.getXAxis()
        );

        captureAxis(
            section,
            "y",
            value.getYAxis()
        );

        data.setUInt(
            section,
            "series_count",
            value.getSeriesCount()
        );

        for(unsigned int i = 0;
            i < value.getSeriesCount();
            ++i)
        {
            const graph_series* series =
                value.getSeries(i);

            if(series == nullptr)
                continue;

            std::string prefix =
                "series." + formatUInt(i) + ".";

            data.setInt(
                section,
                prefix + "r",
                series->color.getR()
            );

            data.setInt(
                section,
                prefix + "g",
                series->color.getG()
            );

            data.setInt(
                section,
                prefix + "b",
                series->color.getB()
            );

            data.setBool(
                section,
                prefix + "connected",
                series->connected
            );

            data.setBool(
                section,
                prefix + "points",
                series->showPoints
            );

            data.setBool(
                section,
                prefix + "visible",
                series->visible
            );

            data.setUInt(
                section,
                prefix + "count",
                static_cast<unsigned int>(
                    series->points.size()
                )
            );

            for(unsigned int p = 0;
                p < series->points.size();
                ++p)
            {
                data.setDouble(
                    section,
                    prefix +
                        "point." +
                        formatUInt(p) +
                        ".x",
                    series->points[p].x
                );

                data.setDouble(
                    section,
                    prefix +
                        "point." +
                        formatUInt(p) +
                        ".y",
                    series->points[p].y
                );
            }
        }

        data.setUInt(
            section,
            "cursor_count",
            value.getCursorCount()
        );

        data.setInt(
            section,
            "active_cursor",
            value.getActiveCursor()
        );

        data.setBool(
            section,
            "cursor_control",
            value.getCursorControl()
        );

        for(unsigned int i = 0;
            i < value.getCursorCount();
            ++i)
        {
            const graph_cursor* cursor =
                value.getCursor(i);

            if(cursor == nullptr)
                continue;

            std::string prefix =
                "cursor." + formatUInt(i) + ".";

            data.setDouble(section, prefix + "x", cursor->x);
            data.setDouble(section, prefix + "y", cursor->y);

            data.setInt(
                section,
                prefix + "r",
                cursor->color.getR()
            );

            data.setInt(
                section,
                prefix + "g",
                cursor->color.getG()
            );

            data.setInt(
                section,
                prefix + "b",
                cursor->color.getB()
            );

            data.setBool(
                section,
                prefix + "visible",
                cursor->visible
            );

            data.setString(
                section,
                prefix + "label",
                cursor->label
            );

            data.setInt(
                section,
                prefix + "style",
                static_cast<int>(cursor->style)
            );
        }
    }

    void apply(
        const std::string& section,
        item_graph& value) const
    {
        applyCommon(section, value);

        graph_axis x = value.getXAxis();
        graph_axis y = value.getYAxis();

        applyAxis(section, "x", x);
        applyAxis(section, "y", y);

        value.setXAxis(x);
        value.setYAxis(y);

        if(data.has(section, "series_count"))
        {
            value.clearAllSeries();

            unsigned int count =
                data.getUIntOr(
                    section,
                    "series_count",
                    0
                );

            for(unsigned int i = 0;
                i < count;
                ++i)
            {
                std::string prefix =
                    "series." +
                    formatUInt(i) +
                    ".";

                Class_color color(
                    data.getIntOr(
                        section,
                        prefix + "r",
                        0
                    ),
                    data.getIntOr(
                        section,
                        prefix + "g",
                        0
                    ),
                    data.getIntOr(
                        section,
                        prefix + "b",
                        0
                    )
                );

                int index = value.addSeries(
                    color,
                    data.getBoolOr(
                        section,
                        prefix + "connected",
                        true
                    ),
                    data.getBoolOr(
                        section,
                        prefix + "points",
                        false
                    )
                );

                graph_series* series =
                    value.getSeries(
                        static_cast<unsigned int>(index)
                    );

                if(series != nullptr)
                {
                    series->visible =
                        data.getBoolOr(
                            section,
                            prefix + "visible",
                            true
                        );
                }

                unsigned int pointCount =
                    data.getUIntOr(
                        section,
                        prefix + "count",
                        0
                    );

                for(unsigned int p = 0;
                    p < pointCount;
                    ++p)
                {
                    std::string pointPrefix =
                        prefix +
                        "point." +
                        formatUInt(p) +
                        ".";

                    value.addPoint(
                        static_cast<unsigned int>(index),
                        data.getDoubleOr(
                            section,
                            pointPrefix + "x",
                            0.0
                        ),
                        data.getDoubleOr(
                            section,
                            pointPrefix + "y",
                            0.0
                        )
                    );
                }
            }
        }

        if(data.has(section, "cursor_count"))
        {
            value.clearCursors();

            unsigned int count =
                data.getUIntOr(
                    section,
                    "cursor_count",
                    0
                );

            for(unsigned int i = 0;
                i < count;
                ++i)
            {
                std::string prefix =
                    "cursor." +
                    formatUInt(i) +
                    ".";

                graph_cursor_style style =
                    static_cast<graph_cursor_style>(
                        data.getIntOr(
                            section,
                            prefix + "style",
                            static_cast<int>(
                                graph_cursor_style::CROSSHAIR
                            )
                        )
                    );

                int cursorIndex =
                    value.addCursor(
                        data.getDoubleOr(
                            section,
                            prefix + "x",
                            0.0
                        ),
                        data.getDoubleOr(
                            section,
                            prefix + "y",
                            0.0
                        ),
                        Class_color(
                            data.getIntOr(
                                section,
                                prefix + "r",
                                255
                            ),
                            data.getIntOr(
                                section,
                                prefix + "g",
                                0
                            ),
                            data.getIntOr(
                                section,
                                prefix + "b",
                                0
                            )
                        ),
                        data.getStringOr(
                            section,
                            prefix + "label",
                            "C"
                        ),
                        style
                    );

                graph_cursor* cursor =
                    value.getCursor(
                        static_cast<unsigned int>(
                            cursorIndex
                        )
                    );

                if(cursor != nullptr)
                {
                    cursor->visible =
                        data.getBoolOr(
                            section,
                            prefix + "visible",
                            true
                        );
                }
            }

            int active =
                data.getIntOr(
                    section,
                    "active_cursor",
                    -1
                );

            if(active >= 0)
                value.setActiveCursor(
                    static_cast<unsigned int>(active)
                );

            value.setCursorControl(
                data.getBoolOr(
                    section,
                    "cursor_control",
                    false
                )
            );
        }
    }

    void capture(
        const std::string& section,
        const item_options_popup& value)
    {
        captureCommon(section, value);

        const std::vector<option_popup_entry>& options =
            value.getOptions();

        data.setUInt(
            section,
            "option_count",
            static_cast<unsigned int>(options.size())
        );

        for(unsigned int i = 0; i < options.size(); ++i)
        {
            std::string prefix =
                "option." +
                formatUInt(i) +
                ".";

            data.setInt(section, prefix + "id", options[i].id);
            data.setInt(
                section,
                prefix + "type",
                static_cast<int>(options[i].type)
            );
            data.setString(section, prefix + "label", options[i].label);
            data.setBool(section, prefix + "value", options[i].value);
            data.setInt(section, prefix + "radio_group", options[i].radioGroup);
        }
    }

    void apply(
        const std::string& section,
        item_options_popup& value) const
    {
        applyCommon(section, value);

        unsigned int count =
            data.getUIntOr(
                section,
                "option_count",
                0
            );

        for(unsigned int i = 0; i < count; ++i)
        {
            std::string prefix =
                "option." +
                formatUInt(i) +
                ".";

            int id =
                data.getIntOr(
                    section,
                    prefix + "id",
                    -1
                );

            if(id < 0 || !value.hasOption(id))
                continue;

            value.setValue(
                id,
                data.getBoolOr(
                    section,
                    prefix + "value",
                    value.getValue(id)
                )
            );
        }
    }

    void capture(
        const std::string& section,
        const item_popup& value)
    {
        captureCommon(section, value);

        data.setString(
            section,
            "title",
            value.getTitle()
        );

        data.setString(
            section,
            "message",
            value.getMessage()
        );

        data.setBool(
            section,
            "open",
            value.isOpen()
        );
    }

    void apply(
        const std::string& section,
        item_popup& value) const
    {
        applyCommon(section, value);

        value.setTitle(
            data.getStringOr(
                section,
                "title",
                value.getTitle()
            )
        );

        value.setMessage(
            data.getStringOr(
                section,
                "message",
                value.getMessage()
            )
        );

        bool opened =
            data.getBoolOr(
                section,
                "open",
                value.isOpen()
            );

        if(opened)
            value.open();
        else
            value.close();
    }

private:
    IniFile data;

    void captureCommon(
        const std::string& section,
        const item& value)
    {
        data.setInt(section, "x", value.getX());
        data.setInt(section, "y", value.getY());
        data.setInt(section, "w", value.getW());
        data.setInt(section, "h", value.getH());

        data.setInt(
            section,
            "z",
            value.getZOrder()
        );

        data.setInt(
            section,
            "page",
            value.getPageId()
        );

        data.setBool(
            section,
            "visible",
            value.isVisible()
        );

        data.setBool(
            section,
            "dimmed",
            value.isDimmed()
        );

        data.setString(
            section,
            "label_on",
            value.getLabelOn()
        );

        data.setString(
            section,
            "label_off",
            value.getLabelOff()
        );

        data.setString(
            section,
            "label_hover_on",
            value.getLabelOnHover()
        );

        data.setString(
            section,
            "label_hover_off",
            value.getLabelOffHover()
        );
    }

    void applyCommon(
        const std::string& section,
        item& value) const
    {
        value.setGeometry(
            STRUCT_pos{
                data.getIntOr(
                    section,
                    "x",
                    value.getX()
                ),
                data.getIntOr(
                    section,
                    "y",
                    value.getY()
                ),
                data.getIntOr(
                    section,
                    "w",
                    value.getW()
                ),
                data.getIntOr(
                    section,
                    "h",
                    value.getH()
                )
            }
        );

        value.setZOrder(
            data.getIntOr(
                section,
                "z",
                value.getZOrder()
            )
        );

        value.setPageId(
            data.getIntOr(
                section,
                "page",
                value.getPageId()
            )
        );

        value.setItemVisible(
            data.getBoolOr(
                section,
                "visible",
                value.isVisible()
            )
        );

        value.setItemDimmed(
            data.getBoolOr(
                section,
                "dimmed",
                value.isDimmed()
            )
        );

        value.setLabelOn(
            data.getStringOr(
                section,
                "label_on",
                value.getLabelOn()
            )
        );

        value.setLabelOff(
            data.getStringOr(
                section,
                "label_off",
                value.getLabelOff()
            )
        );

        value.setLabelOnHover(
            data.getStringOr(
                section,
                "label_hover_on",
                value.getLabelOnHover()
            )
        );

        value.setLabelOffHover(
            data.getStringOr(
                section,
                "label_hover_off",
                value.getLabelOffHover()
            )
        );
    }

    void captureAxis(
        const std::string& section,
        const std::string& prefix,
        const graph_axis& axis)
    {
        data.setDouble(
            section,
            prefix + ".min",
            axis.minValue
        );

        data.setDouble(
            section,
            prefix + ".max",
            axis.maxValue
        );

        data.setInt(
            section,
            prefix + ".scale",
            static_cast<int>(axis.scale)
        );

        data.setInt(
            section,
            prefix + ".divisions",
            axis.divisions
        );

        data.setBool(
            section,
            prefix + ".axis",
            axis.showAxis
        );

        data.setBool(
            section,
            prefix + ".grid",
            axis.showGrid
        );

        data.setString(
            section,
            prefix + ".label",
            axis.label
        );

        data.setBool(
            section,
            prefix + ".graduations",
            axis.showGraduations
        );

        data.setInt(
            section,
            prefix + ".precision",
            axis.graduationPrecision
        );
    }

    void applyAxis(
        const std::string& section,
        const std::string& prefix,
        graph_axis& axis) const
    {
        axis.minValue =
            data.getDoubleOr(
                section,
                prefix + ".min",
                axis.minValue
            );

        axis.maxValue =
            data.getDoubleOr(
                section,
                prefix + ".max",
                axis.maxValue
            );

        axis.scale =
            static_cast<graph_scale>(
                data.getIntOr(
                    section,
                    prefix + ".scale",
                    static_cast<int>(
                        axis.scale
                    )
                )
            );

        axis.divisions =
            data.getIntOr(
                section,
                prefix + ".divisions",
                axis.divisions
            );

        axis.showAxis =
            data.getBoolOr(
                section,
                prefix + ".axis",
                axis.showAxis
            );

        axis.showGrid =
            data.getBoolOr(
                section,
                prefix + ".grid",
                axis.showGrid
            );

        axis.label =
            data.getStringOr(
                section,
                prefix + ".label",
                axis.label
            );

        axis.showGraduations =
            data.getBoolOr(
                section,
                prefix + ".graduations",
                axis.showGraduations
            );

        axis.graduationPrecision =
            data.getIntOr(
                section,
                prefix + ".precision",
                axis.graduationPrecision
            );
    }
};

using ConfigFile = IniFile;
using SaveFile = IniFile;

} // namespace casio_storage

#endif // CASIO_STORAGE_HPP
