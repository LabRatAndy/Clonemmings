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
#pragma once
#include <glm/glm.hpp>
#include <filesystem>
namespace Clonemmings
{
	class Texture
	{
	public:
		Texture(const std::filesystem::path& filename);
		Texture(uint32_t width, uint32_t height, const glm::vec4& colour);
		~Texture();

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		const std::string& GetPath() const { return m_FilenameStr; }
		bool IsLoaded() const { return m_IsLoaded; }

		void Bind(uint32_t slot = 0) const;
		void UnBind(uint32_t slot = 0) const;

		bool operator==(const Texture& other) { return m_Handle == other.m_Handle; }
		bool operator!=(const Texture& other) { return m_Handle != other.m_Handle; }
	private:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		const std::filesystem::path& m_Filename;
		uint32_t m_Handle = 0;
		bool m_IsLoaded = false;
		std::string m_FilenameStr;
	};
}