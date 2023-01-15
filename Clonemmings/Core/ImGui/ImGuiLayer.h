#pragma once
#include "Core/Application/Layer.h"


namespace Clonemmings
{
	//Fwd declaration to avoid including header
	class Event;

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		virtual ~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }
		bool IsBlockEvents() { return m_BlockEvents; }

		void SetDarkThemeColours();
	private:
		bool m_BlockEvents = true;
	};
}