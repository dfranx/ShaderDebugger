#include <stdio.h>
#include <string.h>
#include <fstream>
#include <string>

#include <ShaderDebugger/GLSLTranslator.h>
#include <ShaderDebugger/ShaderDebugger.h>

extern "C" { 
	#include <BlueVM.h>
}

int main() {
	std::ifstream t("shader.glsl");
	std::string src((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	t.close();

	sd::ShaderDebugger dbg;
	dbg.SetSource<sd::GLSLTranslator>(sd::ShaderType::Pixel, src, "main", nullptr);
	bv_variable ret = dbg.Execute();
	
	bv_variable* outColor = dbg.GetValue("outColor");
	printf("outColor = %.4f\n", bv_variable_get_float(*outColor));

	bv_variable_deinitialize(&ret);

    return 0;
}