#pragma once
#include <glm/glm.hpp>
#include <ShaderDebugger/Texture.h>

namespace sd
{
	enum class CubemapFace {
		PositiveX,
		NegativeX,
		PositiveY,
		NegativeY,
		PositiveZ,
		NegativeZ
	};

	class TextureCube
	{
	public:
		TextureCube();

		sd::Texture* Faces[6];

		unsigned int UserData; // you can save your GL texture here (* but don't have to *)

		bool Set(CubemapFace face, sd::Texture* tex);
		glm::vec4 Sample(glm::vec3 normal);

	private:
		CubemapFace m_getCubemapFace(glm::vec3 n, glm::vec2& uv);
	};
}