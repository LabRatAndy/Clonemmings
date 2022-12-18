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
		void OnAttach() {}
		void OnDetach() {}
		void OnUpdate(TimeStep ts) {}
		void OnImGuiRender() {}
		void OnEvent(Event& e) {}

		const std::string& GetName() { return m_Name; }
	protected:
		std::string m_Name;
	};
}