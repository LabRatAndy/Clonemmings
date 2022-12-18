#pragma once
#include <functional>

namespace Clonemmings
{
	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WinodowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};
	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = 1 << 0,
		EventCategoryInput = 1 << 1,
		EventCategoryKeyboard = 1 << 2,
		EventCategoryMouse = 1 << 3,
		EventCategoryMouseButton = 1 << 4,
	};
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() {return EventType::type;}\
	virtual EventType GetEventType() const override {return GetStaticType(); }\
	virtual const char* GetName() const override {return #type;}
	
#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override {return return category;}
	
	class Event
	{
	public:
		bool m_Handled = false;

		virtual ~Event() = default;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const = 0;
		bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& e) : m_Event(e)
		{

		}

		template <typename T, typename F>
		bool Dispatch(const F& func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled |= func((T&)(m_Event));
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}