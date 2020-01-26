#include <ShaderDebugger/HLSLLibrary.h>
#include <ShaderDebugger/CommonLibrary.h>
#include <ShaderDebugger/ShaderDebugger.h>
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
		/*
			TODO:
				- ddx, ddy, ddx_coarse, ddy_coarse, ddx_fine, ddy_fine, fwidth
				- double type
				- printf, errorf, asdouble
		*/

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

		/* misc */
		bv_variable lib_hlsl_abort(bv_program* prog, u8 count, bv_variable* args)
		{
			((sd::ShaderDebugger*)prog->user_data)->SetDiscarded(true);
			return bv_variable_create_void();
		}
		bv_variable lib_hlsl_clip(bv_program* prog, u8 count, bv_variable* args)
		{
			bool shouldDiscard = false;

			if (count == 1) {
				if (args[0].type == bv_type_object) { // acosh(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = sd::AsVector<4, float>(args[0]); // acosh takes only float vectors

					for (u16 i = 0; i < vec->type->props.name_count; i++)
						shouldDiscard |= (vecData[i] < 0.0f);
				}
				else // acosh(scalar)
					shouldDiscard = (bv_variable_get_float(bv_variable_cast(bv_type_float, args[0])) < 0.0f);
			}

			if (shouldDiscard)
				((sd::ShaderDebugger*)prog->user_data)->SetDiscarded(true);
			
			return bv_variable_create_void();
		}

		/* math */

		/* floating points */
		bv_variable lib_hlsl_asint(bv_program* prog, u8 count, bv_variable* args)
		{
			/* asint(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					bv_variable ret = Common::create_vec(prog, bv_type_int, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					glm::ivec4 vecData(0);
					if (vec->prop->type == bv_type_float)
						vecData = glm::floatBitsToInt(sd::AsVector<4, float>(args[0]));
					else vecData = sd::AsVector<4, int>(args[0]);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_int(vecData[i]);

					return ret;
				}
				// asint(scalar)
				else if (args[0].type == bv_type_float)
					return bv_variable_create_int(glm::floatBitsToInt(bv_variable_get_float(args[0])));
				else
					return bv_variable_cast(bv_type_int, args[0]);
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_hlsl_asuint(bv_program* prog, u8 count, bv_variable* args)
		{
			/* asuint(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					bv_variable ret = Common::create_vec(prog, bv_type_uint, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					glm::uvec4 vecData(0);
					if (vec->prop->type == bv_type_float)
						vecData = glm::floatBitsToUint(sd::AsVector<4, float>(args[0]));
					else vecData = sd::AsVector<4, unsigned int>(args[0]);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_uint(vecData[i]);

					return ret;
				}
				// asuint(scalar)
				else if (args[0].type == bv_type_float)
					return bv_variable_create_uint(glm::floatBitsToUint(bv_variable_get_float(args[0])));
				else
					return bv_variable_cast(bv_type_uint, args[0]);
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_hlsl_asfloat(bv_program* prog, u8 count, bv_variable* args)
		{
			/* asfloat(genIType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::intBitsToFloat(sd::AsVector<4, int>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // asfloat(scalar)
					return bv_variable_create_float(glm::intBitsToFloat(bv_variable_get_int(bv_variable_cast(bv_type_int, args[0]))));
			}

			return bv_variable_create_float(0.0f);
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
			bv_library_add_function(lib, "cosh", Common::lib_common_cosh);
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

			// integer
			bv_library_add_function(lib, "countbits", Common::lib_common_bitCount);
			bv_library_add_function(lib, "firstbitlow", Common::lib_common_findLSB);
			bv_library_add_function(lib, "firstbithigh", Common::lib_common_findMSB);

			// float
			bv_library_add_function(lib, "asint", lib_hlsl_asint);
			bv_library_add_function(lib, "asuint", lib_hlsl_asuint);
			bv_library_add_function(lib, "asfloat", lib_hlsl_asfloat);

			// misc
			bv_library_add_function(lib, "abort", lib_hlsl_abort);
			bv_library_add_function(lib, "clip", lib_hlsl_clip);

			// Texture2D
			bv_object_info* texture2D = bv_object_info_create("Texture2D");
			// TODO: methods
			bv_library_add_object_info(lib, texture2D);

			return lib;
		}
	}
}