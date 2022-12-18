#pragma once
#include "Core/Application/LayerStack.h"
#include "Core/Application/Window.h"
#include "Core/Events/Event.h"
#include "Core/Events/ApplicationEvent.h"
#include "Core/Application/Assert.h"


namespace Clonemmings
{
	class ImGuiLayer;
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
		Window& GetWindow() { return m_Window; }
		CommandLineArguements GetCommandLineArguments() { return m_CommandLineArgs; }
		static Application& Get() { return *s_Instance; }
		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

	private:
		LayerStack m_Layers;
		Window m_Window;
		bool m_Running = true;
		bool m_Minimised = false;
		float m_LastFrameTime = 0.0f;
		CommandLineArguements m_CommandLineArgs;
		ImGuiLayer* m_ImGuiLayer;
		static Application* s_Instance;


		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	};
}