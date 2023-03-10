#pragma once

#include "Core/Application/KeyMouseCodes.h"
#include <glm/glm.hpp>

namespace Clonemmings
{
	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);
		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}