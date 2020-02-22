#include <ShaderDebugger/HLSLLibrary.h>
#include <ShaderDebugger/CommonLibrary.h>
#include <ShaderDebugger/ShaderDebugger.h>
#include <ShaderDebugger/TextureCube.h>
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
				- double type, asdouble()
				- printf, errorf, msad4
		*/

		/* swizzle */
		bv_variable Swizzle(bv_program* prog, bv_object* obj, const bv_string field)
		{
			if (obj == nullptr)
				return bv_variable_create_void();
			
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
		bv_variable lib_hlsl_saturate(bv_program* prog, u8 count, bv_variable* args)
		{
			/* saturate(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // ceil(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::clamp(sd::AsVector<4, float>(args[0]), glm::vec4(0.0f), glm::vec4(1.0f));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // saturate(scalar)
					return bv_variable_create_float(glm::clamp(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0])), 0.0f, 1.0f));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_hlsl_sincos(bv_program* prog, u8 count, bv_variable* args)
		{
			/* sincos */
			if (count >= 3) {
				if (args[0].type == bv_type_object) {
					bv_object* x = bv_variable_get_object(args[0]);
					glm::vec4 xData = sd::AsVector<4, float>(args[0]);

					glm::vec4 sinRes = glm::sin(xData);
					glm::vec4 cosRes = glm::cos(xData);

					bv_variable* outSin = bv_variable_get_pointer(args[1]);
					bv_object* outSinObj = bv_variable_get_object(*outSin);
					for (u16 i = 0; i < outSinObj->type->props.name_count; i++)
						outSinObj->prop[i] = bv_variable_create_float(sinRes[i]);

					bv_variable* outCos = bv_variable_get_pointer(args[2]);
					bv_object* outCosObj = bv_variable_get_object(*outCos);
					for (u16 i = 0; i < outCosObj->type->props.name_count; i++)
						outCosObj->prop[i] = bv_variable_create_float(cosRes[i]);
				}
				else {
					float x = bv_variable_get_float(args[0]);
					float sinRes = glm::sin(x), cosRes = glm::cos(x);

					bv_variable* outSin = bv_variable_get_pointer(args[1]);
					bv_variable_set_float(outSin, sinRes);

					bv_variable* outCos = bv_variable_get_pointer(args[2]);
					bv_variable_set_float(outCos, cosRes);
				}
			}

			return bv_variable_create_void();
		}
		bv_variable lib_hlsl_rcp(bv_program* prog, u8 count, bv_variable* args)
		{
			/* rcp(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // ceil(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::vec4(1.0f)/sd::AsVector<4, float>(args[0]);

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // rcp(scalar)
					return bv_variable_create_float(1.0f / bv_variable_get_float(bv_variable_cast(bv_type_float, args[0])));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_hlsl_log10(bv_program* prog, u8 count, bv_variable* args)
		{
			/* rcp(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // ceil(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = sd::AsVector<4, float>(args[0]);

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++) {
						vecData[i] = log10f(vecData[i]);
						retObj->prop[i] = bv_variable_create_float(vecData[i]);
					}

					return ret;
				}
				else // rcp(scalar)
					return bv_variable_create_float(log10f(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_hlsl_isfinite(bv_program* prog, u8 count, bv_variable* args)
		{
			/* isfinite(genType), isfinite(genDType)  */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					glm::vec4 val = sd::AsVector<4, float>(args[0]);
					glm::bvec4 vecData = glm::not_(glm::isnan(val)) && glm::not_(glm::isinf(val));

					bv_variable ret = Common::create_vec(prog, bv_type_uchar, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_uchar(vecData[i]);

					return ret;

				}
				// isfinite(float)
				else {
					float scalarData = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
					return bv_variable_create_uchar(!glm::isnan(scalarData) && !glm::isinf(scalarData));
				}
			}

			return bv_variable_create_uchar(0);
		}
		bv_variable lib_hlsl_fmod(bv_program* prog, u8 count, bv_variable* args)
		{
			/* mod(genType, genType), mod(genType, float), also for genDType */
			if (count == 2) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					// using: genType, float, genDType, double
					glm::vec4 x = sd::AsVector<4, float>(args[0]);
					glm::vec4 y(0.0f);

					// get y
					if (args[1].type == bv_type_object)
						y = sd::AsVector<4, float>(args[1]);
					else
						y = glm::vec4(bv_variable_get_float(bv_variable_cast(bv_type_float, args[1])));

					glm::vec4 vecData = x - y * glm::trunc(x / y);

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}

				// mod(float, float)
				else if (args[0].type == bv_type_float) {
					float x = bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]));
					float y = bv_variable_get_float(bv_variable_cast(bv_type_float, args[1]));

					float retVal = x - y * glm::trunc(x / y);

					return bv_variable_create_float(retVal);
				}
			}

			return bv_variable_create_float(0.0f);
		}

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

			return bv_variable_create_void();
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
		bv_variable lib_hlsl_f32tof16(bv_program* prog, u8 count, bv_variable* args)
		{
			/* f32tof16(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					bv_variable ret = Common::create_vec(prog, bv_type_uint, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					glm::vec4 val = sd::AsVector<4, float>(args[0]);

					glm::uvec4 vecData(0);
					for (u16 i = 0; i < 4; i++)
						vecData[i] = glm::packHalf2x16(glm::vec2(val[i], 0.0f));

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_uint(vecData[i]);

					return ret;
				}
				// f32tof16(scalar)
				else
					return bv_variable_create_uint(glm::packHalf2x16(glm::vec2(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0])), 0.0f)));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_hlsl_f16tof32(bv_program* prog, u8 count, bv_variable* args)
		{
			/* f32tof16(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					glm::uvec4 val = sd::AsVector<4, unsigned int>(args[0]);

					glm::vec4 vecData(0);
					for (u16 i = 0; i < 4; i++)
						vecData[i] = glm::unpackHalf2x16(val[i]).x;

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				// f32tof16(scalar)
				else
					return bv_variable_create_float(glm::unpackHalf2x16(bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[0]))).x);
			}

			return bv_variable_create_float(0.0f);
		}

		/* vector */
		bv_variable lib_hlsl_dst(bv_program* prog, u8 count, bv_variable* args)
		{
			/* dst(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // ceil(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);

					glm::vec4 src1 = sd::AsVector<4, float>(args[0]);
					glm::vec4 src2 = sd::AsVector<4, float>(args[1]);

					glm::vec4 vecData(1.0f, src1.y * src2.y, src1.z, src2.w); // https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dst---vs

					bv_variable ret = Common::create_vec(prog, bv_type_float, 4);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
			}

			return Common::create_vec(prog, bv_type_float, 4);
		}
		bv_variable lib_hlsl_lit(bv_program* prog, u8 count, bv_variable* args)
		{
			float ambient = 1, diffuse = 0, specular = 0;

			/* float4 lit(float, float, float) */
			if (count >= 3) {
				float n_dot_l = bv_variable_get_float(args[0]);
				float n_dot_h = bv_variable_get_float(args[1]);
				float m = bv_variable_get_float(args[2]);
				
				ambient = 1;
				diffuse = (n_dot_l < 0) ? 0 : n_dot_l;
				specular = (n_dot_l < 0 || n_dot_h < 0) ? 0 : glm::pow(n_dot_h, m);
			}

			bv_variable ret = Common::create_vec(prog, bv_type_float, 4);
			bv_object* vec = bv_variable_get_object(ret);
			vec->prop[0] = bv_variable_create_float(ambient);
			vec->prop[1] = bv_variable_create_float(diffuse);
			vec->prop[2] = bv_variable_create_float(specular);
			vec->prop[3] = bv_variable_create_float(1.0f);

			return ret;
		}

		/* Texture1/2/3D/Cube */
		bv_variable lib_hlsl_Texture_Sample(bv_program* prog, bv_object* me, u8 argc, bv_variable* args)
		{
			/* TODO: clamp LOD, etc... */
			if (argc >= 2) {
				bv_object* samplerState = bv_variable_get_object(args[0]);

				// Texture1D/2D/3D
				if (IsBasicTexture(me->type->name)) {
					Texture* tex = (Texture*)me->user_data;
					glm::vec3 uv = sd::AsVector<3, float>(args[1]);


					glm::ivec3 offset(0);
					if (argc >= 3)
						offset = sd::AsVector<3, int>(args[2]);

					float lod = 0.0f; // TODO: LOD calculation

					// TODO: or maybe have an actual argument in sd::Texture::Sample method for offset...
					float xPixel = 1.0f / tex->Width;
					float yPixel = 1.0f / tex->Height;
					float zPixel = 1.0f / tex->Depth;

					uv.x += offset.x * xPixel;
					if (tex->Height > 1)
						uv.y += offset.y * yPixel;
					if (tex->Depth > 1)
						uv.z += offset.z * zPixel;

					glm::vec4 sample = tex->Sample(uv.x, uv.y, uv.z, lod);
					
					return Common::create_float4(prog, sample);
				}
				// TextureCube
				else if (IsCubemap(me->type->name)) {
					TextureCube* tex = (TextureCube*)me->user_data;
					glm::vec3 normal = sd::AsVector<3, float>(args[1]);
					
					glm::vec4 sample = tex->Sample(normal);

					return Common::create_float4(prog, sample);
				}
			}

			return Common::create_float4(prog);
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
			bv_library_add_function(lib, "sincos", lib_hlsl_sincos);
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
			bv_library_add_function(lib, "fmod", lib_hlsl_fmod);
			bv_library_add_function(lib, "frac", Common::lib_common_fract);
			bv_library_add_function(lib, "fwidth", Common::lib_common_fwidth);
			bv_library_add_function(lib, "rcp", lib_hlsl_rcp);
			bv_library_add_function(lib, "rsqrt", Common::lib_common_inversesqrt);
			bv_library_add_function(lib, "pow", Common::lib_common_pow);
			bv_library_add_function(lib, "isinf", Common::lib_common_isinf);
			bv_library_add_function(lib, "isnan", Common::lib_common_isnan);
			bv_library_add_function(lib, "isfinite", lib_hlsl_isfinite);
			bv_library_add_function(lib, "log", Common::lib_common_log);
			bv_library_add_function(lib, "log2", Common::lib_common_log2);
			bv_library_add_function(lib, "log10", lib_hlsl_log10);
			bv_library_add_function(lib, "mad", Common::lib_common_fma);
			bv_library_add_function(lib, "max", Common::lib_common_max);
			bv_library_add_function(lib, "min", Common::lib_common_min);
			bv_library_add_function(lib, "lerp", Common::lib_common_mix);
			bv_library_add_function(lib, "modf", Common::lib_common_modf);
			bv_library_add_function(lib, "round", Common::lib_common_round);
			bv_library_add_function(lib, "saturate", lib_hlsl_saturate);
			bv_library_add_function(lib, "sign", Common::lib_common_sign);
			bv_library_add_function(lib, "smoothstep", Common::lib_common_smoothstep);
			bv_library_add_function(lib, "sqrt", Common::lib_common_sqrt);
			bv_library_add_function(lib, "step", Common::lib_common_step);
			bv_library_add_function(lib, "trunc", Common::lib_common_trunc);

			// vector
			bv_library_add_function(lib, "cross", Common::lib_common_cross);
			bv_library_add_function(lib, "distance", Common::lib_common_distance);
			bv_library_add_function(lib, "dot", Common::lib_common_dot);
			bv_library_add_function(lib, "dst", lib_hlsl_dst);
			bv_library_add_function(lib, "lit", lib_hlsl_lit);
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
			bv_library_add_function(lib, "reversebits", Common::lib_common_bitfieldReverse);
			bv_library_add_function(lib, "countbits", Common::lib_common_bitCount);
			bv_library_add_function(lib, "firstbitlow", Common::lib_common_findLSB);
			bv_library_add_function(lib, "firstbithigh", Common::lib_common_findMSB);

			// float
			bv_library_add_function(lib, "asint", lib_hlsl_asint);
			bv_library_add_function(lib, "asuint", lib_hlsl_asuint);
			bv_library_add_function(lib, "asfloat", lib_hlsl_asfloat);
			bv_library_add_function(lib, "f32tof16", lib_hlsl_f32tof16);
			bv_library_add_function(lib, "f16tof32", lib_hlsl_f16tof32);
			bv_library_add_function(lib, "frexp", Common::lib_common_frexp);
			bv_library_add_function(lib, "ldexp", Common::lib_common_ldexp);
			
			// misc
			bv_library_add_function(lib, "abort", lib_hlsl_abort);
			bv_library_add_function(lib, "clip", lib_hlsl_clip);

			// Texture1D
			bv_object_info* texture1D = bv_object_info_create("Texture1D");
			bv_object_info_add_ext_method(texture1D, "Sample", lib_hlsl_Texture_Sample);
			bv_library_add_object_info(lib, texture1D);

			// Texture2D
			bv_object_info* texture2D = bv_object_info_create("Texture2D");
			bv_object_info_add_ext_method(texture2D, "Sample", lib_hlsl_Texture_Sample);
			bv_library_add_object_info(lib, texture2D);

			// Texture3D
			bv_object_info* texture3D = bv_object_info_create("Texture3D");
			bv_object_info_add_ext_method(texture3D, "Sample", lib_hlsl_Texture_Sample);
			bv_library_add_object_info(lib, texture3D);

			// TextureCube
			bv_object_info* textureCube = bv_object_info_create("TextureCube");
			bv_object_info_add_ext_method(textureCube, "Sample", lib_hlsl_Texture_Sample);
			bv_library_add_object_info(lib, textureCube);

			// SamplerState
			bv_object_info* samplerState = bv_object_info_create("SamplerState");
			bv_library_add_object_info(lib, samplerState);

			// sampler
			bv_object_info* sampler = bv_object_info_create("sampler");
			bv_library_add_object_info(lib, sampler);

			return lib;
		}
	}
}