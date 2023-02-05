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
		vertices[0].Colour = { 0.0,1.0,0.0,1.0 };
		vertices[1].Colour = { 0.0,1.0,0.0,1.0 };
		vertices[2].Colour = { 0.0,1.0,0.0,1.0 };
		vertices[3].Colour = { 0.0,1.0,0.0,1.0 };
		uint32_t* indices = new uint32_t[4];
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 3;
		m_VAO = std::make_shared<VertexArrayObject>();
		m_VAO->Bind();
		m_VBO = std::make_shared<VertexBufferObject>(vertices, 4 * sizeof(ColouredVertex), VertexType::Coloured);
		m_IBO = std::make_shared<IndexBuffer>(indices, 4);
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
		glm::mat4 viewproj = Application::Get().GetRenderer().GetCamera()->GetProjection();
		TRACE("model transform: {0}", modelLHS);
		TRACE("View Projection matrix: {0}", viewproj);
		{
			m_VBO->Bind();
			ColouredVertex* vertices = (ColouredVertex*)m_VBO->GetMappedDataPointer();
			uint32_t vert = 0;
			while (vert < 4)
			{

				glm::vec4 result = viewproj * modelLHS * glm::vec4((vertices + vert)->Position, 1.0);
				TRACE("Vertex: {0}, Position: {1}, Colour: {2}, Transformed Position: {3}", vert, (vertices + vert)->Position, (vertices + vert)->Colour, result);
				vert++;
			}
			m_VBO->UnmapDataPointer();
		}
		Application::Get().GetRenderer().DrawColouredIndexed(*m_VAO, modelLHS);	
	}
}