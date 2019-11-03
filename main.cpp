#include <stdio.h>
#include <string.h>
#include <fstream>
#include <string>

#include <ShaderDebugger/Utils.h>
#include <ShaderDebugger/GLSLLibrary.h>
#include <ShaderDebugger/GLSLTranslator.h>
#include <ShaderDebugger/ShaderDebugger.h>

int main() {
	std::ifstream t("shader.glsl");
	std::string src((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	t.close();

	bv_library* libGLSL = sd::Library::GLSL();

	sd::ShaderDebugger dbg;
	dbg.SetSource<sd::GLSLTranslator>(sd::ShaderType::Pixel, src, "main", libGLSL);
	dbg.SetValue("iFactor", 0.7f);
	dbg.SetValue("iColor", "vec3", glm::vec3(0.5f, 0.6f, 0.7f));

	bv_variable ret = dbg.Execute();
	
	glm::vec3 outColor = sd::AsVec3(*dbg.GetValue("outColor"));
	printf("outColor = vec3(%.2f, %.2f, %.2f);\n", outColor.x, outColor.y, outColor.z);
	
	bv_variable_deinitialize(&ret);

	bv_library_delete(libGLSL);

    return 0;
}