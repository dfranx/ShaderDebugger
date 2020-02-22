#include <ShaderDebugger/TextureCube.h>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

namespace sd
{
	TextureCube::TextureCube()
	{
		memset(&Faces[0], 0, sizeof(sd::Texture*) * 6);
	}
	glm::vec4 TextureCube::Sample(glm::vec3 normal)
	{
		// TODO: mipmaps
		// TODO: filtering
		// TODO: wrap

		glm::vec2 uv(0.0f);
		int face = (int)m_getCubemapFace(normal, uv);

		if (Faces[face] != nullptr)
			return Faces[face]->Sample(uv.x, uv.y);

		return glm::vec4(1.0f);
	}
	CubemapFace TextureCube::m_getCubemapFace(glm::vec3 normal, glm::vec2& uv)
	{
		/* https://github.com/zauonlok/renderer/blob/a6b50f06e85659edbc8ee18c674e6f781aacfd84/renderer/core/texture.c#L210 */
		float absX = (float)fabs(normal.x);
		float absY = (float)fabs(normal.y);
		float absZ = (float)fabs(normal.z);
		float ma, u, v;
		CubemapFace ret;

		if (absX > absY && absX > absZ) {
			ma = absX;
			if (normal.x > 0) {
				ret = CubemapFace::PositiveX;
				u = -normal.z;
				v = -normal.y;
			}
			else {
				ret = CubemapFace::NegativeX;
				u = +normal.z;
				v = -normal.y;
			}
		} else if (absY > absZ) {
			ma = absY;
			if (normal.y > 0) {
				ret = CubemapFace::PositiveY;
				u = +normal.x;
				v = +normal.z;
			}
			else {
				ret = CubemapFace::NegativeY;
				u = +normal.x;
				v = -normal.z;
			}
		} else {
			ma = absZ;
			if (normal.z > 0) {
				ret = CubemapFace::PositiveZ;
				u = +normal.x;
				v = -normal.y;
			}
			else {
				ret = CubemapFace::NegativeZ;
				u = -normal.x;
				v = -normal.y;
			}
		}

		uv = glm::vec2((u / ma + 1) / 2, (v / ma + 1) / 2);

		return ret;
	}
}