#pragma once

#include <ShaderDebugger/Matrix.h>
#include <glm/glm.hpp>

extern "C" {
#include <BlueVM.h>
}

namespace sd
{
	namespace Common
	{
		bv_variable Discard(bv_program* prog, u8 count, bv_variable* args);
		void DefaultConstructor(bv_program* prog, bv_object* obj);

		bv_variable create_float3(bv_program* prog, glm::vec3 val = glm::vec3(0.0f));
		bv_variable create_int3(bv_program* prog, glm::ivec3 val = glm::ivec3(0));
		bv_variable create_uint3(bv_program* prog, glm::uvec3 val = glm::uvec3(0u));
		bv_variable create_bool3(bv_program* prog, glm::bvec3 val = glm::bvec3(false));

		bv_variable create_float4(bv_program* prog, glm::vec4 val = glm::vec4(0.0f));
		bv_variable create_int4(bv_program* prog, glm::ivec4 val = glm::ivec4(0));
		bv_variable create_uint4(bv_program* prog, glm::uvec4 val = glm::uvec4(0u));
		bv_variable create_bool4(bv_program* prog, glm::bvec4 val = glm::bvec4(false));

		bv_variable create_float2(bv_program* prog, glm::vec2 val = glm::vec2(0.0f));
		bv_variable create_int2(bv_program* prog, glm::ivec2 val = glm::ivec2(0));
		bv_variable create_uint2(bv_program* prog, glm::uvec2 val = glm::uvec2(0u));
		bv_variable create_bool2(bv_program* prog, glm::bvec2 val = glm::bvec2(false));

		bv_variable create_vec(bv_program* prog, bv_type type, u16 components);
		bv_variable create_mat(bv_program* prog, const char* name, sd::Matrix* mat);

		bv_type merge_type(bv_type type1, bv_type type2);

		/* vectors and operators */
		bv_variable lib_common_vec_constructor(bv_program* prog, bv_object* me, u8 count, bv_variable* args);
		bv_variable lib_common_vec_operator_equal(bv_program* prog, bv_object* me, u8 count, bv_variable* args);
		bv_variable lib_common_vec_operator_add(bv_program* prog, bv_object* me, u8 count, bv_variable* args);
		bv_variable lib_common_vec_operator_minus(bv_program* prog, bv_object* me, u8 count, bv_variable* args);
		bv_variable lib_common_vec_operator_greater(bv_program* prog, bv_object* me, u8 count, bv_variable* args);
		bv_variable lib_common_vec_operator_divide(bv_program* prog, bv_object* me, u8 count, bv_variable* args);
		bv_variable lib_common_vec_operator_multiply(bv_program* prog, bv_object* me, u8 count, bv_variable* args);
		bv_variable lib_common_vec_operator_increment(bv_program* prog, bv_object* me, u8 count, bv_variable* args);
		bv_variable lib_common_vec_operator_decrement(bv_program* prog, bv_object* me, u8 count, bv_variable* args);
		bv_variable lib_common_vec_operator_logical_not(bv_program* prog, bv_object* me, u8 count, bv_variable* args);
		bv_variable lib_common_vec_operator_array_get(bv_program* prog, bv_object* me, u8 count, bv_variable* args);
		bv_variable lib_common_vec_operator_array_set(bv_program* prog, bv_object* me, u8 count, bv_variable* args);

		/* matrices and operators */
		bv_variable lib_common_mat_constructor(bv_program* prog, bv_object* me, u8 count, bv_variable* args);
		void lib_common_mat_destructor(bv_object* me);
		void* lib_common_mat_copy(void* udata);
		bv_variable lib_common_mat_operator_multiply(bv_program* prog, bv_object* me, u8 count, bv_variable* args);
		bv_variable lib_common_mat_operator_add(bv_program* prog, bv_object* me, u8 count, bv_variable* args);
		bv_variable lib_common_mat_operator_equal(bv_program* prog, bv_object* me, u8 count, bv_variable* args);
		bv_variable lib_common_mat_operator_increment(bv_program* prog, bv_object* me, u8 count, bv_variable* args);
		bv_variable lib_common_mat_operator_decrement(bv_program* prog, bv_object* me, u8 count, bv_variable* args);
		bv_variable lib_common_mat_operator_array_get(bv_program* prog, bv_object* me, u8 count, bv_variable* args);
		bv_variable lib_common_mat_operator_array_set(bv_program* prog, bv_object* me, u8 count, bv_variable* args);

		/* trigonometry */
		bv_variable lib_common_acos(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_asin(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_atan(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_cos(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_cosh(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_sin(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_sinh(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_tan(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_tanh(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_degrees(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_radians(bv_program* prog, u8 count, bv_variable* args);

		/* mathematics */
		bv_variable lib_common_abs(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_ceil(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_clamp(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_dFdx(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_dFdy(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_dFdxCoarse(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_dFdyCoarse(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_dFdxFine(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_dFdyFine(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_exp(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_exp2(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_floor(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_fma(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_fract(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_fwidth(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_inversesqrt(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_isinf(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_isnan(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_log(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_log2(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_max(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_min(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_mix(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_modf(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_pow(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_round(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_sign(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_smoothstep(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_sqrt(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_step(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_trunc(bv_program* prog, u8 count, bv_variable* args);

		// vector
		bv_variable lib_common_cross(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_distance(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_dot(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_faceforward(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_length(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_normalize(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_reflect(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_refract(bv_program* prog, u8 count, bv_variable* args);

		// component comparison
		bv_variable lib_common_all(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_any(bv_program* prog, u8 count, bv_variable* args);

		// matrix
		bv_variable lib_common_determinant(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_transpose(bv_program* prog, u8 count, bv_variable* args);

		// integer
		bv_variable lib_common_bitfieldReverse(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_bitCount(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_findLSB(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_findMSB(bv_program* prog, u8 count, bv_variable* args);

		// floating point
		bv_variable lib_common_frexp(bv_program* prog, u8 count, bv_variable* args);
		bv_variable lib_common_ldexp(bv_program* prog, u8 count, bv_variable* args);

		/* helper functions to create vector & matrix definitions */
		bv_object_info* lib_add_vec(bv_library* lib, const char* name, u8 comp, u8 logNot = 0);
		bv_object_info* lib_add_mat(bv_library* lib, const char* name);
	}
}