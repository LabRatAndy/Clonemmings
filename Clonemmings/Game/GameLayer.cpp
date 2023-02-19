#include "Game/GameLayer.h"
#include "Core/Application/Application.h"
#include "Core/Renderer/Renderer.h"

namespace Clonemmings
{
	GameLayer::GameLayer(const std::string& name) : Layer(name)
	{

	}
	void GameLayer::OnAttach()
	{
#if 0
		ColouredVertex* vertices = new ColouredVertex[6];
		vertices[0].Position = { -0.5,-0.5,0.0 };
		vertices[1].Position = { 0.5,-0.5,0.0 };
		vertices[2].Position = { 0.5,0.5,0.0 };
		vertices[3].Position = { 0.5,0.5,0.0 };
		vertices[4].Position = { -0.5,0.5,0.0 };
		vertices[5].Position = { -0.5,-0.5,0.0 };
		vertices[0].Normal = { 0.0,0.0,0.0 };
		vertices[1].Normal = { 0.0,0.0,0.0 };
		vertices[2].Normal = { 0.0,0.0,0.0 };
		vertices[3].Normal = { 0.0,0.0,0.0 };
		vertices[4].Normal = { 0.0,0.0,0.0 };
		vertices[5].Normal = { 0.0,0.0,0.0 };
		vertices[0].Colour = { 1.0,1.0,1.0,1.0 };
		vertices[1].Colour = { 1.0,1.0,1.0,1.0 }; 
		vertices[2].Colour = { 1.0,1.0,1.0,1.0 };
		vertices[3].Colour = { 1.0,1.0,1.0,1.0 };
		vertices[4].Colour = { 1.0,1.0,1.0,1.0 };
		vertices[5].Colour = { 1.0,1.0,1.0,1.0 };
		uint32_t* indices = new uint32_t[6];
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 2;
		indices[4] = 3;
		indices[5] = 0;
		m_VAO = std::make_shared<VertexArrayObject>();
		m_VAO->Bind();
		m_VBO = std::make_shared<VertexBufferObject>(vertices, 6 * sizeof(ColouredVertex), VertexType::Coloured);
		m_IBO = std::make_shared<IndexBuffer>(indices, 6);
		m_VAO->AddVertexBuffer(m_VBO);
		m_VAO->SetIndexBuffer(m_IBO);
		m_VAO->UnBind();
		delete[] vertices;
		delete[] indices;
#elif 0
		TexturedVertex* vertices = new TexturedVertex[4];
		vertices[0].Position = { -0.5,-0.5,0.0 };
		vertices[1].Position = { 0.5,-0.5,0.0 };
		vertices[2].Position = { 0.5,0.5,0.0 };
		vertices[3].Position = { -0.5,0.5,0.0 };
		vertices[0].Normal = { 0.0,0.0,0.0 };
		vertices[1].Normal = { 0.0,0.0,0.0 };
		vertices[2].Normal = { 0.0,0.0,0.0 };
		vertices[3].Normal = { 0.0,0.0,0.0 };
		vertices[0].TexCoords = { 0.0,0.0 };
		vertices[1].TexCoords = { 1.0,0.0 };
		vertices[2].TexCoords = { 1.0,1.0 };
		vertices[3].TexCoords = { 0.0,1.0 };
		uint32_t* indices = new uint32_t[6];
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 2;
		indices[4] = 3;
		indices[5] = 0;
		m_VAO = std::make_shared<VertexArrayObject>();
		m_VAO->Bind();
		m_VBO = std::make_shared<VertexBufferObject>(vertices, 4 * sizeof(TexturedVertex), VertexType::Textured);
		m_IBO = std::make_shared<IndexBuffer>(indices, 6);
		m_VAO->AddVertexBuffer(m_VBO);
		m_VAO->SetIndexBuffer(m_IBO);
		m_VAO->UnBind();
		delete[] vertices;
		delete[] indices;
		glm::vec4 colour;
		colour.r = 0.0;
		colour.g = 1.0;
		colour.b = 0.0;
		colour.a = 1.0;
		m_Texture = std::make_shared<Texture>(4, 4, glm::vec4(0.0, 1.0, 0.0, 1.0));
#else
		m_Texture = std::make_shared<Texture>("Assets/Textures/test.png");
#endif
	}
	void GameLayer::OnDetach()
	{

	}
	void GameLayer::OnUpdate(TimeStep ts)
	{
		Application::Get().GetRenderer().StartBatch();
		glm::mat4 modelLHS = glm::translate(glm::mat4(1.0), glm::vec3(-5.0, 10.0, 0.0)) * glm::scale(glm::mat4(1.0), glm::vec3(0.01, 10.0, 1.0));
		Application::Get().GetRenderer().SubmitToBatch(modelLHS, nullptr, glm::vec4(0.0, 1.0, 1.0, 1.0), 1.0);
		modelLHS = glm::translate(glm::mat4(1.0), glm::vec3(5.0, 0.0, 0.0));
		Application::Get().GetRenderer().SubmitToBatch(modelLHS, m_Texture, glm::vec4(1.0), 1.0);
		Application::Get().GetRenderer().EndBatch();
	}
}