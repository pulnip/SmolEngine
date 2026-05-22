#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "StringUtil.hpp"

namespace Smol
{
    Str toUpper(StrView view){
        Str upper(view);
        std::transform(view.begin(), view.end(), upper.begin(),
            [](auto c){ return std::toupper(c); }
        );
        return upper;
    }

    Str readFileAsString(const std::filesystem::path& path){
        std::ifstream file(path);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    std::vector<u8> readFileAsBinary(const std::filesystem::path& path){
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if(!file.is_open())
            throw std::runtime_error("Failed to open file: " + path.string());

        auto size = file.tellg();
        file.seekg(0);

        std::vector<u8> buffer(size);
        file.read(reinterpret_cast<char*>(buffer.data()), size);

        return buffer;
    }
}
