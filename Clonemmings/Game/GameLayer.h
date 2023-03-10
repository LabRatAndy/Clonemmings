#pragma once
#include "Core/Application/Layer.h"
#include "Core/Scene/Scene.h"
#include "Core/Renderer/VertexArray.h"
#include "Core/Renderer/Texture.h"
#include "Core/Renderer/Framebuffer.h"
#include "Core/Renderer/FramebufferSpecification.h"
#include <glm/glm.hpp>

namespace Clonemmings
{
	class GameLayer :public Layer
	{
	public:
		GameLayer(const std::string& name);
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(TimeStep ts) override;
		virtual void OnImGuiRender() override;

		void SetScene(std::shared_ptr<Scene> scene);
		void LoadScene(const std::string& filename);

	private:
		std::shared_ptr<Scene> m_ActiveScene = nullptr;
		std::shared_ptr<Scene> m_ResetScene = nullptr;
		std::shared_ptr<VertexArrayObject> m_VAO = nullptr;
		std::shared_ptr<VertexBufferObject> m_VBO = nullptr;
		std::shared_ptr<IndexBuffer> m_IBO = nullptr;
		std::shared_ptr<Texture> m_Texture = nullptr;

		std::shared_ptr<FrameBuffer> m_Framebuffer = nullptr;
		glm::vec2 m_ViewportSize;

		//only really for testing and initial saving of demo scenes etc
		void SaveScene(const std::string& filename);
	};
}