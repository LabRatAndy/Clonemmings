#pragma once
#include "core/Events/Event.h"
#include "Core/Application/KeyMouseCodes.h"

#include <sstream>
#include <string>

namespace Clonemmings
{
	class MouseMovedEvent :public Event
	{
	public:
		MouseMovedEvent(const float x, const float y) : m_MouseX(x), m_MouseY(y)
		{

		}
		float GetX() const { return m_MouseX; }
		float GetY() const { return m_MouseY; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Mouse moved event: " << m_MouseX << " , " << m_MouseY;
			return ss.str();
		}
		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse)
		EVENT_CLASS_TYPE(MouseMoved)
	private:
		float m_MouseX;
		float m_MouseY;
	};
	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(const float xoffset, const float yoffset) :m_Xoffset(xoffset), m_Yoffset(yoffset)
		{

		}
		float GetXOffset() const { return m_Xoffset; }
		float GetYOffset() const { return m_Yoffset; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Mouse scrolled event: " << m_Xoffset << " , " << m_Yoffset;
			return ss.str();
		}
		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse)
		EVENT_CLASS_TYPE(MouseScrolled)
	private:
		float m_Xoffset;
		float m_Yoffset;

	};
	class MouseButtonEvent : public Event
	{
	public:
		MouseCode GetMouseButton() const { return m_Button; }
		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse | EventCategoryMouseButton)
	protected:
		MouseCode m_Button;

		MouseButtonEvent(const MouseCode button) : m_Button(button)
		{

		}
	};
	class  MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(const MouseCode button) :MouseButtonEvent(button)
		{

		}
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Mouse button pressed event: " << m_Button;
			return ss.str();
		}
		EVENT_CLASS_TYPE(MouseButtonPressed)
	};
	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(const MouseCode button) : MouseButtonEvent(button)
		{

		}
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Mouse button released event: " << m_Button;
			return ss.str();
		}
		EVENT_CLASS_TYPE(MouseButtonReleased)
	};

}