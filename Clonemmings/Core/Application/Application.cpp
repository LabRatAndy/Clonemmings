#include "Core/Application/Application.h"
#include "Core/Log/Log.h"
#include "Game/GameLayer.h"
#include "Core/Scripting/ScriptEngine.h"

#include <GLFW/glfw3.h>
namespace Clonemmings
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name, const CommandLineArguements& args) : m_CommandLineArgs(args)
	{
		ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		INFO("Setting up the application window");
		m_Window = std::make_unique<Window>(name, 1100, 720);
		m_Window->SetEventCallbackFunction([this](auto&&...args)->decltype(auto) {return this->Application::OnEvent(std::forward<decltype(args)>(args)...); });
		INFO("Window set up complete!");
		m_Layers = new LayerStack();
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
		INFO("Setting up Renderer");
		RendererSetupData renererdata;
		renererdata.BatchFragmentShaderFilename = "Assets/Shaders/Batch.frag";
		renererdata.BatchVertexShaderFilename = "Assets/Shaders/Batch.vert";
		renererdata.ColouredFragmentShaderFilename = "Assets/Shaders/Coloured.frag";
		renererdata.ColouredVertexShaderFilename = "Assets/Shaders/Coloured.vert";
		renererdata.TexturedVertexShaderFilename = "Assets/Shaders/Textured.vert";
		renererdata.TexturedFragmentShaderFilename = "Assets/Shaders/Textured.frag";
		renererdata.MaxQuads = 1000;
		renererdata.MaxTextures = 32;
		m_Renderer = std::make_unique<Renderer>(renererdata);
		// very temp get camera and transform from ECS system! but not here!! should be in layer!!
		m_Camera = new SceneCamera();
		m_Camera->SetOrthographic(m_Window->GetHeight(), -1.0f, 1.0f);
		m_Camera->SetViewportSize(m_Window->GetWidth(), m_Window->GetHeight());
		glm::mat4 cameratransform = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, -0.5));
		m_Renderer->SetCamera(m_Camera, cameratransform);
		m_Renderer->SetClearColour(glm::vec4(0.0, 0.0, 0.0, 1.0));
		m_Renderer->SetViewPort(0, 0, m_Window->GetWidth(), m_Window->GetHeight());
		INFO("Renderer set up complete");
		INFO("Start up the script engine");
		ScriptEngine::Init();
		INFO("create Game Layer");
		GameLayer* gamelayer = new GameLayer("GameLayer");
		std::shared_ptr<Scene> scene = std::make_shared<Scene>();
		gamelayer->SetScene(scene);
		PushLayer(gamelayer);
	}

	Application::~Application()
	{
		delete m_Layers;
		delete m_Camera;
	}
	void Application::PushLayer(Layer* layer)
	{
		m_Layers->PushLayer(layer);
		layer->OnAttach();
	}
	void Application::PushOverlay(Layer* overlay)
	{
		m_Layers->PushOverlay(overlay);
		overlay->OnAttach();
	}
	void Application::Close()
	{
		ScriptEngine::Shutdown();
		m_Running = false;
	}
	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>([this](auto&&...args)->decltype(auto) {return this->Application::OnWindowClose(std::forward<decltype(args)>(args)...); });
		dispatcher.Dispatch<WindowResizeEvent>([this](auto&&...args)->decltype(auto) {return this->Application::OnWindowResize(std::forward<decltype(args)>(args)...); });
		for (auto it = m_Layers->rbegin(); it != m_Layers->rend(); ++it)
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
		m_Renderer->SetViewPort(0, 0, e.GetWidth(), e.GetHeight());


		return false;
	}
	void Application::Run()
	{
		while (m_Running)
		{
			float time = (float)glfwGetTime();
			TimeStep ts = time - m_LastFrameTime;
			m_LastFrameTime = time;
			ExecuteMainThreadQueue();
			if (!m_Minimised)
			{
				m_Renderer->Clear();
				for (Layer* layer : *m_Layers)
				{
					layer->OnUpdate(ts);
				}
				m_ImGuiLayer->Begin();
				for (Layer* layer : *m_Layers)
				{
					layer->OnImGuiRender();
				}
				m_ImGuiLayer->End();
			}
			m_Window->OnUpdate();
		}
	}
	void Application::SubmitToMainThread(const std::function<void()>& function)
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);
		m_MainThreadQueue.emplace_back(function);
	}
	void Application::ExecuteMainThreadQueue()
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);
		for (auto& func : m_MainThreadQueue)
		{
			func();
		}
		m_MainThreadQueue.clear();
	}
}