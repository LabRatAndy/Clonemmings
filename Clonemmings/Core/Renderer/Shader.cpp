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
#include "Core/Renderer/Shader.h"
#include "Core/Application/Assert.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
namespace Clonemmings
{
	Shader::Shader(const std::filesystem::path& vertexfilename, const std::filesystem::path& fragmentfilename) :
		m_VertexFilename(vertexfilename), m_FragmentFilename(fragmentfilename)//, m_VertexSource((std::string&)""), m_FragmentSource((std::string&)"")
	{
		m_VertexSource = LoadShaderFile(m_VertexFilename);
		m_FragmentSource = LoadShaderFile(m_FragmentFilename);
		CompileShader();
	}
	Shader::Shader(const std::string& vertexsource, const std::string& fragmentsource) :
		m_VertexSource((std::string&)vertexsource), m_FragmentSource((std::string&)fragmentsource), m_VertexFilename(""), m_FragmentFilename("")
	{
		CompileShader();
	}
	Shader::~Shader()
	{
		glDeleteProgram(m_Handle);
	}
	void Shader::Bind() const
	{
		glUseProgram(m_Handle);

	}
	void Shader::UnBind() const
	{
		glUseProgram(0);
	}
	const std::string Shader::LoadShaderFile(const std::filesystem::path& filename)
	{
		ASSERT(filename.has_filename(), "Error the std::filesystem::path filename has no filename component!");
		ASSERT(std::filesystem::exists(filename), "Error filename does not exists!");

		std::ifstream file(filename, std::ios::binary);
		if (!file.is_open())
		{
			CRITICAL("unable to open file {0}", filename.string());
			return "";
		}
		file.seekg(0, std::ios::end);
		size_t filesize = file.tellg();
		if (filesize == -1)
		{
			CRITICAL("unable to read file invalid file size for shader source file: {0} ", filename.string());
			file.close();
			return "";
		}
		std::string source;
		source.resize(filesize);
		file.seekg(0, std::ios::beg);
		file.read(source.data(), filesize);
		file.close();
		return source;
	}
	void Shader::CompileShader()
	{
		ASSERT(!m_VertexSource.empty(), "No vertex shader source code has been given!");
		ASSERT(!m_FragmentSource.empty(), "No fragment shader source code has been given!");
		m_Handle = glCreateProgram();
		uint32_t vertexshaderhandle = glCreateShader(GL_VERTEX_SHADER);
		//vertex shader
		{
			const GLchar* source = m_VertexSource.c_str();
			const GLint sourcelength = (const GLint)m_VertexSource.length();
			glShaderSource(vertexshaderhandle, 1, &source, &sourcelength);
			glCompileShader(vertexshaderhandle);
			int32_t sucess = 0;
			glGetShaderiv(vertexshaderhandle, GL_COMPILE_STATUS, &sucess);
			if (sucess == GL_FALSE)
			{
				int32_t errorlength = 0;
				glGetShaderiv(vertexshaderhandle, GL_INFO_LOG_LENGTH, &errorlength);
				std::string error;
				error.resize(errorlength);
				glGetShaderInfoLog(vertexshaderhandle, errorlength, &errorlength, error.data());
				glDeleteShader(vertexshaderhandle);
				CRITICAL("Vertex shader failed to compile with the following error: {0}", error);
				return;
			}
		}
		uint32_t fragmemntshaderhandle = glCreateShader(GL_FRAGMENT_SHADER);
		//fragment shader
		{
			const GLchar* source = m_FragmentSource.c_str();
			const GLint sourcelength = (const GLint)m_FragmentSource.length();
			glShaderSource(fragmemntshaderhandle, 1, &source, &sourcelength);
			glCompileShader(fragmemntshaderhandle);
			int32_t sucess = 0;
			glGetShaderiv(fragmemntshaderhandle, GL_COMPILE_STATUS, &sucess);
			if (sucess == GL_FALSE)
			{
				int32_t errorlength = 0;
				glGetShaderiv(fragmemntshaderhandle, GL_INFO_LOG_LENGTH, &errorlength);
				std::string error;
				error.resize(errorlength);
				glGetShaderInfoLog(fragmemntshaderhandle, errorlength, &errorlength, error.data());
				glDeleteShader(vertexshaderhandle);
				glDeleteShader(fragmemntshaderhandle);
				CRITICAL("Fragment shader failed to compile with the following error: {0}", error);
				return;
			}
		}
		//link shader
		glAttachShader(m_Handle, vertexshaderhandle);
		glAttachShader(m_Handle, fragmemntshaderhandle);
		glLinkProgram(m_Handle);
		{
			int32_t sucess = 0;
			glGetProgramiv(m_Handle, GL_LINK_STATUS, &sucess);
			if (sucess == GL_FALSE)
			{
				int32_t errorlength = 0;
				glGetProgramiv(m_Handle, GL_INFO_LOG_LENGTH, &errorlength);
				std::string error;
				error.resize(errorlength);
				glGetProgramInfoLog(m_Handle, errorlength, &errorlength, error.data());
				glDetachShader(m_Handle, vertexshaderhandle);
				glDetachShader(m_Handle, fragmemntshaderhandle);
				glDeleteShader(vertexshaderhandle);
				glDeleteShader(fragmemntshaderhandle);
				glDeleteProgram(m_Handle);
				m_Handle = 0;
				CRITICAL("Shader program failed to link with the following error: {0}", error);
				return;
			}
		}
		//clean up
		glDetachShader(m_Handle, vertexshaderhandle);
		glDetachShader(m_Handle, fragmemntshaderhandle);
		glDeleteShader(vertexshaderhandle);
		glDeleteShader(fragmemntshaderhandle);
	}
	void Shader::CheckUniformCache(const std::string& name)
	{
		if (m_UniformCache.find(name) == m_UniformCache.end())
		{
			int32_t uniformhandle = glGetUniformLocation(m_Handle, name.c_str());
			ASSERT(uniformhandle != -1, "Uniform handle not found");
			m_UniformCache[name] = uniformhandle;
		}
	}
	void Shader::SetInt(const std::string& name, int32_t& value)
	{
		CheckUniformCache(name);
		glUniform1i(m_UniformCache[name], value);
	}
	void Shader::SetIntArray(const std::string& name, int32_t* values, uint32_t count)
	{
		CheckUniformCache(name);
		glUniform1iv(m_UniformCache[name], count, values);
	}
	void Shader::SetFloat(const std::string& name, float& value)
	{
		CheckUniformCache(name);
		glUniform1f(m_UniformCache[name], value);
	}
	void Shader::SetFloat2(const std::string& name, glm::vec2& value)
	{
		CheckUniformCache(name);
		glUniform2f(m_UniformCache[name], value.x, value.y);
	}
	void Shader::SetFloat3(const std::string& name, glm::vec3& value)
	{
		CheckUniformCache(name);
		glUniform3f(m_UniformCache[name], value.x, value.y, value.z);
	}
	void Shader::SetFloat4(const std::string& name, glm::vec4& value)
	{
		CheckUniformCache(name);
		glUniform4f(m_UniformCache[name], value.x, value.y, value.z, value.w);
	}
	void Shader::SetMat3(const std::string& name, glm::mat3& value)
	{
		CheckUniformCache(name);
		glUniformMatrix3fv(m_UniformCache[name], 1, GL_FALSE, glm::value_ptr(value));
	}
	void Shader::SetMat4(const std::string& name, glm::mat4& value)
	{
		CheckUniformCache(name);
		glUniformMatrix4fv(m_UniformCache[name], 1, GL_FALSE, glm::value_ptr(value));
	}
	
	void Shader::SetInt(const std::string& name,  const int32_t& value)
	{
		CheckUniformCache(name);
		glUniform1i(m_UniformCache[name], value);
	}
	void Shader::SetFloat(const std::string& name, const float& value)
	{
		CheckUniformCache(name);
		glUniform1f(m_UniformCache[name], value);
	}
	void Shader::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		CheckUniformCache(name);
		glUniform2f(m_UniformCache[name], value.x, value.y);
	}
	void Shader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		CheckUniformCache(name);
		glUniform3f(m_UniformCache[name], value.x, value.y, value.z);
	}
	void Shader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		CheckUniformCache(name);
		glUniform4f(m_UniformCache[name], value.x, value.y, value.z, value.w);
	}
	void Shader::SetMat3(const std::string& name, const glm::mat3& value)
	{
		CheckUniformCache(name);
		glUniformMatrix3fv(m_UniformCache[name], 1, GL_FALSE, glm::value_ptr(value));
	}
	void Shader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		CheckUniformCache(name);
		glUniformMatrix4fv(m_UniformCache[name], 1, GL_FALSE, glm::value_ptr(value));
	}
}