#pragma once
#include "Core/Application/LayerStack.h"
#include "Core/Application/Window.h"
#include "Core/Events/Event.h"
#include "Core/Events/ApplicationEvent.h"
#include "Core/Application/Assert.h"
#include "Core/ImGui/ImGuiLayer.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Assets/AssetManager.h"

#include <memory>

namespace Clonemmings
{
	struct  CommandLineArguements
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index)
		{
			ASSERT(index < Count);
			return Args[index];
		}
	};
	class Application
	{
	public:
		Application(const std::string& name = "Application", const CommandLineArguements& args = CommandLineArguements());
		~Application();
		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void Run();
		void Close();
		Window& GetWindow() { return *m_Window; }
		CommandLineArguements GetCommandLineArguments() { return m_CommandLineArgs; }
		static Application& Get() { return *s_Instance; }
		static AssetManager& GetAssetManager() { return *s_AssetManager; }
		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
		Renderer& GetRenderer() { return *m_Renderer; }
		void SubmitToMainThread(const std::function<void()>& function);

	private:
		LayerStack* m_Layers =  nullptr;
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		bool m_Minimised = false;
		float m_LastFrameTime = 0.0f;
		CommandLineArguements m_CommandLineArgs;
		ImGuiLayer* m_ImGuiLayer;
		static Application* s_Instance;
		static AssetManager* s_AssetManager;
		std::unique_ptr<Renderer> m_Renderer = nullptr;
		SceneCamera* m_Camera = nullptr;
		std::vector<std::function<void()>> m_MainThreadQueue;
		std::mutex m_MainThreadQueueMutex;

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		void ExecuteMainThreadQueue();
	};
}