#include <ShaderDebugger/ShaderConverter.h>
#include <fstream>

namespace sd
{
    bool ShaderConverter::ConvertFromFile(const std::string& filename)
    {
		//Load HLSL into a string
		std::ifstream file(filename);

		if (!file.is_open())
            return false;

		std::string inputSrc((std::istreambuf_iterator<char>(file)),
							std::istreambuf_iterator<char>());

        return ConvertFromSource(inputSrc);
    }
    bool ShaderConverter::ConvertFromSource(const std::string& src)
    {
        return false;
    }
}