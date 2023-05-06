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
#include "Core/Renderer/Vertex.h"
#include <cinttypes>

namespace Clonemmings
{
	class VertexBufferObject
	{
	public:
		VertexBufferObject(void* data, size_t size, VertexType vertextype);
		VertexBufferObject(size_t size, VertexType vertextype);
		~VertexBufferObject();

		void Bind() const;
		void UnBind() const;

		void SetData(void* data, uint32_t size, uint32_t offset = 0);
		void* GetMappedDataPointer();
		void UnmapDataPointer();
		VertexType GetVertexType() const
		{
			return m_VertexType;
		}

		void Draw(uint32_t start, uint32_t count) const;
		void Draw() const;
		void DrawLines(uint32_t count) const;
	
	private:
		uint32_t m_Handle = 0;
		VertexType m_VertexType = VertexType::NotSet;
		size_t m_VertexCount = 0;
	};
}