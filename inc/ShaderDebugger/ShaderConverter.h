#pragma once
#include <string>

namespace sd
{
    /*
        ShaderConverter - convert shader code to BlueVM bytecode
    */
    class ShaderConverter
    {
    public:
        bool ConvertFromFile(const std::string& filename);
        bool ConvertFromSource(const std::string& src);
    };
}