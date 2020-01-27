#include <ShaderDebugger/GLSLLibrary.h>
#include <ShaderDebugger/CommonLibrary.h>
#include <ShaderDebugger/Texture.h>
#include <ShaderDebugger/Matrix.h>
#include <ShaderDebugger/Utils.h>
#include <FastNoise/FastNoise.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>

namespace sd
{
	namespace GLSL
	{
		/*
			TODO: 
				- dFdx, dFdy, dFdxCoarse, dFdyCoarse, dFdxFine, dFdyFine
				- fwidth, fwidthCoarse, fwidthFine
				- interpolateAtCentroid, interpolateAtOffset, interpolateAtSample, textureGatherOffsets, textureGrad, textureGradOffset, textureProjGrad, textureProjGradOffset, textureQueryLod, textureSamples
				- double type & (un)packDouble2x32
				- LOD calculation, or have a ShaderDebugger::LOD which the user can set?
				- reimplement noise functions so that they follow the "rules"
				- geometry and compute (image, atomic, memory barriers) shader functions
		*/

		bv_type get_texture_type(const char* name)
		{
			static const std::vector<const char*> namesU = {
				"usampler1D", "usampler2D", "usampler3D"
			};
			static const std::vector<const char*> namesI = {
				"isampler1D", "isampler2D", "isampler3D"
			};
			for (u16 i = 0; i < namesU.size(); i++)
				if (strcmp(namesU[i], name) == 0)
					return bv_type_uint;
			for (u16 i = 0; i < namesI.size(); i++)
				if (strcmp(namesI[i], name) == 0)
					return bv_type_int;
			return bv_type_float;
		}

		/* swizzle */
		bv_variable Swizzle(bv_program* prog, bv_object* obj, const bv_string field)
		{
			const char* rgba = "rgba\0";
			const char* xyzw = "xyzw\0";
			const char* stpq = "stpq\0";

			char name[4][2] = { 0 };
			for (int j = 0; j < strlen(field); j++)
				for (int i = 0; i < 4; i++) {
					if (field[j] == rgba[i])
						name[j][0] = xyzw[i];
					else if (field[j] == stpq[i])
						name[j][0] = xyzw[i];
					else if (field[j] == xyzw[i])
						name[j][0] = xyzw[i];
				}

			// printf("[DEBUG] swizzle: %s\n", field);

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

		/* noise */
		static FastNoise lib_glsl_noiseGenerator; // I know... this takes least time, i'll have to reimplement noise functions anyway
		float noise1(glm::vec4 pos)
		{
			lib_glsl_noiseGenerator.SetNoiseType(FastNoise::NoiseType::WhiteNoise);
			lib_glsl_noiseGenerator.SetSeed(40000);
			return lib_glsl_noiseGenerator.GetWhiteNoise(pos.x, pos.y, pos.z, pos.w);
		}
		glm::vec2 noise2(glm::vec4 pos)
		{
			glm::vec2 ret(0.0f);

			lib_glsl_noiseGenerator.SetNoiseType(FastNoise::NoiseType::WhiteNoise);
			lib_glsl_noiseGenerator.SetSeed(40000);
			ret.x = lib_glsl_noiseGenerator.GetWhiteNoise(pos.x, pos.y, pos.z, pos.w);
			lib_glsl_noiseGenerator.SetSeed(41000);
			ret.y = lib_glsl_noiseGenerator.GetWhiteNoise(pos.x, pos.y, pos.z, pos.w);

			return ret;
		}
		glm::vec3 noise3(glm::vec4 pos)
		{
			glm::vec3 ret(0.0f);

			lib_glsl_noiseGenerator.SetNoiseType(FastNoise::NoiseType::WhiteNoise);
			lib_glsl_noiseGenerator.SetSeed(40000);
			ret.x = lib_glsl_noiseGenerator.GetWhiteNoise(pos.x, pos.y, pos.z, pos.w);
			lib_glsl_noiseGenerator.SetSeed(41000);
			ret.y = lib_glsl_noiseGenerator.GetWhiteNoise(pos.x, pos.y, pos.z, pos.w);
			lib_glsl_noiseGenerator.SetSeed(42000);
			ret.z = lib_glsl_noiseGenerator.GetWhiteNoise(pos.x, pos.y, pos.z, pos.w);

			return ret;
		}
		glm::vec4 noise4(glm::vec4 pos)
		{
			glm::vec4 ret(0.0f);

			lib_glsl_noiseGenerator.SetNoiseType(FastNoise::NoiseType::WhiteNoise);
			lib_glsl_noiseGenerator.SetSeed(40000);
			ret.x = lib_glsl_noiseGenerator.GetWhiteNoise(pos.x, pos.y, pos.z, pos.w);
			lib_glsl_noiseGenerator.SetSeed(41000);
			ret.y = lib_glsl_noiseGenerator.GetWhiteNoise(pos.x, pos.y, pos.z, pos.w);
			lib_glsl_noiseGenerator.SetSeed(42000);
			ret.z = lib_glsl_noiseGenerator.GetWhiteNoise(pos.x, pos.y, pos.z, pos.w);
			lib_glsl_noiseGenerator.SetSeed(43000);
			ret.w = lib_glsl_noiseGenerator.GetWhiteNoise(pos.x, pos.y, pos.z, pos.w);

			return ret;
		}

		/* trigonometry */
		bv_variable lib_glsl_acosh(bv_program* prog, u8 count, bv_variable* args)
		{
			/* acosh(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // acosh(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::acosh(sd::AsVector<4, float>(args[0])); // acosh takes only float vectors

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // acosh(scalar)
					return bv_variable_create_float(glm::acosh(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f); // floor() must have 1 argument!
		}
		bv_variable lib_glsl_asinh(bv_program* prog, u8 count, bv_variable* args)
		{
			/* asinh(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // asinh(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::asinh(sd::AsVector<4, float>(args[0])); // asinh takes only float vectors

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // asinh(scalar)
					return bv_variable_create_float(glm::asinh(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_atanh(bv_program* prog, u8 count, bv_variable* args)
		{
			/* atanh(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // atanh(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::atanh(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // atanh(scalar)
					return bv_variable_create_float(glm::atanh(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}

		/* mathematics */
		bv_variable lib_glsl_fwidthCoarse(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO: requires dFdx/yCoarse */
			/* fwidthCoarse(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // fwidthCoarse(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					return Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
				}
				else {} // fwidthCoarse(scalar)
			}
			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_fwidthFine(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO: requires dFdx/yFine */
			/* fwidthFine(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // fwidthFine(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					return Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
				}
				else {} // fwidthFine(scalar)
			}
			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_mod(bv_program* prog, u8 count, bv_variable* args)
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

					glm::vec4 vecData = glm::mod(x, y);

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

					return bv_variable_create_float(glm::mod(x, y));
				}
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_noise1(bv_program* prog, u8 count, bv_variable* args)
		{
			/* noise1(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) { // ceil(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::ceil(sd::AsVector<4, float>(args[0]));

					return bv_variable_create_float(noise1(vecData));
				}
				else // noise1(scalar)
					return bv_variable_create_float(noise1(glm::vec4(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0])), 0.0f, 0.0f, 0.0f)));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_noise2(bv_program* prog, u8 count, bv_variable* args)
		{
			bv_variable ret = Common::create_float2(prog);
			bv_object* retObj = bv_variable_get_object(ret);

			/* noise2(genType) */
			if (count == 1) {
				glm::vec2 noiseData(0.0f);

				if (args[0].type == bv_type_object) { // ceil(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::ceil(sd::AsVector<4, float>(args[0]));

					noiseData = noise2(vecData);
				}
				else // noise2(scalar)
					noiseData = noise2(glm::vec4(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0])), 0.0f, 0.0f, 0.0f));

				retObj->prop[0] = bv_variable_create_float(noiseData.x);
				retObj->prop[1] = bv_variable_create_float(noiseData.y);
			}

			return ret;
		}
		bv_variable lib_glsl_noise3(bv_program* prog, u8 count, bv_variable* args)
		{
			bv_variable ret = Common::create_float3(prog);
			bv_object* retObj = bv_variable_get_object(ret);

			/* noise2(genType) */
			if (count == 1) {
				glm::vec3 noiseData(0.0f);

				if (args[0].type == bv_type_object) { // ceil(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::ceil(sd::AsVector<4, float>(args[0]));

					noiseData = noise3(vecData);
				}
				else // noise2(scalar)
					noiseData = noise3(glm::vec4(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0])), 0.0f, 0.0f, 0.0f));

				retObj->prop[0] = bv_variable_create_float(noiseData.x);
				retObj->prop[1] = bv_variable_create_float(noiseData.y);
				retObj->prop[2] = bv_variable_create_float(noiseData.z);
			}

			return ret;
		}
		bv_variable lib_glsl_noise4(bv_program* prog, u8 count, bv_variable* args)
		{
			bv_variable ret = Common::create_float4(prog);
			bv_object* retObj = bv_variable_get_object(ret);

			/* noise2(genType) */
			if (count == 1) {
				glm::vec4 noiseData(0.0f);

				if (args[0].type == bv_type_object) { // ceil(vec3), ...
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::ceil(sd::AsVector<4, float>(args[0]));

					noiseData = noise4(vecData);
				}
				else // noise2(scalar)
					noiseData = noise4(glm::vec4(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0])), 0.0f, 0.0f, 0.0f));

				retObj->prop[0] = bv_variable_create_float(noiseData.x);
				retObj->prop[1] = bv_variable_create_float(noiseData.y);
				retObj->prop[2] = bv_variable_create_float(noiseData.z);
				retObj->prop[3] = bv_variable_create_float(noiseData.w);
			}

			return ret;
		}
		bv_variable lib_glsl_roundEven(bv_program* prog, u8 count, bv_variable* args)
		{
			/* round(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::roundEven(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // cos(scalar)
					return bv_variable_create_float(glm::roundEven(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}

		/* floating-point */
		bv_variable lib_glsl_floatBitsToInt(bv_program* prog, u8 count, bv_variable* args)
		{
			/* floatBitsToInt(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::ivec4 vecData = glm::floatBitsToInt(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_int, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_int(vecData[i]);

					return ret;
				}
				else // floatBitsToInt(scalar)
					return bv_variable_create_int(glm::floatBitsToInt(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_floatBitsToUint(bv_program* prog, u8 count, bv_variable* args)
		{
			/* floatBitsToUint(genType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::uvec4 vecData = glm::floatBitsToUint(sd::AsVector<4, float>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_uint, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_uint(vecData[i]);

					return ret;
				}
				else // floatBitsToUint(scalar)
					return bv_variable_create_uint(glm::floatBitsToUint(bv_variable_get_float(bv_variable_cast(bv_type_float, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_intBitsToFloat(bv_program* prog, u8 count, bv_variable* args)
		{
			/* intBitsToFloat(genIType) */
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
				else // intBitsToFloat(scalar)
					return bv_variable_create_float(glm::intBitsToFloat(bv_variable_get_int(bv_variable_cast(bv_type_int, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_uintBitsToFloat(bv_program* prog, u8 count, bv_variable* args)
		{
			/* uintBitsToFloat(genUType) */
			if (count == 1) {
				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::vec4 vecData = glm::uintBitsToFloat(sd::AsVector<4, unsigned int>(args[0]));

					bv_variable ret = Common::create_vec(prog, bv_type_float, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_float(vecData[i]);

					return ret;
				}
				else // uintBitsToFloat(scalar)
					return bv_variable_create_float(glm::uintBitsToFloat(bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[0]))));
			}

			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_packDouble2x32(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			return bv_variable_create_float(0.0f);
		}
		bv_variable lib_glsl_packHalf2x16(bv_program* prog, u8 count, bv_variable* args)
		{
			/* uint packHalf2x16(vec2) */
			if (count == 1) {
				if (args[0].type == bv_type_object)
					return bv_variable_create_uint(glm::packHalf2x16(sd::AsVector<2, float>(args[0])));
			}
			return bv_variable_create_uint(0);
		}
		bv_variable lib_glsl_packUnorm2x16(bv_program* prog, u8 count, bv_variable* args)
		{
			/* uint packUnorm2x16(vec2) */
			if (count == 1) {
				if (args[0].type == bv_type_object)
					return bv_variable_create_uint(glm::packUnorm2x16(sd::AsVector<2, float>(args[0])));
			}
			return bv_variable_create_uint(0);
		}
		bv_variable lib_glsl_packSnorm2x16(bv_program* prog, u8 count, bv_variable* args)
		{
			/* uint packUnorm2x16(vec2) */
			if (count == 1) {
				if (args[0].type == bv_type_object)
					return bv_variable_create_uint(glm::packSnorm2x16(sd::AsVector<2, float>(args[0])));
			}
			return bv_variable_create_uint(0);
		}
		bv_variable lib_glsl_packUnorm4x8(bv_program* prog, u8 count, bv_variable* args)
		{
			/* uint packUnorm4x8(vec4) */
			if (count == 1) {
				if (args[0].type == bv_type_object)
					return bv_variable_create_uint(glm::packUnorm4x8(sd::AsVector<4, float>(args[0])));
			}
			return bv_variable_create_uint(0);
		}
		bv_variable lib_glsl_packSnorm4x8(bv_program* prog, u8 count, bv_variable* args)
		{
			/* uint packSnorm4x8(vec4) */
			if (count == 1) {
				if (args[0].type == bv_type_object)
					return bv_variable_create_uint(glm::packSnorm4x8(sd::AsVector<4, float>(args[0])));
			}
			return bv_variable_create_uint(0);
		}
		bv_variable lib_glsl_unpackDouble2x32(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			return Common::create_uint2(prog);
		}
		bv_variable lib_glsl_unpackHalf2x16(bv_program* prog, u8 count, bv_variable* args)
		{
			/* vec2 unpackHalf2x16(uint) */
			return Common::create_float2(prog, glm::unpackHalf2x16(bv_variable_get_uint(args[0])));
		}
		bv_variable lib_glsl_unpackUnorm2x16(bv_program* prog, u8 count, bv_variable* args)
		{
			/* vec2 unpackUnorm2x16(uint) */
			return Common::create_float2(prog, glm::unpackUnorm2x16(bv_variable_get_uint(args[0])));
		}
		bv_variable lib_glsl_unpackSnorm2x16(bv_program* prog, u8 count, bv_variable* args)
		{
			/* vec2 unpackSnorm2x16(uint) */
			return Common::create_float2(prog, glm::unpackSnorm2x16(bv_variable_get_uint(args[0])));
		}
		bv_variable lib_glsl_unpackUnorm4x8(bv_program* prog, u8 count, bv_variable* args)
		{
			/* vec2 unpackUnorm4x8(uint) */
			return Common::create_float4(prog, glm::unpackUnorm4x8(bv_variable_get_uint(args[0])));
		}
		bv_variable lib_glsl_unpackSnorm4x8(bv_program* prog, u8 count, bv_variable* args)
		{
			/* vec2 unpackSnorm4x8(uint) */
			return Common::create_float4(prog, glm::unpackSnorm4x8(bv_variable_get_uint(args[0])));
		}

		/* vector */
		bv_variable lib_glsl_equal(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bvecX equal(vecX, vecX) */
			bv_object* x = bv_variable_get_object(args[0]);
			bv_object* y = bv_variable_get_object(args[1]);

			u8 vecSize = x->type->props.name_count;
			bv_variable ret = Common::create_vec(prog, bv_type_uchar, vecSize);
			bv_object* retObj = bv_variable_get_object(ret);

			for (u8 i = 0; i < vecSize; i++)
				retObj->prop[i] = bv_variable_create_uchar(bv_variable_op_equal(prog, x->prop[i], y->prop[i]));

			return ret;
		}
		bv_variable lib_glsl_notEqual(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bvecX notEqual(vecX, vecX) */
			bv_object* x = bv_variable_get_object(args[0]);
			bv_object* y = bv_variable_get_object(args[1]);

			u8 vecSize = x->type->props.name_count;
			bv_variable ret = Common::create_vec(prog, bv_type_uchar, vecSize);
			bv_object* retObj = bv_variable_get_object(ret);
			
			for (u8 i = 0; i < vecSize; i++)
				retObj->prop[i] = bv_variable_create_uchar(bv_variable_op_not_equal(prog, x->prop[i], y->prop[i]));

			return ret;
		}

		/* component comparison */
		bv_variable lib_glsl_greaterThan(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bvecX greaterThan(vecX, vecX) */
			bv_object* x = bv_variable_get_object(args[0]);
			bv_object* y = bv_variable_get_object(args[1]);

			u8 vecSize = x->type->props.name_count;
			bv_variable ret = Common::create_vec(prog, bv_type_uchar, vecSize);
			bv_object* retObj = bv_variable_get_object(ret);

			for (u8 i = 0; i < vecSize; i++)
				retObj->prop[i] = bv_variable_create_uchar(bv_variable_op_greater_than(prog, x->prop[i], y->prop[i]));

			return ret;
		}
		bv_variable lib_glsl_greaterThanEqual(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bvecX greaterThan(vecX, vecX) */
			bv_object* x = bv_variable_get_object(args[0]);
			bv_object* y = bv_variable_get_object(args[1]);

			u8 vecSize = x->type->props.name_count;
			bv_variable ret = Common::create_vec(prog, bv_type_uchar, vecSize);
			bv_object* retObj = bv_variable_get_object(ret);

			for (u8 i = 0; i < vecSize; i++)
				retObj->prop[i] = bv_variable_create_uchar(bv_variable_op_greater_equal(prog, x->prop[i], y->prop[i]));

			return ret;
		}
		bv_variable lib_glsl_lessThan(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bvecX lessThan(vecX, vecX) */
			bv_object* x = bv_variable_get_object(args[0]);
			bv_object* y = bv_variable_get_object(args[1]);

			u8 vecSize = x->type->props.name_count;
			bv_variable ret = Common::create_vec(prog, bv_type_uchar, vecSize);
			bv_object* retObj = bv_variable_get_object(ret);

			for (u8 i = 0; i < vecSize; i++)
				retObj->prop[i] = bv_variable_create_uchar(bv_variable_op_less_than(prog, x->prop[i], y->prop[i]));

			return ret;
		}
		bv_variable lib_glsl_lessThanEqual(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bvecX lessThanEqual(vecX, vecX) */
			bv_object* x = bv_variable_get_object(args[0]);
			bv_object* y = bv_variable_get_object(args[1]);

			u8 vecSize = x->type->props.name_count;
			bv_variable ret = Common::create_vec(prog, bv_type_uchar, vecSize);
			bv_object* retObj = bv_variable_get_object(ret);

			for (u8 i = 0; i < vecSize; i++)
				retObj->prop[i] = bv_variable_create_uchar(bv_variable_op_less_equal(prog, x->prop[i], y->prop[i]));

			return ret;
		}
		bv_variable lib_glsl_not(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bvecX greaterThan(bvecX) */
			bv_object* x = bv_variable_get_object(args[0]);

			u8 vecSize = x->type->props.name_count;
			bv_variable ret = Common::create_vec(prog, bv_type_uchar, vecSize);
			bv_object* retObj = bv_variable_get_object(ret);

			for (u8 i = 0; i < vecSize; i++)
				retObj->prop[i] = bv_variable_create_uchar(!bv_variable_get_uchar(x->prop[i]));

			return ret;
		}

		/* texture */
		bv_variable lib_glsl_texelFetch(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 2) {
				float bias = 0.0f;
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// sampler1D/2D/3D
					if (IsBasicTexture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::ivec3 uv = sd::AsVector<3, int>(args[1]);
						int lod = 0;
						if (count >= 3)
							lod = bv_variable_get_int(bv_variable_cast(bv_type_int, args[2]));

						glm::vec4 sample = tex->TexelFetch(uv.x, uv.y, uv.z, lod);
						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return Common::create_int4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return Common::create_uint4(prog, glm::uvec4(sample));

						return Common::create_float4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return Common::create_float4(prog);
		}
		bv_variable lib_glsl_texelFetchOffset(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 4) {
				float bias = 0.0f;
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// sampler1D/2D/3D
					if (IsBasicTexture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::ivec3 uv = sd::AsVector<3, int>(args[1]);
						int lod = bv_variable_get_int(bv_variable_cast(bv_type_int, args[2]));
						int offset = bv_variable_get_int(bv_variable_cast(bv_type_int, args[3]));

						glm::vec4 sample = tex->TexelFetch(uv.x + offset, uv.y + offset * (tex->Height > 1), uv.z + offset * (tex->Depth > 1), lod);
						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return Common::create_int4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return Common::create_uint4(prog, glm::uvec4(sample));

						return Common::create_float4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return Common::create_float4(prog);
		}
		bv_variable lib_glsl_texture(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 2) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler1D/2D/3D
					if (IsBasicTexture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::vec3 uv = sd::AsVector<3, float>(args[1]);

						float lod = 0.0f; // TODO: LOD calculation ?

						float bias = 0.0f;
						if (count >= 3)
							bias = bv_variable_get_float(bv_variable_cast(bv_type_float, args[2]));
						lod+=floor(bias);

						glm::vec4 sample = tex->Sample(uv.x, uv.y, uv.z, lod);
						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return Common::create_int4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return Common::create_uint4(prog, glm::uvec4(sample));

						return Common::create_float4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return Common::create_float4(prog);
		}
		bv_variable lib_glsl_textureGather(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 2) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler2D
					if (IsBasicTexture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::vec2 uv = sd::AsVector<2, float>(args[1]);

						int comp = 0;
						if (count >= 3)
							comp = bv_variable_get_int(bv_variable_cast(bv_type_int, args[2]));

						float xPixel = 1.0f / tex->Width;
						float yPixel = 1.0f / tex->Height;

						// TODO: LOD calculation

						glm::vec4 sample_i0_j1 = tex->Sample(uv.x, uv.y + yPixel, 0.0f, 0.0f);
						glm::vec4 sample_i1_j1 = tex->Sample(uv.x + xPixel, uv.y + yPixel, 0.0f, 0.0f);
						glm::vec4 sample_i1_j0 = tex->Sample(uv.x + xPixel, uv.y, 0.0f, 0.0f);
						glm::vec4 sample_i0_j0 = tex->Sample(uv.x, uv.y, 0.0f, 0.0f);

						glm::vec4 sample(0.0f);

						if (comp == 1)
							sample = glm::vec4(sample_i0_j1.y, sample_i1_j1.y, sample_i1_j0.y, sample_i0_j0.y);
						else if (comp == 2)
							sample = glm::vec4(sample_i0_j1.z, sample_i1_j1.z, sample_i1_j0.z, sample_i0_j0.z);
						else if (comp == 3)
							sample = glm::vec4(sample_i0_j1.w, sample_i1_j1.w, sample_i1_j0.w, sample_i0_j0.w);
						else
							sample = glm::vec4(sample_i0_j1.x, sample_i1_j1.x, sample_i1_j0.x, sample_i0_j0.x);

						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return Common::create_int4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return Common::create_uint4(prog, glm::uvec4(sample));

						return Common::create_float4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return Common::create_float4(prog);
		}
		bv_variable lib_glsl_textureGatherOffset(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 3) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler2D
					if (IsBasicTexture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::vec2 uv = sd::AsVector<2, float>(args[1]);
						glm::ivec2 offset = sd::AsVector<2, int>(args[2]);

						int comp = 0;
						if (count >= 4)
							comp = bv_variable_get_int(bv_variable_cast(bv_type_int, args[3]));

						float xPixel = 1.0f / tex->Width;
						float yPixel = 1.0f / tex->Height;

						uv.x += offset.x * xPixel; // ?
						uv.y += offset.y * yPixel; // ?

						glm::vec4 sample_i0_j1 = tex->Sample(uv.x, uv.y + yPixel, 0.0f, 0.0f);
						glm::vec4 sample_i1_j1 = tex->Sample(uv.x + xPixel, uv.y + yPixel, 0.0f, 0.0f);
						glm::vec4 sample_i1_j0 = tex->Sample(uv.x + xPixel, uv.y, 0.0f, 0.0f);
						glm::vec4 sample_i0_j0 = tex->Sample(uv.x, uv.y, 0.0f, 0.0f);

						glm::vec4 sample(0.0f);

						if (comp == 1)
							sample = glm::vec4(sample_i0_j1.y, sample_i1_j1.y, sample_i1_j0.y, sample_i0_j0.y);
						else if (comp == 2)
							sample = glm::vec4(sample_i0_j1.z, sample_i1_j1.z, sample_i1_j0.z, sample_i0_j0.z);
						else if (comp == 3)
							sample = glm::vec4(sample_i0_j1.w, sample_i1_j1.w, sample_i1_j0.w, sample_i0_j0.w);
						else
							sample = glm::vec4(sample_i0_j1.x, sample_i1_j1.x, sample_i1_j0.x, sample_i0_j0.x);

						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return Common::create_int4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return Common::create_uint4(prog, glm::uvec4(sample));

						return Common::create_float4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return Common::create_float4(prog);
		}
		bv_variable lib_glsl_textureGatherOffsets(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			if (count >= 1) {
				float bias = 0.0f;
				if (args[0].type == bv_type_object) {
					bv_object* sampler = bv_variable_get_object(args[0]);
					Common::create_vec(prog, get_texture_type(sampler->type->name), 4);
				}
			}

			return Common::create_float4(prog);
		}
		bv_variable lib_glsl_textureGrad(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			if (count >= 1) {
				float bias = 0.0f;
				if (args[0].type == bv_type_object) {
					bv_object* sampler = bv_variable_get_object(args[0]);
					Common::create_vec(prog, get_texture_type(sampler->type->name), 4);
				}
			}

			return Common::create_float4(prog);
		}
		bv_variable lib_glsl_textureGradOffset(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			if (count >= 1) {
				float bias = 0.0f;
				if (args[0].type == bv_type_object) {
					bv_object* sampler = bv_variable_get_object(args[0]);
					Common::create_vec(prog, get_texture_type(sampler->type->name), 4);
				}
			}

			return Common::create_float4(prog);
		}
		bv_variable lib_glsl_textureLod(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 3) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler1D/2D/3D
					if (IsBasicTexture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::vec3 uv = sd::AsVector<3, float>(args[1]);

						float lod = bv_variable_get_float(bv_variable_cast(bv_type_float, args[2]));

						glm::vec4 sample = tex->Sample(uv.x, uv.y, uv.z, lod);
						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return Common::create_int4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return Common::create_uint4(prog, glm::uvec4(sample));

						return Common::create_float4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return Common::create_float4(prog);
		}
		bv_variable lib_glsl_textureLodOffset(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 4) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler1D/2D/3D
					if (IsBasicTexture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::vec3 uv = sd::AsVector<3, float>(args[1]);
						float lod = bv_variable_get_float(bv_variable_cast(bv_type_float, args[2]));
						glm::ivec3 offset = sd::AsVector<3, int>(args[3]);

						float xPixel = 1.0f / tex->Width;
						float yPixel = 1.0f / tex->Height;
						float zPixel = 1.0f / tex->Depth;

						uv.x += offset.x * xPixel;
						if (tex->Height > 1)
							uv.y += offset.y * yPixel;
						if (tex->Depth > 1)
							uv.z += offset.z * zPixel;

						glm::vec4 sample = tex->Sample(uv.x, uv.y, uv.z, lod);
						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return Common::create_int4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return Common::create_uint4(prog, glm::uvec4(sample));

						return Common::create_float4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return Common::create_float4(prog);
		}
		bv_variable lib_glsl_textureOffset(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 3) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler1D/2D/3D
					if (IsBasicTexture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::vec3 uv = sd::AsVector<3, float>(args[1]);
						glm::ivec3 offset = sd::AsVector<3, int>(args[2]);

						float lod = 0.0f; // TODO: LOD calculation ?

						float bias = 0.0f;
						if (count >= 3)
							bias = bv_variable_get_float(bv_variable_cast(bv_type_float, args[3]));
						lod += floor(bias);

						float xPixel = 1.0f / tex->Width;
						float yPixel = 1.0f / tex->Height;
						float zPixel = 1.0f / tex->Depth;

						uv.x += offset.x * xPixel;
						if (tex->Height > 1)
							uv.y += offset.y * yPixel;
						if (tex->Depth > 1)
							uv.z += offset.z * zPixel;

						glm::vec4 sample = tex->Sample(uv.x, uv.y, uv.z, lod);
						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return Common::create_int4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return Common::create_uint4(prog, glm::uvec4(sample));

						return Common::create_float4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return Common::create_float4(prog);
		}
		bv_variable lib_glsl_textureProj(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 2) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler1D/2D/3D
					if (IsBasicTexture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::vec4 uv = sd::AsVector<4, float>(args[1]);

						float lod = 0.0f; // TODO: LOD calculation ?

						float bias = 0.0f;
						if (count >= 3)
							bias = bv_variable_get_float(bv_variable_cast(bv_type_float, args[2]));
						lod += floor(bias);

						u16 vecSize = bv_variable_get_object(args[1])->type->props.name_count;
						if (vecSize == 2)
							uv.x /= uv.y;
						else if (vecSize == 3) {
							uv.x /= uv.z;
							uv.y /= uv.z;
						}
						else if (vecSize == 4) {
							uv.x /= uv.w;
							uv.y /= uv.w;
							uv.z /= uv.w;
						}

						glm::vec4 sample = tex->Sample(uv.x, uv.y, uv.z, lod);
						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return Common::create_int4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return Common::create_uint4(prog, glm::uvec4(sample));

						return Common::create_float4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return Common::create_float4(prog);
		}
		bv_variable lib_glsl_textureProjGrad(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			if (count >= 1) {
				float bias = 0.0f;
				if (args[0].type == bv_type_object) {
					bv_object* sampler = bv_variable_get_object(args[0]);
					Common::create_vec(prog, get_texture_type(sampler->type->name), 4);
				}
			}

			return Common::create_float4(prog);
		}
		bv_variable lib_glsl_textureProjGradOffset(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			if (count >= 1) {
				float bias = 0.0f;
				if (args[0].type == bv_type_object) {
					bv_object* sampler = bv_variable_get_object(args[0]);
					Common::create_vec(prog, get_texture_type(sampler->type->name), 4);
				}
			}

			return Common::create_float4(prog);
		}
		bv_variable lib_glsl_textureProjLod(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 3) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler1D/2D/3D
					if (IsBasicTexture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::vec4 uv = sd::AsVector<4, float>(args[1]);

						float lod = bv_variable_get_float(bv_variable_cast(bv_type_float, args[2]));
						
						u16 vecSize = bv_variable_get_object(args[1])->type->props.name_count;
						if (vecSize == 2)
							uv.x /= uv.y;
						else if (vecSize == 3) {
							uv.x /= uv.z;
							uv.y /= uv.z;
						}
						else if (vecSize == 4) {
							uv.x /= uv.w;
							uv.y /= uv.w;
							uv.z /= uv.w;
						}

						glm::vec4 sample = tex->Sample(uv.x, uv.y, uv.z, lod);
						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return Common::create_int4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return Common::create_uint4(prog, glm::uvec4(sample));

						return Common::create_float4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return Common::create_float4(prog);
		}
		bv_variable lib_glsl_textureProjLodOffset(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 4) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler1D/2D/3D
					if (IsBasicTexture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::vec4 uv = sd::AsVector<4, float>(args[1]);
						glm::ivec3 offset = sd::AsVector<4, int>(args[3]);

						float lod = bv_variable_get_float(bv_variable_cast(bv_type_float, args[2]));

						float xPixel = 1.0f / tex->Width;
						float yPixel = 1.0f / tex->Height;
						float zPixel = 1.0f / tex->Depth;

						uv.x += offset.x * xPixel;
						if (tex->Height > 1)
							uv.y += offset.y * yPixel;
						if (tex->Depth > 1)
							uv.z += offset.z * zPixel;

						u16 vecSize = bv_variable_get_object(args[1])->type->props.name_count;
						if (vecSize == 2)
							uv.x /= uv.y;
						else if (vecSize == 3) {
							uv.x /= uv.z;
							uv.y /= uv.z;
						}
						else if (vecSize == 4) {
							uv.x /= uv.w;
							uv.y /= uv.w;
							uv.z /= uv.w;
						}

						glm::vec4 sample = tex->Sample(uv.x, uv.y, uv.z, lod);
						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return Common::create_int4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return Common::create_uint4(prog, glm::uvec4(sample));

						return Common::create_float4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return Common::create_float4(prog);
		}
		bv_variable lib_glsl_textureProjOffset(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 3) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler1D/2D/3D
					if (IsBasicTexture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						glm::vec4 uv = sd::AsVector<4, float>(args[1]);
						glm::ivec3 offset = sd::AsVector<3, int>(args[2]);

						float lod = 0.0f; // TODO: LOD calculation ?

						float bias = 0.0f;
						if (count >= 4)
							bias = bv_variable_get_float(bv_variable_cast(bv_type_float, args[3]));
						lod += floor(bias);

						float xPixel = 1.0f / tex->Width;
						float yPixel = 1.0f / tex->Height;
						float zPixel = 1.0f / tex->Depth;

						uv.x += offset.x * xPixel;
						if (tex->Height > 1)
							uv.y += offset.y * yPixel;
						if (tex->Depth > 1)
							uv.z += offset.z * zPixel;

						u16 vecSize = bv_variable_get_object(args[1])->type->props.name_count;
						if (vecSize == 2)
							uv.x /= uv.y;
						else if (vecSize == 3) {
							uv.x /= uv.z;
							uv.y /= uv.z;
						}
						else if (vecSize == 4) {
							uv.x /= uv.w;
							uv.y /= uv.w;
							uv.z /= uv.w;
						}

						glm::vec4 sample = tex->Sample(uv.x, uv.y, uv.z, lod);
						bv_type type = get_texture_type(sampler->type->name);

						if (type == bv_type_int)
							return Common::create_int4(prog, glm::ivec4(sample));
						else if (type == bv_type_uint)
							return Common::create_uint4(prog, glm::uvec4(sample));

						return Common::create_float4(prog, sample);
					}
					/* else if samplerCube ... */
				}
			}

			return Common::create_float4(prog);
		}
		bv_variable lib_glsl_textureQueryLevels(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 1) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler1D/2D/3D
					if (IsBasicTexture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						return bv_variable_create_int(tex->MipmapLevels);
					}
					/* else if samplerCube ... */
				}
			}

			return bv_variable_create_int(0);
		}
		bv_variable lib_glsl_textureQueryLod(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			return Common::create_float2(prog);
		}
		bv_variable lib_glsl_textureSamples(bv_program* prog, u8 count, bv_variable* args)
		{
			/* TODO */
			return bv_variable_create_int(1);
		}
		bv_variable lib_glsl_textureSize(bv_program* prog, u8 count, bv_variable* args)
		{
			if (count >= 2) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* sampler = bv_variable_get_object(args[0]);

					// gsampler1D/2D/3D
					if (IsBasicTexture(sampler->type->name)) {
						Texture* tex = (Texture*)sampler->user_data;
						float lod = bv_variable_get_int(bv_variable_cast(bv_type_int, args[1]));

						// TODO: take LOD into account

						if (GetTextureDimension(sampler->type->name) == 1)
							return bv_variable_create_int(tex->Width);
						else if (GetTextureDimension(sampler->type->name) == 3)
							return Common::create_int3(prog, glm::ivec3(tex->Width, tex->Height, tex->Depth));
						else
							return Common::create_int2(prog, glm::ivec2(tex->Width, tex->Height));
					}
					/* else if samplerCube ... */
				}
			}

			return Common::create_int2(prog);
		}

		/* matrix */
		bv_variable lib_glsl_inverse(bv_program* prog, u8 count, bv_variable* args)
		{
			bv_variable ret = bv_variable_create_void();

			if (count >= 1) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* mat = bv_variable_get_object(args[0]);
					sd::Matrix* matData = (sd::Matrix*)mat->user_data;

					sd::Matrix* copyData = CopyMatrixData(matData);

					if (matData->Rows == 2)
						copyData->Data = glm::inverse(glm::mat2(matData->Data));
					else if (matData->Rows == 3)
						copyData->Data = glm::inverse(glm::mat3(matData->Data));
					else
						copyData->Data = glm::inverse(matData->Data);

					return Common::create_mat(prog, mat->type->name, copyData);
				}
			}

			return ret;
		}
		bv_variable lib_glsl_matrixCompMult(bv_program* prog, u8 count, bv_variable* args)
		{
			bv_variable ret = bv_variable_create_void();

			if (count >= 2) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* x = bv_variable_get_object(args[0]);
					sd::Matrix* xData = (sd::Matrix*)x->user_data;

					bv_object* y = bv_variable_get_object(args[1]);
					sd::Matrix* yData = (sd::Matrix*)y->user_data;

					sd::Matrix* copyData = CopyMatrixData(xData);

					for (int x = 0; x < xData->Columns; x++) 
						for (int y = 0; y < xData->Rows; y++)
							copyData->Data[x][y] = yData->Data[x][y] * xData->Data[x][y];

					return Common::create_mat(prog, x->type->name, copyData);
				}
			}

			return ret;
		}
		bv_variable lib_glsl_outerProduct(bv_program* prog, u8 count, bv_variable* args)
		{
			bv_variable ret = bv_variable_create_void();

			if (count >= 2) {
				if (args[0].type == bv_type_object) { // floor(vec3), ...
					bv_object* x = bv_variable_get_object(args[0]);
					bv_object* y = bv_variable_get_object(args[1]);

					glm::vec4 xData = sd::AsVector<4, float>(args[0]);
					glm::vec4 yData = sd::AsVector<4, float>(args[1]);

					glm::mat4 resDat = glm::outerProduct(xData, yData);
					int resCols = y->type->props.name_count;
					int resRows = x->type->props.name_count;

					bool isDouble = x->type->name[0] == 'd';

					std::string newName = "mat" + std::to_string(resCols);
					if (resCols != resRows)
						newName += "x" + std::to_string(resRows);
					if (isDouble)
						newName = "d" + newName;

					sd::Matrix* res = new sd::Matrix();
					res->Columns = resCols;
					res->Rows = resRows;
					res->Data = resDat;
					res->Type = bv_type_float;

					return Common::create_mat(prog, newName.c_str(), res);
				}
			}

			return ret;
		}

		/* integer */
		bv_variable lib_glsl_bitfieldExtract(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bitfieldExtract(genIType, int, int) */
			if (count >= 3) {
				int offset = bv_variable_get_int(args[1]);
				int bits = bv_variable_get_int(args[2]);

				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);
					glm::ivec4 vecData = glm::bitfieldExtract(sd::AsVector<4, int>(args[0]), offset, bits);

					bv_type outType = vec->prop[0].type;
					bv_variable ret = Common::create_vec(prog, outType, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_cast(outType, bv_variable_create_int(vecData[i]));

					return ret;
				}
				// bitfieldExtract(scalar)
				else if (args[0].type == bv_type_int)
					return bv_variable_create_int(glm::bitfieldExtract(bv_variable_get_int(args[0]), offset, bits));
				// bitfieldExtract(scalar)
				else
					return bv_variable_create_uint(glm::bitfieldExtract(bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[0])), offset, bits));
			}

			return bv_variable_create_int(0);
		}
		bv_variable lib_glsl_bitfieldInsert(bv_program* prog, u8 count, bv_variable* args)
		{
			/* bitfieldInsert(genI/UType, genI/UType, int, int) */
			if (count >= 4) {
				int offset = bv_variable_get_int(args[1]);
				int bits = bv_variable_get_int(args[2]);

				if (args[0].type == bv_type_object) {
					bv_object* vec = bv_variable_get_object(args[0]);

					glm::ivec4 base = sd::AsVector<4, int>(args[0]);
					glm::ivec4 insert = sd::AsVector<4, int>(args[1]);

					glm::ivec4 vecData = glm::bitfieldInsert(base, insert, offset, bits);

					bv_type outType = vec->prop[0].type;
					bv_variable ret = Common::create_vec(prog, outType, vec->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);

					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_cast(outType, bv_variable_create_int(vecData[i]));

					return ret;
				}
				// bitfieldInsert(scalar)
				else if (args[0].type == bv_type_int) {
					int base = bv_variable_get_int(args[0]);
					int insert = bv_variable_get_int(bv_variable_cast(bv_type_int, args[1]));

					return bv_variable_create_int(glm::bitfieldInsert(base, insert, offset, bits));
				}
				// bitfieldInsert(scalar)
				else if (args[0].type == bv_type_int) {
					unsigned int base = bv_variable_get_uint(args[0]);
					unsigned int insert = bv_variable_get_uint(bv_variable_cast(bv_type_uint, args[1]));

					return bv_variable_create_uint(glm::bitfieldInsert(base, insert, offset, bits));
				}
			}

			return bv_variable_create_int(0);
		}
		bv_variable lib_glsl_uaddCarry(bv_program* prog, u8 count, bv_variable* args)
		{
			/* uaddCarry */
			if (count >= 3) {
				if (args[0].type == bv_type_object) {
					bv_object* x = bv_variable_get_object(args[0]);
					glm::uvec4 xData = sd::AsVector<4, unsigned int>(args[0]);
					glm::uvec4 yData = sd::AsVector<4, unsigned int>(args[1]);

					glm::uvec4 carry;
					glm::uvec4 res = glm::uaddCarry(xData, yData, carry);

					bv_variable* outCarry = bv_variable_get_pointer(args[2]);
					bv_object* outCarryObj = bv_variable_get_object(*outCarry);
					for (u16 i = 0; i < outCarryObj->type->props.name_count; i++)
						outCarryObj->prop[i] = bv_variable_create_uint(carry[i]);

					bv_variable ret = Common::create_vec(prog, bv_type_uint, outCarryObj->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);
					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_uint(res[i]);

					return ret;
				}
				else {
					unsigned int x = bv_variable_get_uint(args[0]);
					unsigned int y = bv_variable_get_uint(args[1]);

					unsigned int res = 0, carry = 0;
					res = glm::uaddCarry(x, y, carry);

					bv_variable* outCarry = bv_variable_get_pointer(args[2]);
					bv_variable_set_uint(outCarry, carry);

					return bv_variable_create_uint(res);
				}
			}

			return bv_variable_create_uint(0);
		}
		bv_variable lib_glsl_umulExtended(bv_program* prog, u8 count, bv_variable* args)
		{
			/* umulExtended */
			if (count >= 4) {
				if (args[0].type == bv_type_object) {
					glm::uvec4 xData = sd::AsVector<4, unsigned int>(args[0]);
					glm::uvec4 yData = sd::AsVector<4, unsigned int>(args[1]);

					glm::uvec4 msb, lsb;
					glm::umulExtended(xData, yData, msb, lsb);

					bv_variable* outMSB = bv_variable_get_pointer(args[2]);
					bv_object* outMSBObj = bv_variable_get_object(*outMSB);
					for (u16 i = 0; i < outMSBObj->type->props.name_count; i++)
						outMSBObj->prop[i] = bv_variable_create_uint(msb[i]);

					bv_variable* outLSB = bv_variable_get_pointer(args[3]);
					bv_object* outLSBObj = bv_variable_get_object(*outLSB);
					for (u16 i = 0; i < outLSBObj->type->props.name_count; i++)
						outLSBObj->prop[i] = bv_variable_create_uint(lsb[i]);
				}
				else {
					unsigned int x = bv_variable_get_uint(args[0]);
					unsigned int y = bv_variable_get_uint(args[1]);

					unsigned int msb = 0, lsb = 0;
					glm::umulExtended(x, y, msb, lsb);

					bv_variable* outMSB = bv_variable_get_pointer(args[2]);
					bv_variable_set_uint(outMSB, msb);

					bv_variable* outLSB = bv_variable_get_pointer(args[3]);
					bv_variable_set_uint(outLSB, lsb);
				}
			}

			return bv_variable_create_void();
		}
		bv_variable lib_glsl_imulExtended(bv_program* prog, u8 count, bv_variable* args)
		{
			/* imulExtended */
			if (count >= 4) {
				if (args[0].type == bv_type_object) {
					glm::ivec4 xData = sd::AsVector<4, int>(args[0]);
					glm::ivec4 yData = sd::AsVector<4, int>(args[1]);

					glm::ivec4 msb, lsb;
					glm::imulExtended(xData, yData, msb, lsb);

					bv_variable* outMSB = bv_variable_get_pointer(args[2]);
					bv_object* outMSBObj = bv_variable_get_object(*outMSB);
					for (u16 i = 0; i < outMSBObj->type->props.name_count; i++)
						outMSBObj->prop[i] = bv_variable_create_int(msb[i]);

					bv_variable* outLSB = bv_variable_get_pointer(args[3]);
					bv_object* outLSBObj = bv_variable_get_object(*outLSB);
					for (u16 i = 0; i < outLSBObj->type->props.name_count; i++)
						outLSBObj->prop[i] = bv_variable_create_int(lsb[i]);
				}
				else {
					int x = bv_variable_get_int(args[0]);
					int y = bv_variable_get_int(args[1]);

					int msb = 0, lsb = 0;
					glm::imulExtended(x, y, msb, lsb);

					bv_variable* outMSB = bv_variable_get_pointer(args[2]);
					bv_variable_set_int(outMSB, msb);

					bv_variable* outLSB = bv_variable_get_pointer(args[3]);
					bv_variable_set_int(outLSB, lsb);
				}
			}

			return bv_variable_create_void();
		}
		bv_variable lib_glsl_usubBorrow(bv_program* prog, u8 count, bv_variable* args)
		{
			/* uaddCarry */
			if (count >= 3) {
				if (args[0].type == bv_type_object) {
					bv_object* x = bv_variable_get_object(args[0]);
					glm::uvec4 xData = sd::AsVector<4, unsigned int>(args[0]);
					glm::uvec4 yData = sd::AsVector<4, unsigned int>(args[1]);

					glm::uvec4 borrow;
					glm::uvec4 res = glm::usubBorrow(xData, yData, borrow);

					bv_variable* outBorrow = bv_variable_get_pointer(args[2]);
					bv_object* outBorrowObj = bv_variable_get_object(*outBorrow);
					for (u16 i = 0; i < outBorrowObj->type->props.name_count; i++)
						outBorrowObj->prop[i] = bv_variable_create_uint(borrow[i]);

					bv_variable ret = Common::create_vec(prog, bv_type_uint, outBorrowObj->type->props.name_count);
					bv_object* retObj = bv_variable_get_object(ret);
					for (u16 i = 0; i < retObj->type->props.name_count; i++)
						retObj->prop[i] = bv_variable_create_uint(res[i]);

					return ret;
				}
				else {
					unsigned int x = bv_variable_get_uint(args[0]);
					unsigned int y = bv_variable_get_uint(args[1]);

					unsigned int res = 0, borrow = 0;
					res = glm::usubBorrow(x, y, borrow);

					bv_variable* outBorrow = bv_variable_get_pointer(args[2]);
					bv_variable_set_uint(outBorrow, borrow);

					return bv_variable_create_uint(res);
				}
			}

			return bv_variable_create_uint(0);
		}

		bv_library* Library()
		{
			bv_library* lib = bv_library_create();

			// vector types
			Common::lib_add_vec(lib, "vec4", 4);
			Common::lib_add_vec(lib, "ivec4", 4);
			Common::lib_add_vec(lib, "bvec4", 4, 1);
			Common::lib_add_vec(lib, "dvec4", 4);
			Common::lib_add_vec(lib, "uvec4", 4);
			Common::lib_add_vec(lib, "vec3", 3);
			Common::lib_add_vec(lib, "ivec3", 3);
			Common::lib_add_vec(lib, "bvec3", 3, 1);
			Common::lib_add_vec(lib, "dvec3", 3);
			Common::lib_add_vec(lib, "uvec3", 3);
			Common::lib_add_vec(lib, "vec2", 2);
			Common::lib_add_vec(lib, "ivec2", 2);
			Common::lib_add_vec(lib, "bvec2", 2, 1);
			Common::lib_add_vec(lib, "dvec2", 2);
			Common::lib_add_vec(lib, "uvec2", 2);

			// matrix types
			Common::lib_add_mat(lib, "mat2");
			Common::lib_add_mat(lib, "mat3");
			Common::lib_add_mat(lib, "mat4");
			Common::lib_add_mat(lib, "mat2x2");
			Common::lib_add_mat(lib, "mat2x3");
			Common::lib_add_mat(lib, "mat2x4");
			Common::lib_add_mat(lib, "mat3x2");
			Common::lib_add_mat(lib, "mat3x3");
			Common::lib_add_mat(lib, "mat3x4");
			Common::lib_add_mat(lib, "mat4x2");
			Common::lib_add_mat(lib, "mat4x3");
			Common::lib_add_mat(lib, "mat4x4");
			Common::lib_add_mat(lib, "dmat2");
			Common::lib_add_mat(lib, "dmat3");
			Common::lib_add_mat(lib, "dmat4");
			Common::lib_add_mat(lib, "dmat2x2");
			Common::lib_add_mat(lib, "dmat2x3");
			Common::lib_add_mat(lib, "dmat2x4");
			Common::lib_add_mat(lib, "dmat3x2");
			Common::lib_add_mat(lib, "dmat3x3");
			Common::lib_add_mat(lib, "dmat3x4");
			Common::lib_add_mat(lib, "dmat4x2");
			Common::lib_add_mat(lib, "dmat4x3");
			Common::lib_add_mat(lib, "dmat4x4");

			// sampler2D
			bv_object_info* sampler2D = bv_object_info_create("sampler2D");
			bv_library_add_object_info(lib, sampler2D);

			// trigonometry
			bv_library_add_function(lib, "acos", Common::lib_common_acos);
			bv_library_add_function(lib, "acosh", lib_glsl_acosh);
			bv_library_add_function(lib, "asin", Common::lib_common_asin);
			bv_library_add_function(lib, "asinh", lib_glsl_asinh);
			bv_library_add_function(lib, "atan", Common::lib_common_atan);
			bv_library_add_function(lib, "atanh", lib_glsl_atanh);
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
			bv_library_add_function(lib, "dFdx", Common::lib_common_dFdx);
			bv_library_add_function(lib, "dFdy", Common::lib_common_dFdy);
			bv_library_add_function(lib, "dFdxCoarse", Common::lib_common_dFdxCoarse);
			bv_library_add_function(lib, "dFdyCoarse", Common::lib_common_dFdyCoarse);
			bv_library_add_function(lib, "dFdxFine", Common::lib_common_dFdxFine);
			bv_library_add_function(lib, "dFdyFine", Common::lib_common_dFdyFine);
			bv_library_add_function(lib, "exp", Common::lib_common_exp);
			bv_library_add_function(lib, "exp2", Common::lib_common_exp2);
			bv_library_add_function(lib, "floor", Common::lib_common_floor);
			bv_library_add_function(lib, "fma", Common::lib_common_fma);
			bv_library_add_function(lib, "fract", Common::lib_common_fract);
			bv_library_add_function(lib, "fwidth", Common::lib_common_fwidth);
			bv_library_add_function(lib, "fwidthCoarse", lib_glsl_fwidthCoarse);
			bv_library_add_function(lib, "fwidthFine", lib_glsl_fwidthFine);
			bv_library_add_function(lib, "inversesqrt", Common::lib_common_inversesqrt);
			bv_library_add_function(lib, "isinf", Common::lib_common_isinf);
			bv_library_add_function(lib, "isnan", Common::lib_common_isnan);
			bv_library_add_function(lib, "log", Common::lib_common_log);
			bv_library_add_function(lib, "log2", Common::lib_common_log2);
			bv_library_add_function(lib, "max", Common::lib_common_max);
			bv_library_add_function(lib, "min", Common::lib_common_min);
			bv_library_add_function(lib, "mix", Common::lib_common_mix);
			bv_library_add_function(lib, "mod", lib_glsl_mod);
			bv_library_add_function(lib, "modf", Common::lib_common_modf);
			bv_library_add_function(lib, "noise1", lib_glsl_noise1);
			bv_library_add_function(lib, "noise2", lib_glsl_noise2);
			bv_library_add_function(lib, "noise3", lib_glsl_noise3);
			bv_library_add_function(lib, "noise4", lib_glsl_noise4);
			bv_library_add_function(lib, "pow", lib_glsl_noise4);
			bv_library_add_function(lib, "round", Common::lib_common_round);
			bv_library_add_function(lib, "roundEven", lib_glsl_roundEven);
			bv_library_add_function(lib, "sign", Common::lib_common_sign);
			bv_library_add_function(lib, "smoothstep", Common::lib_common_smoothstep);
			bv_library_add_function(lib, "sqrt", Common::lib_common_sqrt);
			bv_library_add_function(lib, "step", Common::lib_common_step);
			bv_library_add_function(lib, "trunc", Common::lib_common_trunc);

			// floating-point functions
			bv_library_add_function(lib, "floatBitsToInt", lib_glsl_floatBitsToInt);
			bv_library_add_function(lib, "floatBitsToUint", lib_glsl_floatBitsToUint);
			bv_library_add_function(lib, "frexp", Common::lib_common_frexp);
			bv_library_add_function(lib, "intBitsToFloat", lib_glsl_intBitsToFloat);
			bv_library_add_function(lib, "uintBitsToFloat", lib_glsl_uintBitsToFloat);
			bv_library_add_function(lib, "ldexp", Common::lib_common_ldexp);
			bv_library_add_function(lib, "packDouble2x32", lib_glsl_packDouble2x32);
			bv_library_add_function(lib, "packHalf2x16", lib_glsl_packHalf2x16);
			bv_library_add_function(lib, "packUnorm2x16", lib_glsl_packUnorm2x16);
			bv_library_add_function(lib, "packSnorm2x16", lib_glsl_packSnorm2x16);
			bv_library_add_function(lib, "packUnorm4x8", lib_glsl_packUnorm4x8);
			bv_library_add_function(lib, "packSnorm4x8", lib_glsl_packSnorm4x8);
			bv_library_add_function(lib, "unpackDouble2x32", lib_glsl_unpackDouble2x32);
			bv_library_add_function(lib, "unpackHalf2x16", lib_glsl_unpackHalf2x16);
			bv_library_add_function(lib, "unpackUnorm2x16", lib_glsl_unpackUnorm2x16);
			bv_library_add_function(lib, "unpackSnorm2x16", lib_glsl_unpackSnorm2x16);
			bv_library_add_function(lib, "unpackUnorm4x8", lib_glsl_unpackUnorm4x8);
			bv_library_add_function(lib, "unpackSnorm4x8", lib_glsl_unpackSnorm4x8);
			
			// vector
			bv_library_add_function(lib, "cross", Common::lib_common_cross);
			bv_library_add_function(lib, "distance", Common::lib_common_distance);
			bv_library_add_function(lib, "dot", Common::lib_common_dot);
			bv_library_add_function(lib, "equal", lib_glsl_equal);
			bv_library_add_function(lib, "faceforward", Common::lib_common_faceforward);
			bv_library_add_function(lib, "length", Common::lib_common_length);
			bv_library_add_function(lib, "normalize", Common::lib_common_normalize);
			bv_library_add_function(lib, "notEqual", lib_glsl_notEqual);
			bv_library_add_function(lib, "reflect", Common::lib_common_reflect);
			bv_library_add_function(lib, "refract", Common::lib_common_refract);

			// component comparison
			bv_library_add_function(lib, "all", Common::lib_common_all);
			bv_library_add_function(lib, "any", Common::lib_common_any);
			bv_library_add_function(lib, "greaterThan", lib_glsl_greaterThan);
			bv_library_add_function(lib, "greaterThanEqual", lib_glsl_greaterThanEqual);
			bv_library_add_function(lib, "lessThan", lib_glsl_lessThan);
			bv_library_add_function(lib, "lessThanEqual", lib_glsl_lessThanEqual);
			bv_library_add_function(lib, "not", lib_glsl_not);

			// texture
			bv_library_add_function(lib, "texelFetch", lib_glsl_texelFetch);
			bv_library_add_function(lib, "texelFetchOffset", lib_glsl_texelFetchOffset);
			bv_library_add_function(lib, "texture", lib_glsl_texture);
			bv_library_add_function(lib, "textureGather", lib_glsl_textureGather);
			bv_library_add_function(lib, "textureGatherOffset", lib_glsl_textureGatherOffset);
			bv_library_add_function(lib, "textureGatherOffsets", lib_glsl_textureGatherOffsets);
			bv_library_add_function(lib, "textureGrad", lib_glsl_textureGrad);
			bv_library_add_function(lib, "textureGradOffset", lib_glsl_textureGradOffset);
			bv_library_add_function(lib, "textureLod", lib_glsl_textureLod);
			bv_library_add_function(lib, "textureLod", lib_glsl_textureLodOffset);
			bv_library_add_function(lib, "textureOffset", lib_glsl_textureOffset);
			bv_library_add_function(lib, "textureProj", lib_glsl_textureProj);
			bv_library_add_function(lib, "textureProjGrad", lib_glsl_textureProjGrad);
			bv_library_add_function(lib, "textureProjGradOffset", lib_glsl_textureProjGradOffset);
			bv_library_add_function(lib, "textureProjLod", lib_glsl_textureProjLod);
			bv_library_add_function(lib, "textureProjLodOffset", lib_glsl_textureProjLodOffset);
			bv_library_add_function(lib, "textureProjOffset", lib_glsl_textureProjOffset);
			bv_library_add_function(lib, "textureQueryLevels", lib_glsl_textureQueryLevels);
			bv_library_add_function(lib, "textureQueryLod", lib_glsl_textureQueryLod);
			bv_library_add_function(lib, "textureSamples", lib_glsl_textureSamples);
			bv_library_add_function(lib, "textureSize", lib_glsl_textureSize);

			// matrix
			bv_library_add_function(lib, "determinant", Common::lib_common_determinant);
			bv_library_add_function(lib, "inverse", lib_glsl_inverse);
			bv_library_add_function(lib, "matrixCompMult", lib_glsl_matrixCompMult);
			bv_library_add_function(lib, "outerProduct", lib_glsl_outerProduct);
			bv_library_add_function(lib, "transpose", Common::lib_common_transpose);

			// integer
			bv_library_add_function(lib, "bitCount", Common::lib_common_bitCount);
			bv_library_add_function(lib, "bitfieldExtract", lib_glsl_bitfieldExtract);
			bv_library_add_function(lib, "bitfieldInsert", lib_glsl_bitfieldInsert);
			bv_library_add_function(lib, "bitfieldReverse", Common::lib_common_bitfieldReverse);
			bv_library_add_function(lib, "findLSB", Common::lib_common_findLSB);
			bv_library_add_function(lib, "findMSB", Common::lib_common_findMSB);
			bv_library_add_function(lib, "uaddCarry", lib_glsl_uaddCarry);
			bv_library_add_function(lib, "umulExtended", lib_glsl_umulExtended);
			bv_library_add_function(lib, "imulExtended", lib_glsl_imulExtended);
			bv_library_add_function(lib, "usubBorrow", lib_glsl_usubBorrow);

			return lib;
		}
	}
}