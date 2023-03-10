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
#include "Core/Renderer/Renderer.h"
#include "Core/Application/Assert.h"
#include <glad/glad.h>
namespace Clonemmings
{
	void OpenGLErrorCallback(unsigned source, unsigned type, unsigned id, unsigned severity, int length, const char* msg, const void* userparam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH: CRITICAL(msg); return;
		case GL_DEBUG_SEVERITY_MEDIUM: LOGERROR(msg); return;
		case GL_DEBUG_SEVERITY_LOW: WARN(msg); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: TRACE(msg); return;
		}
		ASSERT(false, "Unknown Debug severity level");
	}
	Renderer::Renderer(RendererSetupData setupdata) : m_MaxQuads(setupdata.MaxQuads), m_MaxTextures(setupdata.MaxTextures)
	{
		//setup debug callback!
		INFO("-1-Debug callback setup!");
#ifdef DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLErrorCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, false);
#endif
		//batch renderer
		INFO("-2--Building batch shader");
		m_BatchShader = std::make_unique<Shader>(setupdata.BatchVertexShaderFilename, setupdata.BatchFragmentShaderFilename);
		int32_t* samplers = new int32_t[m_MaxTextures];
		for (uint32_t i = 0; i < m_MaxTextures; i++)
		{
			samplers[i] = i;
		}
		m_BatchShader->Bind();
		m_BatchShader->SetIntArray("u_Textures", samplers, m_MaxTextures);
		delete[] samplers;
		m_BatchVBO = std::make_shared<VertexBufferObject>(m_MaxVertices * sizeof(BatchedVertex), VertexType::Batch);
		m_BatchIBO = std::make_shared<IndexBuffer>((uint32_t) m_MaxIndices);
		m_BatchVAO = std::make_unique<VertexArrayObject>();
		m_BatchVAO->Bind();
		m_BatchVAO->AddVertexBuffer(m_BatchVBO);
		m_BatchVAO->SetIndexBuffer(m_BatchIBO);
		m_Textures.resize(m_MaxTextures);
		m_WhiteTexture = std::make_shared<Texture>(1, 1, glm::vec4(1.0));

		m_QuadVertices[0] = { -0.5f,-0.5f,0.0f,1.0f };
		m_QuadVertices[1] = { 0.5f,-0.5f,0.0f,1.0f };
		m_QuadVertices[2] = { 0.5f,0.5f,0.0f,1.0f };
		m_QuadVertices[3] = { -0.5f,0.5f,0.0f,1.0f };
		m_TexCoords[0] = { 0.0f,0.0f };
		m_TexCoords[1] = { 1.0f,0.0f };
		m_TexCoords[2] = { 1.0f,1.0f };
		m_TexCoords[3] = { 0.0f,1.0f };
			
		//setup other shaders
		INFO("-3---Building texture shader");
		m_TexturedShader = std::make_unique<Shader>(setupdata.TexturedVertexShaderFilename, setupdata.TexturedFragmentShaderFilename);
		INFO("-4----Building coloured shader");
		m_ColouredShader = std::make_unique<Shader>(setupdata.ColouredVertexShaderFilename, setupdata.ColouredFragmentShaderFilename);
		INFO("-5-----Geting initial default settings");
		GetInitalDefaults();
		INFO("Depth test setting: {0}", m_DepthTestOn);
		INFO("Blending setting: {0}", m_BlendingOn);
		INFO("Backface Culling setting: {0}", m_BackFaceCull);
		INFO("Clockwise winding order: {0}", m_ClockwiseWinding);
		SetWindingOrderAntiClockwise();
		SetDepthTest(true);
		SetBackFaceCull(true);
	}
	Renderer::~Renderer()
	{
		if (m_CurrentVertex)
		{
			m_BatchVBO->UnmapDataPointer();
		}
	}
	void Renderer::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void Renderer::SetClearColour(const glm::vec4& colour)
	{
		glClearColor(colour.r, colour.g, colour.b, colour.a);
	}
	void Renderer::SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}
	void Renderer::SetBlending(bool enable)
	{
		if (enable)
		{
			if (m_BlendingOn) return;
			else
			{
				glEnable(GL_BLEND);
				m_BlendingOn = true;
			}
		}
		else
		{
			if (m_BlendingOn)
			{
				glDisable(GL_BLEND);
				m_BlendingOn = false;
			}
		}
	}
	void Renderer::SetDepthTest(bool enable)
	{
		if (enable)
		{
			if (m_DepthTestOn) return;
			else
			{
				glEnable(GL_DEPTH_TEST);
				m_DepthTestOn = true;
			}
		}
		else
		{
			if (m_DepthTestOn)
			{
				glDisable(GL_DEPTH_TEST);
				m_DepthTestOn = false;
			}
		}
	}
	void Renderer::DrawColouredNonIndexed(const VertexArrayObject& vao, const glm::mat4& modeltransform)
	{
		glm::mat4 viewprojection = m_Camera->GetProjection() * glm::inverse(m_CameraTransform);
		m_ColouredShader->Bind();
		m_ColouredShader->SetMat4("u_ModelTransform", modeltransform);
		m_ColouredShader->SetMat4("u_ViewProjection", viewprojection);
		vao.Bind();
		ASSERT(vao.GetVertexBuffer(), "No VBO is added to VAO!");
		vao.GetVertexBuffer()->Draw();
	}
	void Renderer::DrawColouredIndexed(const VertexArrayObject& vao, const glm::mat4& modeltransform)
	{
		glm::mat4 viewprojection = m_Camera->GetProjection() * glm::inverse(m_CameraTransform);
		m_ColouredShader->Bind();
		m_ColouredShader->SetMat4("u_ModelTransform", modeltransform);
		m_ColouredShader->SetMat4("u_ViewProjection", viewprojection);
		vao.Bind();
		ASSERT(vao.GetIndexBuffer(), "No IBO is added to VAO!");
		vao.GetIndexBuffer()->Draw();
	}
	void Renderer::DrawTexturedNonIndexed(const VertexArrayObject& vao, const glm::mat4& modeltransform, std::shared_ptr<Texture> texture)
	{
		glm::mat4 viewprojection = m_Camera->GetProjection() * glm::inverse(m_CameraTransform);
		m_TexturedShader->Bind();
		m_TexturedShader->SetMat4("u_ModelTransform", modeltransform);
		m_TexturedShader->SetMat4("u_ViewProjection", viewprojection);
		ASSERT(texture->IsLoaded(), "Supplied texture is not loaded");
		texture->Bind();
		ASSERT(vao.GetVertexBuffer(), "No VBO is added to VAO!");
		vao.Bind();
		vao.GetVertexBuffer()->Draw();
	}
	void Renderer::DrawTexturedIndexed(const VertexArrayObject& vao, const glm::mat4& modeltransform, std::shared_ptr<Texture> texture)
	{
		glm::mat4 viewprojection = m_Camera->GetProjection() * glm::inverse(m_CameraTransform);
		m_TexturedShader->Bind();
		m_TexturedShader->SetMat4("u_ModelTransform", modeltransform);
		m_TexturedShader->SetMat4("u_ViewProjection", viewprojection);
		ASSERT(texture->IsLoaded(), "Supplied texture is not loaded!");
		texture->Bind();
		ASSERT(vao.GetIndexBuffer(), "No IBO is added to VAO!");
		vao.Bind();
		vao.GetIndexBuffer()->Draw();
	}
	void Renderer::StartBatch()
	{
		m_CurrentVertex = (BatchedVertex*)m_BatchVBO->GetMappedDataPointer();
		ASSERT(m_CurrentVertex, "Error mapping vertex data pointer")
		m_Textures.clear();
		m_Textures.resize(m_MaxTextures);
		m_Textures[0] = m_WhiteTexture;
		m_TextureCount = 1;
		m_QuadCount = 0;
	}
	void Renderer::SubmitToBatch(const glm::mat4& transform, std::shared_ptr<Texture> texture, const glm::vec4& colour, float tilingfactor, int entityID)
	{
		if (m_QuadCount >= m_MaxQuads || m_TextureCount >= m_MaxTextures)
		{
			EndBatch();
			StartBatch();
		}

		float textureindex = -1.0f;
		if (texture)
		{
			for (uint32_t i = 1; i < m_MaxTextures; i++)
			{
				if (m_Textures[i] && *m_Textures[i] == *texture)
				{
					textureindex = (float)i;
					break;
				}
			}
			if (textureindex < 0.9f)
			{
				//new texture
				m_Textures[m_TextureCount] = texture;
				textureindex = (float)m_TextureCount;
				m_TextureCount++;
			}
		}
		else
		{
			textureindex = 0.0f;
		}
		for (uint32_t i = 0; i < 4; i++)
		{
			m_CurrentVertex->Position = transform * m_QuadVertices[i];
			m_CurrentVertex->TilingFactor = tilingfactor;
			m_CurrentVertex->Colour = colour;
			m_CurrentVertex->TexCoords = m_TexCoords[i];
			m_CurrentVertex->TexIndex = textureindex;
			m_CurrentVertex->Normal = { 1.0f,1.0f,1.0f };
			m_CurrentVertex->EntityID = entityID;
			m_CurrentVertex++;
		}
	}
	void Renderer::EndBatch()
	{
		m_BatchVBO->UnmapDataPointer();
		m_CurrentVertex = nullptr;
		for (uint32_t i = 0; i < m_TextureCount; i++)
		{
			m_Textures[i]->Bind(i);
		}
		glm::mat4 viewprojection = m_Camera->GetProjection() * glm::inverse(m_CameraTransform);
		m_BatchShader->Bind();
		m_BatchShader->SetMat4("u_ViewProjection", viewprojection);
		m_BatchVAO->Bind();
		m_BatchVAO->GetIndexBuffer()->Draw();
	}
	void Renderer::SetBackFaceCull(bool enable)
	{
		if (enable && !m_BackFaceCull)
		{
			//not already enabled and requested to be on
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			m_BackFaceCull = true;
		}
		else if (!enable && m_BackFaceCull)
		{
			// turned on need to be switched off
			glDisable(GL_CULL_FACE);
			m_BackFaceCull = false;
		}
	}
	void Renderer::SetWindingOrderClockwise()
	{
		if (m_ClockwiseWinding) return;
		glFrontFace(GL_CW);
		m_ClockwiseWinding = true;
	}
	void Renderer::SetWindingOrderAntiClockwise()
	{
		if (!m_ClockwiseWinding) return;
		glFrontFace(GL_CCW);
		m_ClockwiseWinding = false;
	}
	void Renderer::GetInitalDefaults()
	{
		GLboolean value;
		glGetBooleanv(GL_BLEND, &value );
		m_BlendingOn = value;
		glGetBooleanv(GL_DEPTH_TEST, &value);
		m_DepthTestOn = value;
		glGetBooleanv(GL_CULL_FACE, &value);
		m_BackFaceCull = value;
		glGetBooleanv(GL_FRONT_FACE, &value);
		m_ClockwiseWinding = value;
	}
	void Renderer::DrawBatchedQuad(const glm::vec3& position, const glm::vec2& size, std::shared_ptr<Texture>texture, const glm::vec4& colour, float tilingfactor,
		int entityID)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0), position) * glm::scale(glm::mat4(1.0), glm::vec3(size, 1.0));
		SubmitToBatch(transform, texture, colour, tilingfactor, entityID);
	}
	void Renderer::DrawBatchedRotatedQuad(const glm::vec3& position, const glm::vec2& size, std::shared_ptr<Texture> texture, const glm::vec4& colour, float rotation,
		float tilingfactor, int entityID)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0), position) * glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 0, 1)) 
			* glm::scale(glm::mat4(1.0), glm::vec3(size, 1.0));
		SubmitToBatch(transform, texture, colour, tilingfactor, entityID);
	}
}