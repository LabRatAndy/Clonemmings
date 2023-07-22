#include "Game/ControlPanel.h"
#include "Game/DefaultLabels.h"
#include "Game/GameLayer.h"
#include "Game/LabelSerialiser.h"
#include "Core/Application/Application.h"
#include "Core/Application/FileDialog.h"
#include <imgui.h>


namespace Clonemmings
{
	ControlPanel::ControlPanel(std::shared_ptr<Scene>& scene) : m_Context(scene)
	{

	}

	void ControlPanel::SetContext(std::shared_ptr<Scene>& context)
	{
		m_Context = context;
	}
	void ControlPanel::SetSelectedEntity(Entity entity)
	{
		if (entity && !entity.HasComponent<ClonemmingComponent>());
		{
				m_SelectedEntity = {};
				return;
		}
		m_SelectedEntity = entity;

	}
	void ControlPanel::OnImGuiRender()
	{
		GetLargestButtonSize();
		ImGui::Begin(GetLabelText(CONTROLPANELLABEL));
		ImGui::Text(GetLabelText(LEVELCONTROLSLABEL));
		if (ImGui::Button(GetStartStopButtonText(), m_ButtonSize))
		{
			if (!m_Context->IsRunning())
				m_Context->StartScene();
			else
				m_Context->StopScene();
		}
		ImGui::SameLine();
		if (ImGui::Button(GetPauseUnpauseButtonText(), m_ButtonSize))
		{
			//scene is running can pause if not already paused and unpause if already paused
			if (m_Context->IsRunning())
			{
				if (m_Context->IsPaused())
				{
					m_Context->SetPaused(false);
				}
				else
				{
					m_Context->SetPaused(true);
				}
			}
		}
		if (ImGui::Button(GetLabelText(OPENLEVELBUTTON), m_ButtonSize))
		{
			m_ShowLoadLevelPanel = true;
		}
		ImGui::SameLine();
		if (ImGui::Button(GetLabelText(QUITBUTTON), m_ButtonSize))
		{
			Application::Get().Close();
		}
		ImGui::Separator();
		ImGui::Text(GetLabelText(GAMECONTROLLEVEL));
		ImGui::PushID("column set 1");
		ImGui::Columns(2, "column set 1", false);
		ImGui::SetColumnWidth(0, GetWidestGameControllerLabel());
		ImGui::Text(GetLabelText(NUMFLOATERSLEFTLABEL));
		ImGui::Text(GetLabelText(NUMBLOCKERSLEFTLABEL));
		ImGui::Text(GetLabelText(NUMDIGGERSLEFTLABEL));
		ImGui::Text(GetLabelText(NUMMINERSLEFTLABEL));
		ImGui::NextColumn();
		uint32_t left = m_Context->GetGameLevelData().MaxNumFloaters - m_Context->GetGameLevelData().NumFloatersUsed;
		ImGui::Text(std::to_string(left).c_str());
		left = m_Context->GetGameLevelData().MaxNumBlockers - m_Context->GetGameLevelData().NumBlockersUsed;
		ImGui::Text(std::to_string(left).c_str());
		left = m_Context->GetGameLevelData().MaxNumDiggers - m_Context->GetGameLevelData().NumDiggersUsed;
		ImGui::Text(std::to_string(left).c_str());
		left = m_Context->GetGameLevelData().MaxNumMiners - m_Context->GetGameLevelData().NumMinersUsed;
		ImGui::Text(std::to_string(left).c_str());
		ImGui::Columns(1);
		ImGui::PopID();
		if (ImGui::Button(GetLabelText(MAKEFLOATERBUTTON), m_ButtonSize))
		{
			if (m_Context->GetGameLevelData().UseFloater())
				ChangeClonemmingStatus(ClonemmingComponent::ClonemingStatus::Floater);
		}
		ImGui::SameLine();
		if (ImGui::Button(GetLabelText(MAKEBLOCKERBUTTON), m_ButtonSize))
		{
			if (m_Context->GetGameLevelData().UseBlocker())
				ChangeClonemmingStatus(ClonemmingComponent::ClonemingStatus::Blocker);
		}
		if (ImGui::Button(GetLabelText(MAKEDIGGERBUTTON), m_ButtonSize))
		{
			if (m_Context->GetGameLevelData().UseDigger())
				ChangeClonemmingStatus(ClonemmingComponent::ClonemingStatus::Digger);
		}
		ImGui::SameLine();
		if (ImGui::Button(GetLabelText(MAKEMINERBUTTON), m_ButtonSize))
		{
			if (m_Context->GetGameLevelData().UseMiner())
				ChangeClonemmingStatus(ClonemmingComponent::ClonemingStatus::Miner);
		}
		if (ImGui::Button(GetLabelText(MAKEWALKERBUTTON), m_ButtonSize))
		{
			
			RecycleClonemmingStatus();
		}
		float rate = 0.0f;
		Entity startpoint = m_Context->GetEntityByName("spawnpoint");
		if (startpoint)
		{
			rate = startpoint.GetComponent<ClonemmingStartComponent>().ClonemmingReleaseRate;
		}
		ImGui::PushID("Column set 2");
		ImGui::Columns(2, NULL, false);
		ImGui::SetColumnWidth(0, GetReleaseRateLabelWidth());
		ImGui::Text(GetLabelText(RELEASERATEDRAGFLOAT));
		ImGui::NextColumn();
		if (ImGui::SliderFloat("##ReleaseRateSlider", &rate, 0.1f, 10.0f))
		{
			if (startpoint)
				startpoint.GetComponent<ClonemmingStartComponent>().ClonemmingReleaseRate = rate;
		}
		ImGui::Columns(1);
		ImGui::PopID();
		ImGui::Separator();
		ImGui::Text(GetLabelText(STATISTICSLABEL));
		ImGui::PushID("Columns set 3");
		ImGui::Columns(2, "Columns set 3", false);
		ImGui::SetColumnWidth(0, m_LabelMap.at(CLONEMMINGSSAVEDLABEL).SizeInImGui.x + (ImGui::GetStyle().FramePadding.x*2));
		ImGui::Text(GetLabelText(CLONEMMINGSSAVEDLABEL));
		ImGui::Text(GetLabelText(CLONEMMINGSLOSTLABEL));	
		ImGui::NextColumn();
		Entity exitpoint = m_Context->GetEntityByName("Exitpoint");
		if (startpoint && exitpoint)
		{
			uint32_t numclonemmings = exitpoint.GetComponent<ClonemmingExitComponent>().NumberOfClonemmings;
			ImGui::Text(std::to_string(numclonemmings).c_str());
			uint32_t lostclonemmings = startpoint.GetComponent<ClonemmingStartComponent>().NumberOfClonemmings - numclonemmings;
			ImGui::Text(std::to_string(lostclonemmings).c_str());
		}
		else
		{
			ImGui::Text("Level not loaded!");
			ImGui::Text("Level not loaded");
		}
		ImGui::Columns(1);
		ImGui::PopID();
		ImGui::End();
		if (m_ShowLoadLevelPanel)
		{
			m_LoadLevelPanel.SetControlPanel(this);
			m_LoadLevelPanel.OnImGuiRender();
		}
		if (m_LoadLevelPanelResult == LevelLoad::Selected)
		{
			TRACE("Level selected");
			UUID leveluuid = m_LoadLevelPanel.GetSelectedLevelHandle();
			((GameLayer*)m_Context->GetGameLayer())->SetScene(Application::Get().GetAssetManager().GetAssetAs<Scene>(leveluuid));
			m_LoadLevelPanelResult = LevelLoad::Invalid;
		}
		else if (m_LoadLevelPanelResult == LevelLoad::Cancelled)
		{
			TRACE("Level load cancelled");
			m_LoadLevelPanelResult = LevelLoad::Invalid;
		}
	}
	void ControlPanel::LoadLabelText(const std::string& filename)
	{
#ifdef SAVELBLS
		m_LabelMap[CONTROLPANELLABEL] = LabelText(CONTROLPANELLABELTEXT);
		m_LabelMap[LEVELCONTROLSLABEL] = LabelText(LEVELCONTROLSLABELTEXT);
		m_LabelMap[STARTLEVELBUTTON] = LabelText(STARTLEVELBUTTONTEXT);
		m_LabelMap[PAUSELEVELBUTTON] = LabelText(PAUSELEVELBUTTONTEXT);
		m_LabelMap[OPENLEVELBUTTON] = LabelText(OPENLEVELBUTTONTEXT);
		m_LabelMap[QUITBUTTON] = LabelText(QUITBUTTONTEXT);
		m_LabelMap[GAMECONTROLLEVEL] = LabelText(GAMECONTROLLEVELTEXT);
		m_LabelMap[RELEASERATEDRAGFLOAT] = LabelText(RELEASERATEDRAGFLOATTEXT);
		m_LabelMap[NUMFLOATERSLEFTLABEL] = LabelText(NUMFLOATERSLEFTLABELTEXT);
		m_LabelMap[NUMBLOCKERSLEFTLABEL] = LabelText(NUMBLOCKERSLEFTLABELTEXT);
		m_LabelMap[NUMDIGGERSLEFTLABEL] = LabelText(NUMDIGGERSLEFTLABELTEXT);
		m_LabelMap[NUMMINERSLEFTLABEL] = LabelText(NUMMINERSLEFTLABELTEXT);
		m_LabelMap[MAKEFLOATERBUTTON] = LabelText(MAKEFLOATERBUTTONTEXT);
		m_LabelMap[MAKEBLOCKERBUTTON] = LabelText(MAKEBLOCKERBUTTONTEXT);
		m_LabelMap[MAKEDIGGERBUTTON] = LabelText(MAKEDIGGERBUTTONTEXT);
		m_LabelMap[MAKEMINERBUTTON] = LabelText(MAKEMINERBUTTONTEXT);
		m_LabelMap[MAKEWALKERBUTTON] = LabelText(MAKEWALKERBUTTONTEXT);
		m_LabelMap[STATISTICSLABEL] = LabelText(STATISTICSLABELTEXT);
		m_LabelMap[CLONEMMINGSSAVEDLABEL] = LabelText(CLONEMMINGSSAVEDLABELTEXT);
		m_LabelMap[CLONEMMINGSLOSTLABEL] = LabelText(CLONEMMINGSLOSTLABELTEXT);
		m_LabelMap[UNPAUSELEVELBUTTON] = LabelText(UNPAUSElEVELBUTTONTEXT);
		m_LabelMap[STOPLEVELBUTTON] = LabelText(STOPLEVELBUTTONTEXT);
		LabelSerialiser serialiser(this);
		serialiser.Serialise(filename);		
#else
		LabelSerialiser serialiser(this);
		if (!serialiser.Deserialise(filename))
		{
			CRITICAL("Unable to load Labels from file: {0}", filename);
		}
#endif // DEBUG
	}
	void ControlPanel::GetLargestButtonSize()
	{
		std::array<uint32_t, 11> buttonlist{ STARTLEVELBUTTON,PAUSELEVELBUTTON,OPENLEVELBUTTON,QUITBUTTON,MAKEFLOATERBUTTON,MAKEBLOCKERBUTTON,MAKEDIGGERBUTTON,MAKEMINERBUTTON,MAKEWALKERBUTTON,UNPAUSELEVELBUTTON,STOPLEVELBUTTON };
		uint32_t indexoflargest = 0;
		for (uint32_t n = 1; n != buttonlist.size(); n++)
		{
			if (m_LabelMap.at(buttonlist[indexoflargest]).operator<(m_LabelMap.at(buttonlist[n])))
			{
				indexoflargest = n;
			}
		}
		m_ButtonSize.x = m_LabelMap.at(buttonlist[indexoflargest]).SizeInImGui.x + (ImGui::GetStyle().FramePadding.x * 2);
		m_ButtonSize.y = m_LabelMap.at(buttonlist[indexoflargest]).SizeInImGui.y + (ImGui::GetStyle().FramePadding.y * 2);

	
	}
	const char* ControlPanel::GetLabelText(const uint32_t stringkey) const
	{
		auto it = m_LabelMap.find(stringkey);
		ASSERT(it != m_LabelMap.end(), "Invalid Label Text key");
		return it->second.Text.c_str();
	}
	void ControlPanel::ChangeClonemmingStatus(const ClonemmingComponent::ClonemingStatus status)
	{
		if (m_SelectedEntity)
		{
			if (m_SelectedEntity.HasComponent<ClonemmingComponent>())
			{
				m_SelectedEntity.GetComponent<ClonemmingComponent>().Status = status;
			}
		}
	}
	void ControlPanel::RecycleClonemmingStatus()
	{
		if (!m_SelectedEntity) return;
		switch (m_SelectedEntity.GetComponent<ClonemmingComponent>().Status)
		{
		case ClonemmingComponent::ClonemingStatus::Blocker:
			if (m_Context->GetGameLevelData().RecycleBlocker())
			{
				ChangeClonemmingStatus(ClonemmingComponent::ClonemingStatus::Walker);
				return;
			}
			break;
		case ClonemmingComponent::ClonemingStatus::Floater:
			if (m_Context->GetGameLevelData().RecycleFloater())
			{
				ChangeClonemmingStatus(ClonemmingComponent::ClonemingStatus::Walker);
				return;
			}
			break;
		case ClonemmingComponent::ClonemingStatus::Digger:
			if (m_Context->GetGameLevelData().RecycleDigger())
			{
				ChangeClonemmingStatus(ClonemmingComponent::ClonemingStatus::Walker);
				return;
			}
			break;
		case ClonemmingComponent::ClonemingStatus::Miner:
			if (m_Context->GetGameLevelData().RecycleMiner())
			{
				ChangeClonemmingStatus(ClonemmingComponent::ClonemingStatus::Walker);
				return;
			}
			break;
		case ClonemmingComponent::ClonemingStatus::Walker:
			return;
		}
		ASSERT(false, "Unknown clonemming status");
	}
	const char* ControlPanel::GetStartStopButtonText() const
	{
		if (m_Context->IsRunning())
			return GetLabelText(STOPLEVELBUTTON);
		else
			return GetLabelText(STARTLEVELBUTTON);
	}
	const char* ControlPanel::GetPauseUnpauseButtonText() const
	{
		if (!m_Context->IsRunning())
			return GetLabelText(PAUSELEVELBUTTON);
		if (m_Context->IsPaused())
			return GetLabelText(UNPAUSELEVELBUTTON);
		else
			return GetLabelText(PAUSELEVELBUTTON);
	}
	float ControlPanel::GetReleaseRateLabelWidth() const
	{
		return m_LabelMap.at(RELEASERATEDRAGFLOAT).SizeInImGui.x + (ImGui::GetStyle().FramePadding.x * 2);
	}
	float ControlPanel::GetWidestGameControllerLabel()
	{
		std::array<uint32_t, 4> labellist = { NUMFLOATERSLEFTLABEL,NUMBLOCKERSLEFTLABEL,NUMDIGGERSLEFTLABEL,NUMMINERSLEFTLABEL };
		uint32_t indexofwidest = 0;
		for (uint32_t n = 1; n != labellist.size(); n++)
		{
			if (m_LabelMap.at(labellist[indexofwidest]) < m_LabelMap.at(labellist[n]))
			{
				indexofwidest = n;
			}
		}
		return m_LabelMap.at(labellist[indexofwidest]).SizeInImGui.x + (ImGui::GetStyle().FramePadding.x * 2);
	}
	float ControlPanel::GetWidestStatisticsLabel()
	{
		if (m_LabelMap.at(CLONEMMINGSSAVEDLABEL) < m_LabelMap.at(CLONEMMINGSLOSTLABEL))
		{
			return m_LabelMap.at(CLONEMMINGSLOSTLABEL).SizeInImGui.x + (ImGui::GetStyle().FramePadding.x * 2);
		}
		else
		{
			return m_LabelMap.at(CLONEMMINGSSAVEDLABEL).SizeInImGui.x + (ImGui::GetStyle().FramePadding.x * 2);
		}
	}
}