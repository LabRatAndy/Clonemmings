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
#include "Core/Renderer/VertexArray.h"
#include "Core/Application/Assert.h"
#include <glad/glad.h>
namespace Clonemmings
{
	VertexArrayObject::VertexArrayObject()
	{
		glCreateVertexArrays(1, &m_Handle);
	}

	VertexArrayObject::~VertexArrayObject()
	{
		glDeleteVertexArrays(1, &m_Handle);
	}

	void VertexArrayObject::Bind() const
	{
		glBindVertexArray(m_Handle);
		GLenum errorcode = glGetError();
	}
	void VertexArrayObject::UnBind() const
	{
		glBindVertexArray(0);
	}
	void VertexArrayObject::AddVertexBuffer(std::shared_ptr<VertexBufferObject>vbo)
	{
		ASSERT(vbo, "VBO passed is nullptr");
		Bind();
		m_VBO = vbo;
		m_VBO->Bind();
		switch(m_VBO->GetVertexType())
		{
		case VertexType::Coloured:
			SetColouredVertexLayout();
			break;
		case VertexType::Textured:
			SetTexturedVertexLayout();
			break;
		case VertexType::Batch:
			SetBatchVertexLayout();
			break;
		case VertexType::NotSet:
			ASSERT("Vertex buffer has no vertex type set");
			break;
		}
	}
	void VertexArrayObject::SetIndexBuffer(std::shared_ptr<IndexBuffer>ibo)
	{
		ASSERT(ibo, "IBO passed is nullptr");
		Bind();
		m_IBO = ibo;
		m_IBO->Bind();
	}
	void VertexArrayObject::SetColouredVertexLayout()
	{
		size_t positionoffset = offsetof(ColouredVertex, Position);
		size_t normaloffset = offsetof(ColouredVertex, Normal);
		size_t colouroffset = offsetof(ColouredVertex, Colour);
		size_t entityidoffset = offsetof(ColouredVertex, EntityID);
		glEnableVertexAttribArray(0);	//position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColouredVertex), (const void*)positionoffset);
		glEnableVertexAttribArray(1);	//normal
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ColouredVertex), (const void*)normaloffset);
		glEnableVertexAttribArray(2);	//colour
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ColouredVertex), (const void*)colouroffset);
		glEnableVertexAttribArray(3);	// Entity ID
		//note when attribute is an int need to pass this version of the function. NOTE the I between the b and the p
		glVertexAttribIPointer(3, 1, GL_INT, sizeof(ColouredVertex), (const void*)entityidoffset);
	}
	void VertexArrayObject::SetTexturedVertexLayout()
	{
		size_t positionoffset = offsetof(TexturedVertex, Position);
		size_t normaloffset = offsetof(TexturedVertex, Normal);
		size_t texcoordoffset = offsetof(TexturedVertex, TexCoords);
		size_t entityidoffset = offsetof(TexturedVertex, EntityID);
		glEnableVertexAttribArray(0);	//position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (const void*)positionoffset);
		glEnableVertexAttribArray(1);	//normal
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (const void*)normaloffset);
		glEnableVertexAttribArray(2);	//Tex coords
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (const void*)texcoordoffset);
		glEnableVertexAttribArray(3);	//Entity ID
		//note when attribute is an int need to pass this version of the function. NOTE the I between the b and the p
		glVertexAttribIPointer(3, 1, GL_INT, sizeof(TexturedVertex), (const void*)entityidoffset);
	}
	void VertexArrayObject::SetBatchVertexLayout()
	{
		size_t positionoffset = offsetof(BatchedVertex, Position);
		size_t normaloffset = offsetof(BatchedVertex, Normal);
		size_t texcoordoffset = offsetof(BatchedVertex, TexCoords);
		size_t colouroffset = offsetof(BatchedVertex, Colour);
		size_t texindexoffset = offsetof(BatchedVertex, TexIndex);
		size_t tilingfactoroffset = offsetof(BatchedVertex, TilingFactor);
		size_t entityidoffset = offsetof(BatchedVertex, EntityID);
		glEnableVertexAttribArray(0);	//position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BatchedVertex), (const void*)positionoffset);
		glEnableVertexAttribArray(1);	//normal
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BatchedVertex), (const void*)normaloffset);
		glEnableVertexAttribArray(2);	//TexCoords
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(BatchedVertex), (const void*)texcoordoffset);
		glEnableVertexAttribArray(3);	//Colour
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(BatchedVertex), (const void*)colouroffset);
		glEnableVertexAttribArray(4);	//TexIndex
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(BatchedVertex), (const void*)texindexoffset);
		glEnableVertexAttribArray(5);	//tiling factor
		glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(BatchedVertex), (const void*)tilingfactoroffset);
		glEnableVertexAttribArray(6);	//Entity ID
		//note when attribute is an int need to pass this version of the function. NOTE the I between the b and the p
		glVertexAttribIPointer(6, 1, GL_INT, sizeof(BatchedVertex), (const void*)entityidoffset);
	}
 }