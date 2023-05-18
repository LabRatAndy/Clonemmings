#pragma once
#include "Core/Application/Layer.h"
#include "Core/Events/MouseEvent.h"
#include "Core/Scene/Scene.h"
#include "Core/Scene/Entity.h"
#include "Core/Renderer/VertexArray.h"
#include "Core/Renderer/Texture.h"
#include "Core/Renderer/Framebuffer.h"
#include "Core/Renderer/FramebufferSpecification.h"
#include "Game/ControlPanel.h"
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
		virtual void OnEvent(Event& e) override;

		void SetScene(std::shared_ptr<Scene> scene);
		void LoadScene(const std::string& filename);
		Entity GetSelectedEntity() { return m_CurrentSelectedEntity; }

	private:
		std::shared_ptr<Scene> m_ActiveScene = nullptr;
		std::shared_ptr<Scene> m_ResetScene = nullptr;
		std::shared_ptr<VertexArrayObject> m_VAO = nullptr;
		std::shared_ptr<VertexBufferObject> m_VBO = nullptr;
		std::shared_ptr<IndexBuffer> m_IBO = nullptr;
		std::shared_ptr<Texture> m_Texture = nullptr;

		std::shared_ptr<FrameBuffer> m_Framebuffer = nullptr;
		glm::vec2 m_ViewportSize;
		glm::vec2 m_ViewportBounds[2];
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		Entity m_HoveredEntity = Entity();
		Entity m_CurrentSelectedEntity = Entity();
		bool m_LabelsLoaded = false;

		ControlPanel m_ControlPanel;

		//only really for testing and initial saving of demo scenes etc
		void SaveScene(const std::string& filename);

		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
	};
}