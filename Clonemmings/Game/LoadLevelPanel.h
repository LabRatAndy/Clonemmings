#pragma once
#include "Core/Application/UUID.h"
#include "Core/Assets/AssetMetaData.h"
#include <vector>
namespace Clonemmings
{
	enum  class LevelLoad
	{
		Invalid = 0,
		Cancelled,
		Selected
	};
	class ControlPanel;
	class LoadLevelPanel
	{
	public:
		LoadLevelPanel() = default;
		LoadLevelPanel(ControlPanel* controlpanel);
		void SetControlPanel(ControlPanel* controlpanel);
		void OnImGuiRender();
		UUID GetSelectedLevelHandle() const;

	private:
		UUID m_SelectedLevel = 0;
		std::vector<UUID> m_UUIDList;
		std::vector<std::string> m_LevelName;
		ControlPanel* m_ControlPanel = nullptr;
		size_t m_LevelSelected = -1;

		void PopulateLevelLists();

	};
}