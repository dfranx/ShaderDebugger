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
	"mat4x4"
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
	"dmat4x4"
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
			4,4,4
		};
		static const std::vector<int> row_list = {
			2,3,4,
			2,3,4,
			2,3,4,
			2,3,4
		};
		*cols = 0;
		*rows = 0;
		for (int i = 0; i < floatMats.size(); i++)
			if (strcmp(floatMats[i], name) == 0 || strcmp(doubleMats[i], name) == 0) {
				*cols = col_list[i];
				*rows = row_list[i];
				break;
			}
	}
	bv_type GetMatrixTypeFromName(const char* name)
	{
		for (int i = 0; i < floatMats.size(); i++)
			if (strcmp(floatMats[i], name) == 0 || strcmp(doubleMats[i], name) == 0)
				return bv_type_float;
		
		return bv_type_void;
	}
}