#include <iostream>

#include <ShaderDebugger/BVMGenerator.h>
#include <glslang/glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/StandAlone/DirStackFileIncluder.h>


const TBuiltInResource DefaultTBuiltInResource = {
	/* .MaxLights = */ 32,
	/* .MaxClipPlanes = */ 6,
	/* .MaxTextureUnits = */ 32,
	/* .MaxTextureCoords = */ 32,
	/* .MaxVertexAttribs = */ 64,
	/* .MaxVertexUniformComponents = */ 4096,
	/* .MaxVaryingFloats = */ 64,
	/* .MaxVertexTextureImageUnits = */ 32,
	/* .MaxCombinedTextureImageUnits = */ 80,
	/* .MaxTextureImageUnits = */ 32,
	/* .MaxFragmentUniformComponents = */ 4096,
	/* .MaxDrawBuffers = */ 32,
	/* .MaxVertexUniformVectors = */ 128,
	/* .MaxVaryingVectors = */ 8,
	/* .MaxFragmentUniformVectors = */ 16,
	/* .MaxVertexOutputVectors = */ 16,
	/* .MaxFragmentInputVectors = */ 15,
	/* .MinProgramTexelOffset = */ -8,
	/* .MaxProgramTexelOffset = */ 7,
	/* .MaxClipDistances = */ 8,
	/* .MaxComputeWorkGroupCountX = */ 65535,
	/* .MaxComputeWorkGroupCountY = */ 65535,
	/* .MaxComputeWorkGroupCountZ = */ 65535,
	/* .MaxComputeWorkGroupSizeX = */ 1024,
	/* .MaxComputeWorkGroupSizeY = */ 1024,
	/* .MaxComputeWorkGroupSizeZ = */ 64,
	/* .MaxComputeUniformComponents = */ 1024,
	/* .MaxComputeTextureImageUnits = */ 16,
	/* .MaxComputeImageUniforms = */ 8,
	/* .MaxComputeAtomicCounters = */ 8,
	/* .MaxComputeAtomicCounterBuffers = */ 1,
	/* .MaxVaryingComponents = */ 60,
	/* .MaxVertexOutputComponents = */ 64,
	/* .MaxGeometryInputComponents = */ 64,
	/* .MaxGeometryOutputComponents = */ 128,
	/* .MaxFragmentInputComponents = */ 128,
	/* .MaxImageUnits = */ 8,
	/* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
	/* .MaxCombinedShaderOutputResources = */ 8,
	/* .MaxImageSamples = */ 0,
	/* .MaxVertexImageUniforms = */ 0,
	/* .MaxTessControlImageUniforms = */ 0,
	/* .MaxTessEvaluationImageUniforms = */ 0,
	/* .MaxGeometryImageUniforms = */ 0,
	/* .MaxFragmentImageUniforms = */ 8,
	/* .MaxCombinedImageUniforms = */ 8,
	/* .MaxGeometryTextureImageUnits = */ 16,
	/* .MaxGeometryOutputVertices = */ 256,
	/* .MaxGeometryTotalOutputComponents = */ 1024,
	/* .MaxGeometryUniformComponents = */ 1024,
	/* .MaxGeometryVaryingComponents = */ 64,
	/* .MaxTessControlInputComponents = */ 128,
	/* .MaxTessControlOutputComponents = */ 128,
	/* .MaxTessControlTextureImageUnits = */ 16,
	/* .MaxTessControlUniformComponents = */ 1024,
	/* .MaxTessControlTotalOutputComponents = */ 4096,
	/* .MaxTessEvaluationInputComponents = */ 128,
	/* .MaxTessEvaluationOutputComponents = */ 128,
	/* .MaxTessEvaluationTextureImageUnits = */ 16,
	/* .MaxTessEvaluationUniformComponents = */ 1024,
	/* .MaxTessPatchComponents = */ 120,
	/* .MaxPatchVertices = */ 32,
	/* .MaxTessGenLevel = */ 64,
	/* .MaxViewports = */ 16,
	/* .MaxVertexAtomicCounters = */ 0,
	/* .MaxTessControlAtomicCounters = */ 0,
	/* .MaxTessEvaluationAtomicCounters = */ 0,
	/* .MaxGeometryAtomicCounters = */ 0,
	/* .MaxFragmentAtomicCounters = */ 8,
	/* .MaxCombinedAtomicCounters = */ 8,
	/* .MaxAtomicCounterBindings = */ 1,
	/* .MaxVertexAtomicCounterBuffers = */ 0,
	/* .MaxTessControlAtomicCounterBuffers = */ 0,
	/* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
	/* .MaxGeometryAtomicCounterBuffers = */ 0,
	/* .MaxFragmentAtomicCounterBuffers = */ 1,
	/* .MaxCombinedAtomicCounterBuffers = */ 1,
	/* .MaxAtomicCounterBufferSize = */ 16384,
	/* .MaxTransformFeedbackBuffers = */ 4,
	/* .MaxTransformFeedbackInterleavedComponents = */ 64,
	/* .MaxCullDistances = */ 8,
	/* .MaxCombinedClipAndCullDistances = */ 8,
	/* .MaxSamples = */ 4,

	/* .maxMeshOutputVerticesNV = */ 256,
	/* .maxMeshOutputPrimitivesNV = */ 512,
	/* .maxMeshWorkGroupSizeX_NV = */ 32,
	/* .maxMeshWorkGroupSizeY_NV = */ 1,
	/* .maxMeshWorkGroupSizeZ_NV = */ 1,
	/* .maxTaskWorkGroupSizeX_NV = */ 32,
	/* .maxTaskWorkGroupSizeY_NV = */ 1,
	/* .maxTaskWorkGroupSizeZ_NV = */ 1,
	/* .maxMeshViewCountNV = */ 4,

	/* .limits = */ {
		/* .nonInductiveForLoops = */ 1,
		/* .whileLoops = */ 1,
		/* .doWhileLoops = */ 1,
		/* .generalUniformIndexing = */ 1,
		/* .generalAttributeMatrixVectorIndexing = */ 1,
		/* .generalVaryingIndexing = */ 1,
		/* .generalSamplerIndexing = */ 1,
		/* .generalVariableIndexing = */ 1,
		/* .generalConstantMatrixVectorIndexing = */ 1,
	}
};

namespace ed
{
	std::string TranscompileSource(int inLang, const std::string &filename, const std::string &inputSrc, int sType, const std::string &entry)
	{
		const char* inputStr = inputSrc.c_str();

		// create shader
		EShLanguage shaderType = EShLangVertex;
		if (sType == 1)
			shaderType = EShLangFragment;
		else if (sType == 2)
			shaderType = EShLangGeometry;
		else if (sType == 3)
			shaderType = EShLangCompute;

		glslang::TShader shader(shaderType);
		if (entry.size() > 0 && entry != "main") {
			shader.setEntryPoint(entry.c_str());
			shader.setSourceEntryPoint(entry.c_str());
		}
		shader.setStrings(&inputStr, 1);

		// set up
		int sVersion = (sType == 3) ? 430 : 330;
		glslang::EShTargetClientVersion vulkanVersion = glslang::EShTargetVulkan_1_0;
		glslang::EShTargetLanguageVersion targetVersion = glslang::EShTargetSpv_1_0;

		shader.setEnvInput(inLang == 1 ? glslang::EShSourceHlsl : glslang::EShSourceGlsl, shaderType, glslang::EShClientVulkan, sVersion);
		shader.setEnvClient(glslang::EShClientVulkan, vulkanVersion);
		shader.setEnvTarget(glslang::EShTargetSpv, targetVersion);
		
		TBuiltInResource res = DefaultTBuiltInResource;
		EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

		const int defVersion = sVersion;

		// includer
		DirStackFileIncluder includer;
		includer.pushExternalLocalDirectory(filename.substr(0, filename.find_last_of("/\\")));

		std::string processedShader;

		if (!shader.preprocess(&res, defVersion, ENoProfile, false, false, messages, &processedShader, includer))
		{
			return "error";
		}

		// update strings
		const char *processedStr = processedShader.c_str();
		shader.setStrings(&processedStr, 1);

		// parse
		if (!shader.parse(&res, 100, false, messages))
		{
			return "error";
		}

		// link
		glslang::TProgram prog;
		prog.addShader(&shader);

		if (!prog.link(messages))
		{
			return "error";
		}

		// convert to bvm
		std::vector<uint8_t> bvm;

		sd::BVMGenerator gen;
		gen.Translate(*prog.getIntermediate(shaderType), bvm);

		return "success";
	}
	std::string Transcompile(int inLang, const std::string &filename, int sType, const std::string &entry)
	{
		//Load HLSL into a string
		std::ifstream file(filename);

		if (!file.is_open())
		{
			file.close();
			return "errorFile";
		}

		std::string inputHLSL((std::istreambuf_iterator<char>(file)),
							std::istreambuf_iterator<char>());

		file.close();

		return TranscompileSource(inLang, filename, inputHLSL, sType, entry);
	}
}

#include <unistd.h>

int main() {
	// start glslang process
	bool glslangInit = glslang::InitializeProcess();
	if (!glslangInit) {
		printf("Tst!\n");
	}
	
	sleep(1);

	std::string stat = ed::Transcompile(1, "bin/shader.hlsl", 1, "main");
	printf("%s\n", stat.c_str());
    return 0;
}