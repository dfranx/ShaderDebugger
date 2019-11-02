#include <stdio.h>
#include <string.h>
#include <fstream>
#include <string>

#include <ShaderDebugger/GLSLTranslator.h>

extern "C" { 
	#include <BlueVM.h>
}

int main() {
	std::ifstream t("shader.glsl");
	std::string src((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	t.close();

	sd::GLSLTranslator glsl;
	bool status = glsl.Parse(sd::ShaderType::Pixel, src, "main");

	if (!status) {
		printf("[ERROR] Failed to parse the GLSL code.\n");
		return 0;
	}

	std::vector<uint8_t> bytecode = glsl.GetBytecode();

	if (bytecode.size() == 0) {
		printf("[ERROR] No bytecode generated.\n");
		return 0;
	}

	// parse bytecode
	bv_program* prog = bv_program_create(bytecode.data());

	if (prog == nullptr) {
		printf("[ERROR] Invalid bytecode.\n");
		return 0;
	}

	// get main() function from loaded file
	bv_function* func_main = bv_program_get_function(prog, "main");

	// check if it exists
	if (func_main == NULL)
		printf("Program is missing function 'main'.\n");
	else {
		// call function and store its return value
		bv_variable ret = bv_program_call(prog, func_main, NULL, NULL);

		// print the value
		printf("main()\n");
		
		// we have to deinitialize returned variables
		bv_variable_deinitialize(&ret);
	}

	// free memory
	bv_program_delete(prog);
	
    return 0;
}