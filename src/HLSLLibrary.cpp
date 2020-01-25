#include <ShaderDebugger/HLSLLibrary.h>
#include <ShaderDebugger/CommonLibrary.h>
#include <ShaderDebugger/Texture.h>
#include <ShaderDebugger/Matrix.h>
#include <ShaderDebugger/Utils.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>

namespace sd
{
	namespace HLSL
	{
		/* swizzle */
		bv_variable Swizzle(bv_program* prog, bv_object* obj, const bv_string field)
		{
			const char* rgba = "rgba\0";
			const char* xyzw = "xyzw\0";

			char name[4][2] = { 0 };
			for (int j = 0; j < strlen(field); j++)
				for (int i = 0; i < 4; i++) {
					if (field[j] == rgba[i])
						name[j][0] = xyzw[i];
					else if (field[j] == xyzw[i])
						name[j][0] = xyzw[i];
				}

			if (strlen(field) == 1)
				return bv_variable_create_pointer(bv_object_get_property(obj, name[0]));
			else if (strlen(field) == 2) {
				bv_variable ret = Common::create_vec(prog, obj->prop[0].type, 2);
				bv_object* vec = bv_variable_get_object(ret);
				bv_object_set_property(vec, "x", bv_variable_create_pointer(bv_object_get_property(obj, name[0])));
				bv_object_set_property(vec, "y", bv_variable_create_pointer(bv_object_get_property(obj, name[1])));
				return ret;
			}
			else if (strlen(field) == 3) {
				bv_variable ret = Common::create_vec(prog, obj->prop[0].type, 3);
				bv_object* vec = bv_variable_get_object(ret);
				bv_object_set_property(vec, "x", bv_variable_create_pointer(bv_object_get_property(obj, name[0])));
				bv_object_set_property(vec, "y", bv_variable_create_pointer(bv_object_get_property(obj, name[1])));
				bv_object_set_property(vec, "z", bv_variable_create_pointer(bv_object_get_property(obj, name[2])));
				return ret;
			}
			else if (strlen(field) == 4) {
				bv_variable ret = Common::create_vec(prog, obj->prop[0].type, 4);
				bv_object* vec = bv_variable_get_object(ret);
				bv_object_set_property(vec, "x", bv_variable_create_pointer(bv_object_get_property(obj, name[0])));
				bv_object_set_property(vec, "y", bv_variable_create_pointer(bv_object_get_property(obj, name[1])));
				bv_object_set_property(vec, "z", bv_variable_create_pointer(bv_object_get_property(obj, name[2])));
				bv_object_set_property(vec, "w", bv_variable_create_pointer(bv_object_get_property(obj, name[3])));
				return ret;
			}
		}

		bv_library* Library()
		{
			bv_library* lib = bv_library_create();

			// vector types
			Common::lib_add_vec(lib, "float4", 4);
			Common::lib_add_vec(lib, "int4", 4);
			Common::lib_add_vec(lib, "bool4", 4, 1);
			Common::lib_add_vec(lib, "half4", 4);
			Common::lib_add_vec(lib, "uint4", 4);
			Common::lib_add_vec(lib, "float3", 3);
			Common::lib_add_vec(lib, "int3", 3);
			Common::lib_add_vec(lib, "bool3", 3, 1);
			Common::lib_add_vec(lib, "half3", 3);
			Common::lib_add_vec(lib, "uint3", 3);
			Common::lib_add_vec(lib, "float2", 2);
			Common::lib_add_vec(lib, "int2", 2);
			Common::lib_add_vec(lib, "bool2", 2, 1);
			Common::lib_add_vec(lib, "half2", 2);
			Common::lib_add_vec(lib, "uint2", 2);

			// matrix types
			Common::lib_add_mat(lib, "float2x2");
			Common::lib_add_mat(lib, "float3x2");
			Common::lib_add_mat(lib, "float4x2");
			Common::lib_add_mat(lib, "float2x3");
			Common::lib_add_mat(lib, "float3x3");
			Common::lib_add_mat(lib, "float4x3");
			Common::lib_add_mat(lib, "float2x4");
			Common::lib_add_mat(lib, "float3x4");
			Common::lib_add_mat(lib, "float4x4");

			// trigonometry
			bv_library_add_function(lib, "acos", Common::lib_common_acos);
			bv_library_add_function(lib, "asin", Common::lib_common_asin);
			bv_library_add_function(lib, "atan", Common::lib_common_atan);
			bv_library_add_function(lib, "atan2", Common::lib_common_atan);
			bv_library_add_function(lib, "cos", Common::lib_common_cos);
			bv_library_add_function(lib, "acosh", Common::lib_common_cosh);
			bv_library_add_function(lib, "sin", Common::lib_common_sin);
			bv_library_add_function(lib, "sinh", Common::lib_common_sinh);
			bv_library_add_function(lib, "tan", Common::lib_common_tan);
			bv_library_add_function(lib, "tanh", Common::lib_common_tanh);
			bv_library_add_function(lib, "degrees", Common::lib_common_degrees);
			bv_library_add_function(lib, "radians", Common::lib_common_radians);

			// mathematics
			bv_library_add_function(lib, "abs", Common::lib_common_abs);
			bv_library_add_function(lib, "ceil", Common::lib_common_ceil);
			bv_library_add_function(lib, "clamp", Common::lib_common_clamp);
			bv_library_add_function(lib, "ddx", Common::lib_common_dFdx);
			bv_library_add_function(lib, "ddy", Common::lib_common_dFdy);
			bv_library_add_function(lib, "ddx_coarse", Common::lib_common_dFdxCoarse);
			bv_library_add_function(lib, "ddy_coarse", Common::lib_common_dFdyCoarse);
			bv_library_add_function(lib, "ddx_fine", Common::lib_common_dFdxFine);
			bv_library_add_function(lib, "ddy_fine", Common::lib_common_dFdyFine);
			bv_library_add_function(lib, "exp", Common::lib_common_exp);
			bv_library_add_function(lib, "exp2", Common::lib_common_exp2);
			bv_library_add_function(lib, "floor", Common::lib_common_floor);
			bv_library_add_function(lib, "fma", Common::lib_common_fma);
			bv_library_add_function(lib, "frac", Common::lib_common_fract);
			bv_library_add_function(lib, "fwidth", Common::lib_common_fwidth);
			bv_library_add_function(lib, "rsqrt", Common::lib_common_inversesqrt);
			bv_library_add_function(lib, "isinf", Common::lib_common_isinf);
			bv_library_add_function(lib, "isnan", Common::lib_common_isnan);
			bv_library_add_function(lib, "log", Common::lib_common_log);
			bv_library_add_function(lib, "log2", Common::lib_common_log2);
			bv_library_add_function(lib, "max", Common::lib_common_max);
			bv_library_add_function(lib, "min", Common::lib_common_min);
			bv_library_add_function(lib, "modf", Common::lib_common_modf);
			bv_library_add_function(lib, "round", Common::lib_common_round);
			bv_library_add_function(lib, "sign", Common::lib_common_sign);
			bv_library_add_function(lib, "smoothstep", Common::lib_common_smoothstep);
			bv_library_add_function(lib, "sqrt", Common::lib_common_sqrt);
			bv_library_add_function(lib, "step", Common::lib_common_step);
			bv_library_add_function(lib, "trunc", Common::lib_common_trunc);

			// vector
			bv_library_add_function(lib, "cross", Common::lib_common_cross);
			bv_library_add_function(lib, "distance", Common::lib_common_distance);
			bv_library_add_function(lib, "dot", Common::lib_common_dot);
			bv_library_add_function(lib, "faceforward", Common::lib_common_faceforward);
			bv_library_add_function(lib, "length", Common::lib_common_length);
			bv_library_add_function(lib, "normalize", Common::lib_common_normalize);
			bv_library_add_function(lib, "reflect", Common::lib_common_reflect);
			bv_library_add_function(lib, "refract", Common::lib_common_refract);

			// component comparison
			bv_library_add_function(lib, "all", Common::lib_common_all);
			bv_library_add_function(lib, "any", Common::lib_common_any);

			// matrix
			bv_library_add_function(lib, "determinant", Common::lib_common_determinant);
			bv_library_add_function(lib, "transpose", Common::lib_common_transpose);

			// Texture2D
			bv_object_info* texture2D = bv_object_info_create("Texture2D");
			// TODO: methods
			bv_library_add_object_info(lib, texture2D);

			return lib;
		}
	}
}