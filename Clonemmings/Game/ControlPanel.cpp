#include "Game/ControlPanel.h"
#include "Game/DefaultLabels.h"
#include "Game/GameLayer.h"
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
		if (!entity.HasComponent<ClonemmingComponent>())
		{
			m_SelectedEntity = {};
			return;
		}
		m_SelectedEntity = entity;

	}
	void ControlPanel::OnImGuiRender()
	{
		ImGui::Begin(GetLabelText(CONTROLPANELLABEL));
		ImGui::Text(GetLabelText(LEVELCONTROLSLABEL));
		if (ImGui::Button(GetLabelText(STARTLEVELBUTTON), m_ButtonSize))
		{
			if (!m_Context->IsRunning())
				m_Context->StartScene();
		}
		ImGui::SameLine();
		if (ImGui::Button(GetLabelText(PAUSELEVELBUTTON), m_ButtonSize))
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
			std::string filename = FileDialog::OpenFile("lvl");
			if (!filename.empty())
			{
				((GameLayer*)m_Context->GetGameLayer())->LoadScene(filename);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(GetLabelText(QUITBUTTON), m_ButtonSize))
		{
			Application::Get().Close();
		}
		ImGui::Separator();
		ImGui::Text(GetLabelText(GAMECONTROLLEVEL));
		ImGui::Text(GetLabelText(NUMFLOATERSLEFTLABEL));
		ImGui::SameLine();
		uint32_t left = m_Context->GetGameLevelData().MaxNumFloaters - m_Context->GetGameLevelData().NumFloatersUsed;
		ImGui::Text(std::to_string(left).c_str());
		ImGui::Text(GetLabelText(NUMBLOCKERSLEFTLABEL));
		ImGui::SameLine();
		left = m_Context->GetGameLevelData().MaxNumBlockers - m_Context->GetGameLevelData().NumBlockersUsed;
		ImGui::Text(std::to_string(left).c_str());
		ImGui::Text(GetLabelText(NUMDIGGERSLEFTLABEL));
		ImGui::SameLine();
		left = m_Context->GetGameLevelData().MaxNumDiggers - m_Context->GetGameLevelData().NumDiggersUsed;
		ImGui::Text(std::to_string(left).c_str());
		ImGui::Text(GetLabelText(NUMMINERSLEFTLABEL));
		ImGui::SameLine();
		left = m_Context->GetGameLevelData().MaxNumMiners - m_Context->GetGameLevelData().NumMinersUsed;
		ImGui::Text(std::to_string(left).c_str());
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
		if (ImGui::DragFloat(GetLabelText(RELEASERATEDRAGFLOAT), &rate, 1.0f, 0.1f, 10.0f))
		{
			if (startpoint)
				startpoint.GetComponent<ClonemmingStartComponent>().ClonemmingReleaseRate = rate;
		}
		ImGui::Separator();
		ImGui::Text(GetLabelText(STATISTICSLABEL));
		ImGui::Text(GetLabelText(CLONEMMINGSSAVEDLABEL));
		ImGui::SameLine();
		Entity exitpoint = m_Context->GetEntityByName("Exitpoint");
		if (startpoint && exitpoint)
		{
			uint32_t numclonemmings = exitpoint.GetComponent<ClonemmingExitComponent>().NumberOfClonemmings;
			ImGui::Text(std::to_string(numclonemmings).c_str());
			ImGui::Text(GetLabelText(CLONEMMINGSLOSTLABEL));
			ImGui::SameLine();
			uint32_t lostclonemmings = startpoint.GetComponent<ClonemmingStartComponent>().NumberOfClonemmings - numclonemmings;
			ImGui::Text(std::to_string(lostclonemmings).c_str());
		}
		else
		{
			ImGui::Text("Level not loaded!");
			ImGui::Text(GetLabelText(CLONEMMINGSLOSTLABEL));
			ImGui::SameLine();
			ImGui::Text("Level not loaded");
		}
		ImGui::End();
	}
	void ControlPanel::LoadLabelText(const std::string& filename)
	{
#ifdef DEBUG
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

		
#else
		LabelSerialiser serialiser(this);
		serialiser.Deserialise(filename);
#endif // DEBUG
		GetLargestButtonSize();
	}
	void ControlPanel::GetLargestButtonSize()
	{
		std::array<uint32_t, 9> buttonlist{ STARTLEVELBUTTON,PAUSELEVELBUTTON,OPENLEVELBUTTON,QUITBUTTON,MAKEFLOATERBUTTON,MAKEBLOCKERBUTTON,MAKEDIGGERBUTTON,MAKEMINERBUTTON,MAKEWALKERBUTTON };
		uint32_t indexoflargest = 0;
		for (uint32_t n = 1; n != buttonlist.size(); n++)
		{
			if (m_LabelMap.at(indexoflargest).operator<(m_LabelMap.at(n)))
			{
				indexoflargest = n;
			}
		}
		m_ButtonSize.x = m_LabelMap.at(indexoflargest).SizeInImGui.x;
		m_ButtonSize.y = m_LabelMap.at(indexoflargest).SizeInImGui.y + (ImGui::GetStyle().FramePadding.y * 2);

	
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
}