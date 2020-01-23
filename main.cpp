#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <string>

#include <ShaderDebugger/Utils.h>
#include <ShaderDebugger/Texture.h>
#include <ShaderDebugger/GLSLLibrary.h>
#include <ShaderDebugger/GLSLTranslator.h>
#include <ShaderDebugger/HLSLTranslator.h>
#include <ShaderDebugger/ShaderDebugger.h>


std::vector<std::string> SplitString(const std::string& str, const std::string& dlm)
{
    std::vector<std::string> ret;
    size_t pos = 0, prev = 0;
    while ((pos = str.find(dlm, prev)) != std::string::npos)
    {
        ret.push_back(str.substr(prev, pos - prev));
        prev = pos + 1;
    }

    ret.push_back(str.substr(prev));
    return ret;
}

void OutputVariableValue(bv_variable* val)
{
	if (val == nullptr)
		return;

	if (val->type == bv_type_float)
		printf("%.4f\n", sd::AsFloat(*val));
	else if (val->type == bv_type_int)
		printf("%d\n", sd::AsInteger(*val));
	else if (val->type == bv_type_uint)
		printf("%u\n", sd::AsUnsignedInteger(*val));
	else if (val->type == bv_type_uchar || val->type == bv_type_char)
		printf("%d\n", (int)sd::AsBool(*val));
	else if (val->type == bv_type_object) {
		bv_object* obj = bv_variable_get_object(*val);
		std::string objtypeName(obj->type->name);

		// vectors
		// do additional checks... the user might have defined structure zvec5... you never know :P
		bool isVector = false;
		size_t vecPos = objtypeName.find("vec");
		if (vecPos != std::string::npos && (objtypeName.size() == 4 || objtypeName.size() == 5)) // vecX or gvecX
		{
			const char* fieldNames = "xyzw";

			u8 vecLen = sd::GetVectorSizeFromName(objtypeName.c_str());
			if (vecPos == 0 && !isdigit(objtypeName[3]))
				vecLen = -1;
			else if (vecPos == 1 && !isdigit(objtypeName[4]))
				vecLen = -1;

			if (vecLen >= 2 && vecLen <= 4) {
				bv_type vecType = sd::GetVectorTypeFromName(objtypeName.c_str());

				char chType = objtypeName[0];
				if (vecPos == 1 && chType != 'b' && chType != 'i' && chType != 'u' && chType != 'd')
					vecType = bv_type_void;

				// actual vector
				if (vecType != bv_type_void) {
					if (vecType == bv_type_float) {
						glm::vec4 vecVal(0.0f);

						switch (vecLen) {
						case 2: vecVal = glm::vec4(sd::AsVector<2, float>(*val), 0.0f, 0.0f); break;
						case 3: vecVal = glm::vec4(sd::AsVector<3, float>(*val), 0.0f); break;
						case 4: vecVal = sd::AsVector<4, float>(*val); break;
						}

						for (u8 i = 0; i < vecLen; i++)
							printf("%c=%.2f ", fieldNames[i], vecVal[i]);
						printf("\n");
					}
					else if (vecType == bv_type_int) {
						glm::ivec4 vecVal(0);

						switch (vecLen) {
						case 2: vecVal = glm::ivec4(sd::AsVector<2, int>(*val), 0, 0); break;
						case 3: vecVal = glm::ivec4(sd::AsVector<3, int>(*val), 0); break;
						case 4: vecVal = sd::AsVector<4, int>(*val); break;
						}

						for (u8 i = 0; i < vecLen; i++)
							printf("%c=%d ", fieldNames[i], vecVal[i]);
						printf("\n");
					}
					else if (vecType == bv_type_uint) {
						glm::uvec4 vecVal(0u);

						switch (vecLen) {
						case 2: vecVal = glm::uvec4(sd::AsVector<2, unsigned int>(*val), 0u, 0u); break;
						case 3: vecVal = glm::uvec4(sd::AsVector<3, unsigned int>(*val), 0u); break;
						case 4: vecVal = sd::AsVector<4, unsigned int>(*val); break;
						}

						for (u8 i = 0; i < vecLen; i++)
							printf("%c=%u ", fieldNames[i], vecVal[i]);
						printf("\n");
					}
					else if (vecType == bv_type_uchar) {
						glm::bvec4 vecVal(false);

						switch (vecLen) {
						case 2: vecVal = glm::bvec4(sd::AsVector<2, bool>(*val), false, false); break;
						case 3: vecVal = glm::bvec4(sd::AsVector<3, bool>(*val), false); break;
						case 4: vecVal = sd::AsVector<4, bool>(*val); break;
						}

						for (u8 i = 0; i < vecLen; i++)
							printf("%c=%d ", fieldNames[i], (int)vecVal[i]);
						printf("\n");
					}
					isVector = true;
				}
			}
		}

		if (!isVector)
		{
			// matrices
			if (sd::GetMatrixTypeFromName(objtypeName.c_str()) != bv_type_void)
			{
				sd::Matrix mat = sd::AsMatrix(*val);
				for (int x = 0; x < mat.Columns; x++) {
					for (int y = 0; y < mat.Rows; y++)
						printf("%7.2f ", mat.Data[x][y]);
					printf("\n");
				}
			}
			else
			{
				bool isNull = true;
				for (int i = 0; i < obj->type->props.name_count; i++) {
					if (obj->prop[i].type == bv_type_void)
						continue;
					isNull = false;
					printf(".%s = ", obj->type->props.names[i]);
					OutputVariableValue(&obj->prop[i]);
				}

				if (isNull)
					printf("null\n");
			}
		}
	}
}

int main() {
	std::ifstream t("shader.hlsl");
	std::string src((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	t.close();

	sd::ShaderDebugger dbg;
	bool res = dbg.SetSource<sd::HLSLTranslator>(sd::ShaderType::Pixel, src, "main", NULL, sd::Library::GLSL());
	
	if (!res) {
		printf("[ERROR] Failed to compile the shader.\n");
		return 1;
	}

	sd::Texture white;
	white.Allocate(1, 1);
	white.Fill(glm::vec4(0.5f, 0.1f, 0.6f, 0.2f));
	
	// initialize globals
	const auto& globals = dbg.GetTranslator()->GetGlobals();
	for (const auto& global : globals)
	{
		if (global.Storage == sd::Variable::StorageType::Uniform ||
			global.Storage == sd::Variable::StorageType::In)
		{
			if (global.Type == "sampler2D") {
				// TODO: texture loading
				dbg.SetValue(global.Name, "sampler2D", &white);
			} else {
				printf("Please enter value for %s %s: ", global.Type.c_str(), global.Name.c_str());

				if (global.Type == "float") {
					float val;
					scanf("%f", &val);
					dbg.SetValue(global.Name, val);
				}
				else if (global.Type == "vec3") {
					float x, y, z;
					scanf("%f %f %f", &x, &y, &z);
					dbg.SetValue(global.Name, "vec3", glm::vec3(x, y, z));
				}
			}
			// TODO: matrix
		}
	}

	// skip initialization
	dbg.Step();

	std::vector<std::string> srcLines = SplitString(src, "\n");

	// debugger
	bool hasStepped = true;
	while (true) {
		int curLine = dbg.GetCurrentLine()-1;
		if (hasStepped) {
			#ifdef _WIN32
			system("cls"); // I know...
			#else 
			system("clear"); // I know...
			#endif

			int lnBegin = std::max(0, curLine-5);
			int lnEnd = std::min((int)srcLines.size()-1, curLine+5);
			for (int i = lnBegin; i <= lnEnd; i++) {
				char hasBreakpoint = dbg.HasBreakpoint(i+1) ? 'x' : ' ';
				if (i == curLine)
					printf(">>>>%c| %s\n", hasBreakpoint, srcLines[i].c_str());
				else
					printf("%4d%c| %s\n", i+1, hasBreakpoint, srcLines[i].c_str());
			}
			hasStepped = false;
 		}

		printf("> ");
		std::string cmd;
		std::getline(std::cin, cmd);
		std::vector<std::string> tokens = SplitString(cmd, " ");

		if (tokens.size() == 0)
			continue;

		if (tokens[0] == "quit" || tokens[0] == "q")
			break;
		else if (tokens[0] == "step") {
			bool ret = dbg.Step();
			if (!ret) break;
			hasStepped = true;
		}
		else if (tokens[0] == "stepover") {
			bool ret = dbg.StepOver();
			if (!ret) break;
			hasStepped = true;
		}
		else if (tokens[0] == "stepout") {
			bool ret = dbg.StepOut();
			if (!ret) break;
			hasStepped = true;
		}
		else if (tokens[0] == "jump") {
			if (tokens.size() > 1) {
				int lnGoal = std::stoi(tokens[1]);
				dbg.Jump(lnGoal);
				hasStepped = true;
			}
		}
		else if (tokens[0] == "get") { // getlocal name
			if (tokens.size() > 1) {
				bv_variable* val = dbg.GetLocalValue(tokens[1]);
				if (val == nullptr)
					val = dbg.GetValue(tokens[1]);

				OutputVariableValue(val);
			}
		}
		else if (tokens[0] == "imd") { // immediate
			if (tokens.size() > 1) {
				bv_variable val = dbg.Immediate(cmd.substr(4));
				OutputVariableValue(&val);
				bv_variable_deinitialize(&val);
			}
		}
		else if (tokens[0] == "continue") {
			bool res = dbg.Continue();
			if (!res) break;
			hasStepped = true; // update the view
		}
		else if (tokens[0] == "bkpt") {
			if (tokens.size() > 1) {
				int line = std::stoi(tokens[1]);
				dbg.AddBreakpoint(line);
				hasStepped = true; // update the view
			}
		}
		else if (tokens[0] == "cbkpt") {
			if (tokens.size() > 1) {
				int line = std::stoi(tokens[1]);
				
				size_t pos = 0;
				for (size_t i = 0; i < cmd.size(); i++)
					if (isdigit(cmd[i])) {
						pos = i;
						break;
					}
				pos = cmd.find_first_of(' ', pos);

				dbg.AddConditionalBreakpoint(line, cmd.substr(pos));

				hasStepped = true; // update the view
			}
		}
		else if (tokens[0] == "rembkpt") {
			if (tokens.size() > 1) {
				int line = std::stoi(tokens[1]);
				dbg.ClearBreakpoint(line);
			}
			else
				dbg.ClearBreakpoints();
			hasStepped = true;
		}
		else if (tokens[0] == "func")
			printf("%s\n", dbg.GetCurrentFunction().c_str());
		else if (tokens[0] == "fstack") {
			std::vector<std::string> fstack = dbg.GetFunctionStack();
			for (int i = 0; i < fstack.size(); i++) {
				printf("%s", fstack[i].c_str());
				if (i != fstack.size()-1)
					printf(" -> ");
			}
			printf("\n");
		}
	}

	printf("Finished debugging!\n");
	printf("Discarded: %d\n", (int)dbg.IsDiscarded());

	// if (isGLSL) {
		printf("Output:\n");
		for (const auto& gl : globals) {
			if (gl.Storage == sd::Variable::StorageType::Out) {
				printf("\t%s = ", gl.Name.c_str());
				OutputVariableValue(dbg.GetValue(gl.Name));
			}
		}
	// }

	// if (isHLSL) {
		bv_variable retValue = dbg.GetReturnValue();
		if (retValue.type != bv_type_void) {
			printf("Return: ");
			OutputVariableValue(&retValue);
		}
		bv_variable_deinitialize(&retValue);
	// }

    return 0;
}