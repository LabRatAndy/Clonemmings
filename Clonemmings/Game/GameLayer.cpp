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
		ColouredVertex* vertices = new ColouredVertex[4];
		vertices[0].Position = { -0.5,-0.5,0.0 };
		vertices[1].Position = { 0.5,-0.5,0.0 };
		vertices[2].Position = { 0.5,0.5,0.0 };
		vertices[3].Position = { -0.5,0.5,0.0 };
		vertices[0].Normal = { 0.0,0.0,0.0 };
		vertices[1].Normal = { 0.0,0.0,0.0 };
		vertices[2].Normal = { 0.0,0.0,0.0 };
		vertices[3].Normal = { 0.0,0.0,0.0 };
		vertices[0].Colour = { 1.0,1.0,1.0,1.0 };
		vertices[1].Colour = { 1.0,1.0,1.0,1.0 }; 
		vertices[2].Colour = { 1.0,1.0,1.0,1.0 };
		vertices[3].Colour = { 1.0,1.0,1.0,1.0 };
		uint32_t* indices = new uint32_t[6];
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 2;
		indices[4] = 3;
		indices[5] = 0;
		m_VAO = std::make_shared<VertexArrayObject>();
		m_VAO->Bind();
		m_VBO = std::make_shared<VertexBufferObject>(vertices, 4 * sizeof(ColouredVertex), VertexType::Coloured);
		m_IBO = std::make_shared<IndexBuffer>(indices, 6);
		m_VAO->AddVertexBuffer(m_VBO);
		m_VAO->SetIndexBuffer(m_IBO);
		m_VAO->UnBind();
		delete[] vertices;
		delete[] indices;
	}
	void GameLayer::OnDetach()
	{

	}
	void GameLayer::OnUpdate(TimeStep ts)
	{
		glm::mat4 modelLHS = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0));
		Application::Get().GetRenderer().DrawColouredIndexed(*m_VAO, modelLHS);	
	}
}