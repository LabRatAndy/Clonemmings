#pragma once

#include "Core/Events/Event.h"
#include <GLFW/glfw3.h>

#include <string>
#include <functional>
namespace Clonemmings
{
	class Window
	{
	public:
		Window(const std::string& title, uint32_t width, uint32_t height);
		~Window();
		void OnUpdate();
		void SetEventCallbackFunction(const std::function<void(Event&)>& callback) { m_Data.EventCallBack = callback; }
		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		const std::string& GetTitle() const { return m_Title; }
		bool GetVSyncEnabled() const { return m_VsyncEnabled; }
		void EnableVSync();
		void DisableVSync();
		GLFWwindow* GetHandle() const { return m_Window; }
	private:
		struct DataPointers
		{
			std::string* Title = nullptr;
			uint32_t* Width = nullptr;
			uint32_t* Height= nullptr;
			bool* VSync = nullptr;
			std::function<void(Event&)> EventCallBack;
		};
		GLFWwindow* m_Window = nullptr;
		uint32_t m_Width;
		uint32_t m_Height;
		std::string m_Title;
		bool m_VsyncEnabled = false;
		DataPointers m_Data;
	};
}