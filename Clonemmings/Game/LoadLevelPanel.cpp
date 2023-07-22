#include "Game/LoadLevelPanel.h"
#include "Core/Application/Application.h"
#include "Core/Assets/AssetManager.h"
#include "Game/ControlPanel.h"

#include <imgui.h>
namespace Clonemmings
{
	LoadLevelPanel::LoadLevelPanel(ControlPanel* controlpanel) : m_ControlPanel(controlpanel)
	{
		
	}
	void LoadLevelPanel::SetControlPanel(ControlPanel* controlpanel)
	{
		m_ControlPanel = controlpanel;
	}
	UUID LoadLevelPanel::GetSelectedLevelHandle() const 
	{
		return m_SelectedLevel;
	}
	void LoadLevelPanel::OnImGuiRender()
	{
		if (m_LevelSelected == -1)
		{
			PopulateLevelLists();
		}
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize;
		ImGui::Begin("Open Level", (bool*)0, flags);
		ImGui::Text("Select level to open from list");
		ImGui::Separator();
		const char* currentlevelstr = m_LevelName[m_LevelSelected].c_str();
		if (ImGui::BeginCombo("Level", currentlevelstr))
		{
			for (size_t i = 0; i < m_UUIDList.size() ; i++)
			{
				bool isselected = i == m_LevelSelected;
				if (ImGui::Selectable(m_LevelName[i].c_str(), isselected))
				{
					currentlevelstr = m_LevelName[i].c_str();
					m_SelectedLevel = m_UUIDList[i];
					m_LevelSelected = i;
				}
				if(isselected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Separator();
		if (ImGui::Button("Load level"))
		{
			m_ControlPanel->m_LoadLevelPanelResult = LevelLoad::Selected;
			m_ControlPanel->m_ShowLoadLevelPanel = false;
			m_LevelSelected = -1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Close"))
		{
			m_ControlPanel->m_LoadLevelPanelResult = LevelLoad::Cancelled;
			m_ControlPanel->m_ShowLoadLevelPanel = false;
			m_LevelSelected = -1;
		}
		ImGui::End();
	}
	void LoadLevelPanel::PopulateLevelLists()
	{
		const std::map<UUID,AssetMetaData>& assetregistry = Application::Get().GetAssetManager().GetAssetRegistry();
		m_UUIDList.clear();
		m_LevelName.clear();
		if (assetregistry.empty())
		{
			//asset registry is empty! 
			ASSERT(false, "Asset registry is empty!");
			m_UUIDList.resize(1);
			m_LevelName.resize(1);
			m_UUIDList[0] = 0;
			m_LevelName[0] = "No levels available";
			m_LevelSelected = 0;
			return;
		}
		size_t levelcount = assetregistry.size();
		m_UUIDList.resize(levelcount);
		m_LevelName.resize(levelcount + 1);;
		size_t level = 0;
		for (auto it = assetregistry.begin(); it != assetregistry.end(); it++)
		{
			if (it->second.Type != AssetType::Level)
				continue;
			m_UUIDList[level] = it->first;
			std::string levelname = it->second.FilePath.filename().generic_string();
			m_LevelName[level] = levelname;
			level++;
		}
		m_LevelName[level] = "No level selected";
		m_LevelSelected = level;
	}
}