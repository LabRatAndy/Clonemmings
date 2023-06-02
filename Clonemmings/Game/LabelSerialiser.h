#pragma once
#include "Game/ControlPanel.h"
namespace Clonemmings
{
	class LabelSerialiser
	{
	public:
		LabelSerialiser(ControlPanel* controlpanel);
		void Serialise(const std::string& filename);
		bool Deserialise(const std::string& filename);

	private:
		ControlPanel* m_ControlPanelPtr = nullptr;
	};
}
