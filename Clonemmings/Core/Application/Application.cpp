#include "Core/Application/Application.h"
#include "Core/Log/Log.h"

#include <GLFW/glfw3.h>
namespace Clonemmings
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name, const CommandLineArguements& args) : m_CommandLineArgs(args)
	{
		ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		m_Window = std::make_unique<Window>(name, 640, 480);
		m_Window->SetEventCallbackFunction([this](auto&&...args)->decltype(auto) {return this->Application::OnEvent(std::forward<decltype(args)>(args)...); });
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{

	}
	void Application::PushLayer(Layer* layer)
	{
		m_Layers.PushLayer(layer);
		layer->OnAttach();
	}
	void Application::PushOverlay(Layer* overlay)
	{
		m_Layers.PushOverlay(overlay);
		overlay->OnAttach();
	}
	void Application::Close()
	{
		m_Running = false;
	}
	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>([this](auto&&...args)->decltype(auto) {return this->Application::OnWindowClose(std::forward<decltype(args)>(args)...); });
		dispatcher.Dispatch<WindowResizeEvent>([this](auto&&...args)->decltype(auto) {return this->Application::OnWindowResize(std::forward<decltype(args)>(args)...); });
		for (auto it = m_Layers.rbegin(); it != m_Layers.rend(); ++it)
		{
			if (e.m_Handled) break;
			(*it)->OnEvent(e);
		}
	}
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimised = true;
			return false;
		}
		m_Minimised = false;
		//call renderer resize function, here!

		return false;
	}
	void Application::Run()
	{
		while (m_Running)
		{
			float time = (float)glfwGetTime();
			TimeStep ts = time - m_LastFrameTime;
			m_LastFrameTime = time;
			if (!m_Minimised)
			{
				for (Layer* layer : m_Layers)
				{
					layer->OnUpdate(ts);
				}
				m_ImGuiLayer->Begin();
				for (Layer* layer : m_Layers)
				{
					layer->OnImGuiRender();
				}
				m_ImGuiLayer->End();
			}
			m_Window->OnUpdate();
		}
	}
}