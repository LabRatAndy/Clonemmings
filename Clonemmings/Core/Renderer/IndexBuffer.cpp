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

#include "Core/Renderer/IndexBuffer.h"
#include "Core/Application/Assert.h"
#include <glad/glad.h>

namespace Clonemmings
{
	IndexBuffer::IndexBuffer(uint32_t* data, uint32_t count) : m_Count(count), m_Type(GL_UNSIGNED_INT)
	{
		ASSERT(data, "Index buffer data is nullptr");
		ASSERT(count, "Index buffer count is zero");
		glCreateBuffers(1, &m_Handle);
		ASSERT(m_Handle, "Failed to create index buffer");
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * (sizeof(uint32_t)), data, GL_STATIC_DRAW);
	}
	IndexBuffer::IndexBuffer(uint16_t* data, uint16_t count) : m_Count(count), m_Type(GL_UNSIGNED_SHORT)
	{
		ASSERT(data, "Index buffer data is nullptr");
		ASSERT(count, "Index buffer count is zero");
		glCreateBuffers(1, &m_Handle);
		ASSERT(m_Handle, "Failed to create index buffer");
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint16_t), data, GL_STATIC_DRAW);
	}
	IndexBuffer::IndexBuffer(uint32_t count) : m_Count(count), m_Type(GL_UNSIGNED_INT)
	{
		ASSERT(count, "Index buffer count is zero");
		glCreateBuffers(1, &m_Handle);
		ASSERT(m_Handle, "Failed to create index buffer");
		uint32_t* data = new uint32_t[count];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < count; i += 6)
		{
			data[i + 0] = offset + 0;
			data[i + 1] = offset + 1;
			data[i + 2] = offset + 2;
			data[i + 3] = offset + 2;
			data[i + 4] = offset + 3;
			data[i + 5] = offset + 0;
			offset += 4;
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_DYNAMIC_DRAW);
		delete[] data;
	}
	IndexBuffer::~IndexBuffer()
	{
		if (m_Handle)glDeleteBuffers(1, &m_Handle);
	}

	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle);
	}
	void IndexBuffer::UnBind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	void IndexBuffer::Draw() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle);
		glDrawElements(GL_TRIANGLES, m_Count, m_Type, NULL);
	}
}