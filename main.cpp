#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <string>

#include <ShaderDebugger/Utils.h>
#include <ShaderDebugger/GLSLLibrary.h>
#include <ShaderDebugger/GLSLTranslator.h>
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

int main() {
	std::ifstream t("shader.glsl");
	std::string src((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	t.close();

	sd::ShaderDebugger dbg;
	dbg.SetSource<sd::GLSLTranslator>(sd::ShaderType::Pixel, src, "main", sd::Library::GLSL());
	dbg.SetValue("iFactor", 0.7f);

	// TODO: it crashes if iColor is not used but declared
	// dbg.SetValue("iColor", "vec3", glm::vec3(0.5f, 0.6f, 0.7f));

	// TODO: dbg.GetGlobalList() -> "Please enter value for iFactor"
	// TODO: iColor is == vec3(0) for some reason

	std::vector<std::string> srcLines = SplitString(src, "\n");

	bool hasStepped = true;
	while (true) {
		int curLine = dbg.GetCurrentLine()-1;
		if (hasStepped) {
			#ifdef _WIN32
			system("cls"); // I know...
			#else 
			system("clear"); // I know...
			#endif

			int lnBegin = std::max(0, curLine-3);
			int lnEnd = std::min((int)srcLines.size(), curLine+3);
			for (int i = lnBegin; i < lnEnd; i++) {
				if (i == curLine)
					printf(">>>> | %s\n", srcLines[i].c_str());
				else
					printf("%4d | %s\n", i, srcLines[i].c_str());
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
			if (!ret) {
				printf("Finished debugging!\n");
				break;
			}
			hasStepped = true;
		}
		else if (tokens[0] == "jump") {
			if (tokens.size() > 1) {
				int lnGoal = std::stoi(tokens[1]);
				while (dbg.Step()) {
					if (dbg.GetCurrentLine()-1 == lnGoal)
						break;
				}
				hasStepped = true;
			}
		}
		else if (tokens[0] == "getlocal" || tokens[0] == "getglobal") { // getlocal name
			if (tokens.size() > 1) {
				bv_variable* val = nullptr;
				if (tokens[0] == "getlocal")
					val = dbg.GetLocalValue(tokens[1]);
				else if (tokens[0] == "getglobal")
					val = dbg.GetValue(tokens[1]);

				if (val != nullptr) {
					if (val->type == bv_type_float)
						printf("%.4f\n", sd::AsFloat(*val));
					else if (val->type == bv_type_int)
						printf("%d\n", sd::AsInteger(*val));
					else if (val->type == bv_type_object) {
						bv_object* obj = bv_variable_get_object(*val);
						if (strcmp(obj->type->name, "vec3") == 0) {
							glm::vec3 vecval = sd::AsVec3(*val);
							printf("%.4f %.4f %.4f\n", vecval.x, vecval.y, vecval.z);
						}
					}
				}
			}
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

    return 0;
}