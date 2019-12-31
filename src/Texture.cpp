#include <ShaderDebugger/Texture.h>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

namespace sd
{
	Texture::Texture()
	{
		Data = nullptr;
		Width = Height = Depth = 0;
		MipmapLevels = 0;
		MinFilter = Filter::Nearest;
		MagFilter = Filter::Nearest;
		WrapU = Wrap::Repeat;
		WrapV = Wrap::Repeat;
	}
	Texture::~Texture()
	{
		m_cleanup();
	}
	bool Texture::Allocate(int w, int h, int d)
	{
		m_cleanup();

		if (d <= 0 || w <= 0 || h <= 0)
			return false;

		Width = w;
		Height = h;
		Depth = d;
		MipmapLevels = 1;

		Data = (float**)malloc(sizeof(float*) * MipmapLevels);
		
		// TODO: mipmaps
		Data[0] = (float*)malloc(sizeof(float) * w * h * d * 4);

		return true;
	}
	void Texture::Fill(glm::vec4 val)
	{
		// TODO: mipmaps
		for (int i = 0; i < Width * Height * Depth; i++)
			memcpy(Data[0] + i * 4, (void*)glm::value_ptr(val), sizeof(float) * 4);
	}
	glm::vec4 Texture::Sample(float u, float v, float w, float mip)
	{
		// TODO: mipmaps
		// TODO: filtering
		// TODO: wrap

		if (m_isInvalid())
			return glm::vec4(1.0f);

		// kind of "nearest" filtering
		int x = u * Width;
		int y = v * Height;
		int z = w * Depth;
		int m = mip * MipmapLevels;

		// clamp (TODO: wrap)
		x = std::min(Width, std::max(0, x));
		y = std::min(Height, std::max(0, y));
		z = std::min(Depth, std::max(0, z));

		// clamp mipmap level
		m = std::min(MipmapLevels, std::max(0, m));

		float* mem = &Data[m][(x + Width * (y + Depth * z)) * 4];

		return glm::vec4(mem[0], mem[1], mem[2], mem[3]);
	}
	glm::vec4 Texture::TexelFetch(int u, int v, int w, int mip)
	{
		if (m_isInvalid())
			return glm::vec4(1.0f);
		
		// clamp (TODO: wrap)
		u = std::min(Width, std::max(0, w));
		v = std::min(Height, std::max(0, v));
		w = std::min(Depth, std::max(0, w));

		// clamp mipmap level
		mip = std::min(MipmapLevels, std::max(0, mip));

		float* mem = &Data[mip][(u + Width * (v + Depth * w)) * 4];

		return glm::vec4(mem[0], mem[1], mem[2], mem[3]);
	}
	void Texture::m_cleanup()
	{
		if (Data != nullptr) {
			for (unsigned int i = 0; i < MipmapLevels; i++)
				free(Data[i]);
			free(Data);
			Data = nullptr;
		}
		Width = Height = Depth = 0;
		MipmapLevels = 0;
		MinFilter = Filter::Nearest;
		MagFilter = Filter::Nearest;
		WrapU = Wrap::Repeat;
		WrapV = Wrap::Repeat;
	}
	bool Texture::m_isInvalid()
	{
		return Width <= 0 || Height <= 0 || Depth <= 0 || MipmapLevels <= 0 || Data == nullptr;
	}
}