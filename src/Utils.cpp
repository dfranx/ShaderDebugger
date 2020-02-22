#include <ShaderDebugger/Utils.h>
#include <string.h>
#include <vector>

static const std::vector<const char*> floatMats = {
	"mat2",
	"mat3",
	"mat4",
	"mat2x2",
	"mat2x3",
	"mat2x4",
	"mat3x2",
	"mat3x3",
	"mat3x4",
	"mat4x2",
	"mat4x3",
	"mat4x4",
	"float2x2",
	"float3x2",
	"float4x2",
	"float2x3",
	"float3x3",
	"float4x3",
	"float2x4",
	"float3x4",
	"float4x4"
};
static const std::vector<const char*> doubleMats = {
	"dmat2",
	"dmat3",
	"dmat4",
	"dmat2x2",
	"dmat2x3",
	"dmat2x4",
	"dmat3x2",
	"dmat3x3",
	"dmat3x4",
	"dmat4x2",
	"dmat4x3",
	"dmat4x4",
};

namespace sd
{
	sd::Matrix AsMatrix(const bv_variable& var)
	{
		if (var.type == bv_type_object) {
			bv_object* obj = bv_variable_get_object(var);
			return *(sd::Matrix*)obj->user_data;
		}

		return sd::Matrix();
	}
	float AsFloat(const bv_variable& var)
	{
		if (var.type == bv_type_float)
			return bv_variable_get_float(var);
		return 0.0f;
	}
	int AsInteger(const bv_variable& var)
	{
		if (bv_type_is_integer(var.type))
			return bv_variable_get_int(var);
		return 0;
	}
	unsigned int AsUnsignedInteger(const bv_variable& var)
	{
		if (bv_type_is_integer(var.type))
			return bv_variable_get_uint(var);
		return 0u;
	}
	unsigned int AsBool(const bv_variable& var)
	{
		if (bv_type_is_integer(var.type))
			return (bool)bv_variable_get_uchar(var);
		return false;
	}

	bv_type GetVectorTypeFromName(const char* name)
	{
		static const char* iNames[] = { "ivec4", "ivec3", "ivec2", "int4", "int3", "int2" };
		static const char* uNames[] = { "uvec4", "uvec3", "uvec2", "uint4", "uint3", "uint2" };
		static const char* bNames[] = { "bvec4", "bvec3", "bvec2", "bool4", "bool3", "bool2" };

		for (u8 i = 0; i < 6; i++) {
			if (strcmp(name, iNames[i]) == 0) return bv_type_int;
			if (strcmp(name, uNames[i]) == 0) return bv_type_uint;
			if (strcmp(name, bNames[i]) == 0) return bv_type_uchar;
		}

		return bv_type_float;
	}
	void GetMatrixSizeFromName(const char* name, int* cols, int* rows)
	{
		static const std::vector<int> col_list = {
			2,3,4,
			2,2,2,
			3,3,3,
			4,4,4,
			2,2,2,
			3,3,3,
			4,4,4
		};
		static const std::vector<int> row_list = {
			2,3,4,
			2,3,4,
			2,3,4,
			2,3,4,
			2,3,4,
			2,3,4,
			2,3,4
		};
		*cols = 0;
		*rows = 0;
		for (int i = 0; i < floatMats.size(); i++)
			if (strcmp(floatMats[i], name) == 0 || (i < doubleMats.size() && strcmp(doubleMats[i], name) == 0)) {
				*cols = col_list[i];
				*rows = row_list[i];
				break;
			}
	}
	bv_type GetMatrixTypeFromName(const char* name)
	{
		for (int i = 0; i < floatMats.size(); i++)
			if (strcmp(floatMats[i], name) == 0 || (i < doubleMats.size() && strcmp(doubleMats[i], name) == 0))
				return bv_type_float;
		
		return bv_type_void;
	}
	bv_type GetVariableTypeFromName(const char* name)
	{
		static const char* tNames[] = { "int", "float", "bool", "double", "uint", "byte", "char", "short", "half" };
		static bv_type tTypes[] =     { bv_type_int, bv_type_float, bv_type_uchar, bv_type_float, bv_type_uint, bv_type_uchar, bv_type_char, bv_type_short, bv_type_float };

		for (u8 i = 0; i < 9; i++)
			if (strcmp(name, tNames[i]) == 0)
				return tTypes[i];

		return bv_type_void;
	}

	bool IsBasicTexture(const char* name)
	{
		static const std::vector<const char*> names = {
			"Texture1D", "Texture2D", "Texture3D",
			"isampler1D", "usampler1D", "sampler1D",
			"isampler2D", "usampler2D", "sampler2D",
			"isampler3D", "usampler3D", "sampler3D"
		};
		for (u16 i = 0; i < names.size(); i++)
			if (strcmp(names[i], name) == 0)
				return 1;
		return 0;
	}
	bool IsCubemap(const char* name)
	{
		static const std::vector<const char*> names = {
			"TextureCube", "samplerCube"
		};
		for (u16 i = 0; i < names.size(); i++)
			if (strcmp(names[i], name) == 0)
				return 1;
		return 0;
	}
	u8 GetTextureDimension(const char* name)
	{
		static const std::vector<const char*> names = {
			"Texture1D", "Texture2D", "Texture3D",
			"sampler1D", "sampler2D", "sampler3D",
			"isampler1D", "isampler2D", "isampler3D",
			"usampler1D", "usampler2D", "usampler3D"
		};
		for (u16 i = 0; i < names.size(); i++)
			if (strcmp(names[i], name) == 0)
				return (i % 3) + 1;
		return 0;
	}
	
	sd::Matrix* CopyMatrixData(sd::Matrix* mat)
	{
		sd::Matrix* ret = new sd::Matrix();
		ret->Columns = mat->Columns;
		ret->Rows = mat->Rows;
		ret->Type = mat->Type;
		ret->Data = mat->Data;
		return ret;
	}
}