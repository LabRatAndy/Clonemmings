/*MIT License

Copyright(c) 2022, Andrew Dawson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this softwareand associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/
#include "Core/Renderer/Texture.h"
#include "Core/Application/Assert.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>
namespace Clonemmings
{
	Texture::Texture(const std::filesystem::path& filename) :m_Filename(filename)
	{
		ASSERT(std::filesystem::exists(m_Filename), "Texture file does not exist!");
		int width, height, channels;
		GLenum internalformat;
		GLenum dataformat;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(m_Filename.string().c_str(), &width, &height, &channels, 0);
		if (data)
		{
			m_Height = height;
			m_Width = width;
			if (channels == 4)
			{
				internalformat = GL_RGBA8;
				dataformat = GL_RGBA;
			}
			else if (channels == 3)
			{
				internalformat = GL_RGB8;
				dataformat = GL_RGB;
			}
			else
			{
				ASSERT(false, "Unsupported texture format");
			}
			glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);
			glTextureStorage2D(m_Handle, 1, internalformat, m_Width, m_Height);
			glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTextureSubImage2D(m_Handle, 0, 0, 0, m_Width, m_Height, dataformat, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
			m_IsLoaded = true;
		}
	}
	Texture::Texture(uint32_t width, uint32_t height, glm::vec4& colour): m_Width(width), m_Height(height), m_Filename("")
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);
		glTextureStorage2D(m_Handle, 1, GL_RGBA8, m_Width, m_Height);
		glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, GL_REPEAT);
		uint8_t* data = new uint8_t[(uint64_t)m_Width * (uint64_t)m_Height * 4];
		uint8_t red = ((uint8_t)colour.r) * 256;
		uint8_t green = ((uint8_t)colour.g) * 256;
		uint8_t blue = ((uint8_t)colour.b) * 256;
		uint8_t alpha = ((uint8_t)colour.a) * 256;
		for (uint32_t x = 0; x < m_Width; x++)
		{
			for (uint32_t y = 0; y < m_Height; y++)
			{
				data[x + y + 0] = red;
				data[x + y + 1] = green;
				data[x + y + 2] = blue;
				data[x + y + 3] = alpha;
			}
		}
		glTextureSubImage2D(m_Handle, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, data);
		delete[] data;
		m_IsLoaded = true;
	}
	Texture::~Texture()
	{
		glDeleteTextures(1, &m_Handle);
	}
	void Texture::Bind(uint32_t slot) const
	{
		ASSERT(m_IsLoaded, "Texture is not loaded!");
		glBindTextureUnit(slot, m_Handle);
	}
	void Texture::UnBind(uint32_t slot) const
	{
		glBindTextureUnit(slot, 0);
	}
}