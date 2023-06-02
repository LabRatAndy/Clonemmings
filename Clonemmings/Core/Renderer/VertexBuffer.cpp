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
#include "Core/Renderer/VertexBuffer.h"
#include "Core/Application/Assert.h"
#include <glad/glad.h>

namespace Clonemmings
{
	// size is size in bytes of data buffer ie length of data * sizeof(vertex)
	VertexBufferObject::VertexBufferObject(void* data, size_t size, VertexType vertextype) :m_VertexType(vertextype)
	{
		glCreateBuffers(1, &m_Handle);
		glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		switch (m_VertexType)
		{
		case  VertexType::Coloured:
			m_VertexCount = size / sizeof(ColouredVertex);
			break;
		case VertexType::Textured:
			m_VertexCount = size / sizeof(TexturedVertex);
			break;
		case VertexType::Batch:
			m_VertexCount = size / sizeof(BatchedVertex);
			break;
		case VertexType::Line:
			m_VertexCount = size / sizeof(LineVertex);
			break;
		default:
			ASSERT(false, "Vertex type is not set!");
			break;
		}
	}
	// size is the amount of space to reserve in buffer
	VertexBufferObject::VertexBufferObject(size_t size, VertexType vertextype) : m_VertexType(vertextype)
	{
		glCreateBuffers(1, &m_Handle);
		glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		switch (m_VertexType)
		{
		case  VertexType::Coloured:
			m_VertexCount = size / sizeof(ColouredVertex);
			break;
		case VertexType::Textured:
			m_VertexCount = size / sizeof(TexturedVertex);
			break;
		case VertexType::Batch:
			m_VertexCount = size / sizeof(BatchedVertex);
			break;
		case VertexType::Line:
			m_VertexCount = size / sizeof(LineVertex);
			break;
		default:
			ASSERT(false, "Vertex type is not set!");
			break;
		}
	}
	VertexBufferObject::~VertexBufferObject()
	{
		if (m_Handle) glDeleteBuffers(1, &m_Handle);
	}
	void VertexBufferObject::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
	}
	void VertexBufferObject::UnBind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void VertexBufferObject::SetData(void* data, uint32_t size, uint32_t offset)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
	}
	void* VertexBufferObject::GetMappedDataPointer()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
		return glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	}
	void VertexBufferObject::UnmapDataPointer()
	{
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	void VertexBufferObject::Draw(uint32_t start, uint32_t count) const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
		glDrawArrays(GL_TRIANGLES, start, count);
	}
	void VertexBufferObject::Draw() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
		glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
	}
	void VertexBufferObject::DrawLines(uint32_t count) const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
		glDrawArrays(GL_LINES, 0, count);
	}
}