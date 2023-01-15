#pragma once
#include "Core/Events/Event.h"
#include "Core/Application/TimeStep.h"
namespace Clonemmings
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer"): m_Name(name){}
		virtual ~Layer() = default;
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(TimeStep ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& e) {}

		const std::string& GetName() { return m_Name; }
	protected:
		std::string m_Name;
	};
}