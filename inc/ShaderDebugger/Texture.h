#pragma once
#include <glm/glm.hpp>

namespace sd
{
	class Texture
	{
	public:
		Texture();
		~Texture();

		float** Data; // [MipmapLevels][Width * Height * Depth * 4] -> must have 4 components
		int Width, Height, Depth;
		int MipmapLevels;

		enum class Filter
		{
			Linear,
			Nearest,
			Nearest_MipmapNearest,
			Linear_MipmapNearest,
			Nearest_MipmapLinear,
			Linear_MipmapLinear
		};
		Filter MinFilter, MagFilter;

		enum class Wrap
		{
			ClampToEdge,
			MirroredRepeat,
			Repeat
		};
		Wrap WrapU, WrapV;

		bool Allocate(int w, int h, int d = 1);
		void Fill(glm::vec4 val);
		glm::vec4 Sample(float u, float v, float w = 0.0f, float mip = 0.0f);

		// TODO: GenerateMipmaps()

	private:
		void m_cleanup();
		bool m_isInvalid();
	};
}