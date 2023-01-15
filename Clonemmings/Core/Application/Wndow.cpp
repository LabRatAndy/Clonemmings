#include "Core/Application/Window.h"
#include "Core/Log/Log.h"
#include "Core/Application/Assert.h"
#include "Core/Events/ApplicationEvent.h"
#include "Core/Events/KeyEvent.h"
#include "Core/Events/MouseEvent.h"

#include <glad.h>
namespace Clonemmings
{
	static uint8_t s_GLFWwindowCount = 0;
	
	static void GLFWErrorCallBack(int error, const char* description)
	{
		LOGERROR("GLFW error({0}): {1}", error, description);
	}

	Window::Window(const std::string& title, uint32_t width, uint32_t height) : m_Title(title), m_Width(width), m_Height(height)
	{
		INFO("Creating window {0} ({1},{2})", m_Title, m_Width, m_Height);
		if (s_GLFWwindowCount == 0)
		{
			int sucess = glfwInit();
			ASSERT(sucess, "Failed to initialise GLFW!");
			glfwSetErrorCallback(GLFWErrorCallBack);
		}

#ifdef DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

		m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
		ASSERT(m_Window, "Failed to create glfw window");
		s_GLFWwindowCount++;
		glfwMakeContextCurrent(m_Window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ASSERT(status, "Failed to initialise GLAD");
		//opengl info
		INFO("OpenGL info:");
		INFO(" Vendor: {0}", (void*)glGetString(GL_VENDOR));
		INFO(" Renderer: {0}", (void*)glGetString(GL_RENDERER));
		INFO(" Version: {0}", (void*)glGetString(GL_VERSION));
		ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "OpenGL version must be at least 4.5");
		EnableVSync();
		m_Data.Title = &m_Title;
		m_Data.Height = &m_Height;
		m_Data.Width = &m_Width;
		m_Data.VSync = &m_VsyncEnabled;
		glfwSetWindowUserPointer(m_Window, &m_Data);

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				DataPointers& data = *(DataPointers*)glfwGetWindowUserPointer(window);
				*data.Width = width;
				*data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallBack(event);

			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				DataPointers& data = *(DataPointers*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallBack(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				DataPointers& data = *(DataPointers*)glfwGetWindowUserPointer(window);
				switch (action)
				{
				case GLFW_PRESS:
					{
						KeyPressedEvent event(key, 0);
						data.EventCallBack(event);
						break;
					}
				case GLFW_RELEASE:
					{
						KeyReleasedEvent event(key);
						data.EventCallBack(event);
						break;
					}
				case GLFW_REPEAT:
					{
						KeyPressedEvent event(key, 1);
						data.EventCallBack(event);
						break;
					}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				DataPointers& data = *(DataPointers*)glfwGetWindowUserPointer(window);
				KeyTypedEvent event(keycode);
				data.EventCallBack(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				DataPointers& data = *(DataPointers*)glfwGetWindowUserPointer(window);
				switch (action)
				{
				case GLFW_PRESS:
					{
						MouseButtonPressedEvent event(button);
						data.EventCallBack(event);
						break;
					}
				case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent event(button);
						data.EventCallBack(event);
						break;
					}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffest)
			{
				DataPointers& data = *(DataPointers*)glfwGetWindowUserPointer(window);
				MouseScrolledEvent event((float)xoffset, (float)yoffest);
				data.EventCallBack(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
			{
				DataPointers& data = *(DataPointers*)glfwGetWindowUserPointer(window);
				MouseMovedEvent event((float)xpos, (float)ypos);
				data.EventCallBack(event);
			});
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_Window);
		s_GLFWwindowCount--;
		if (s_GLFWwindowCount == 0)
		{
			glfwTerminate();
		}
	}

	void Window::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void Window::EnableVSync()
	{
		if (m_VsyncEnabled) return;
		glfwSwapInterval(1);
		m_VsyncEnabled = true;
	}
	void Window::DisableVSync()
	{
		if (!m_VsyncEnabled) return;
		glfwSwapInterval(0);
		m_VsyncEnabled = false;
	}
}