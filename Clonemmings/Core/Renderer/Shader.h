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
#include <string>
#include <unordered_map>
namespace Clonemmings
{
	class Shader
	{
	public:
		Shader(const std::filesystem::path& vertexfilename, const std::filesystem::path& fragmentfilename);
		Shader(const std::string& vertexsource, const std::string& fragmentsource);
		~Shader();

		void Bind() const;
		void UnBind() const;

		void SetInt(const std::string& name, int32_t& value);
		void SetIntArray(const std::string& name, int32_t* values, uint32_t count);
		void SetFloat(const std::string& name, float& value);
		void SetFloat2(const std::string& name, glm::vec2& value);
		void SetFloat3(const std::string& name, glm::vec3& value);
		void SetFloat4(const std::string& name, glm::vec4& value);
		void SetMat3(const std::string& name, glm::mat3& value);
		void SetMat4(const std::string& name, glm::mat4& value);

	private:
		uint32_t m_Handle;
		const std::filesystem::path& m_VertexFilename;
		const std::filesystem::path& m_FragmentFilename;
		std::string& m_VertexSource;
		std::string& m_FragmentSource;
		std::unordered_map<std::string, int32_t> m_UniformCache;

		const std::string LoadShaderFile(const std::filesystem::path& filename);
		void CompileShader();
		void CheckUniformCache(const std::string& name);
	};
}