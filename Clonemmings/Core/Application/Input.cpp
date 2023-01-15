#include "Core/Application/Input.h"
#include "Core/Application/Application.h"
#include <GLFW/glfw3.h>
namespace Clonemmings
{
	bool Input::IsKeyPressed(KeyCode key)
	{
		GLFWwindow* window = Application::Get().GetWindow().GetHandle();
		int32_t status = glfwGetKey(window, (int32_t)key);
		return status == GLFW_PRESS || status == GLFW_REPEAT;
	}
	bool Input::IsMouseButtonPressed(MouseCode button)
	{
		GLFWwindow* window = Application::Get().GetWindow().GetHandle();
		int32_t status = glfwGetMouseButton(window, (int32_t)button);
		return status == GLFW_PRESS;
	}
	glm::vec2 Input::GetMousePosition()
	{
		GLFWwindow* window = Application::Get().GetWindow().GetHandle();
		double x;
		double y;
		glfwGetCursorPos(window, &x, &y);
		return glm::vec2((float)x, (float)y);
	}
	float Input::GetMouseX()
	{
		return GetMousePosition().x;
	}
	float Input::GetMouseY()
	{
		return GetMousePosition().y;
	}
}