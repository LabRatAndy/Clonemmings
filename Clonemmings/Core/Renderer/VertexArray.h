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
#include "Core/Renderer/VertexBuffer.h"
#include "Core/Renderer/IndexBuffer.h"
#include "Core/Renderer/Vertex.h"
#include <memory>

namespace Clonemmings
{
	class VertexArrayObject
	{
	public:
		VertexArrayObject();
		~VertexArrayObject();

		void Bind() const;
		void UnBind() const;

		void AddVertexBuffer(std::shared_ptr<VertexBufferObject> vbo);
		void SetIndexBuffer(std::shared_ptr<IndexBuffer>ibo);

		std::shared_ptr<VertexBufferObject> GetVertexBuffer() const
		{
			return m_VBO;
		}
		std::shared_ptr<IndexBuffer> GetIndexBuffer() const
		{
			return m_IBO;
		}

	private:
		uint32_t m_Handle = 0;
		std::shared_ptr<VertexBufferObject> m_VBO = nullptr;
		std::shared_ptr<IndexBuffer> m_IBO = nullptr;

		void SetColouredVertexLayout();
		void SetTexturedVertexLayout();
		void SetBatchVertexLayout();
	};
}