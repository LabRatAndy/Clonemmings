#pragma once
#include "Core/Events/Event.h"
#include "Core/Application/KeyMouseCodes.h"

#include <string>
#include <sstream>
namespace Clonemmings
{
	class KeyEvent : public Event
	{
	public:
		KeyCode GetKeyCode() const { return m_KeyCode; }
		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	protected:
		KeyEvent(const KeyCode keycode) :m_KeyCode(keycode)
		{

		}

		KeyCode m_KeyCode;
	};
	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(const KeyCode keycode, uint16_t repeatcount) : KeyEvent(keycode), m_RepeatCount(repeatcount)
		{

		}
		uint16_t GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Key pressed event: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}
		EVENT_CLASS_TYPE(KeyPressed)
	private: 
		uint16_t m_RepeatCount;
	};
	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(const KeyCode keycode) : KeyEvent(keycode)
		{

		}
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Key released event: " << m_KeyCode;
			return ss.str();
		}
		EVENT_CLASS_TYPE(KeyReleased)
	};
	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(const KeyCode keycode) : KeyEvent(keycode)
		{

		}
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "key typed event: " << m_KeyCode;
			return ss.str();
		}
		EVENT_CLASS_TYPE(KeyTyped)
	};
}