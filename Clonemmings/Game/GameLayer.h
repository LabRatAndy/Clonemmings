#pragma once
#include "Core/Application/Layer.h"
#include "Core/Scene/Scene.h"
#include "Core/Renderer/VertexArray.h"
#include "Core/Renderer/Texture.h"

namespace Clonemmings
{
	class GameLayer :public Layer
	{
	public:
		GameLayer(const std::string& name);
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(TimeStep ts) override;

		void SetScene(std::shared_ptr<Scene> scene);

	private:
		std::shared_ptr<Scene> m_ActiveScene = nullptr;
		std::shared_ptr<VertexArrayObject> m_VAO = nullptr;
		std::shared_ptr<VertexBufferObject> m_VBO = nullptr;
		std::shared_ptr<IndexBuffer> m_IBO = nullptr;
		std::shared_ptr<Texture> m_Texture = nullptr;
	};
}