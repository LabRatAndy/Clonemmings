#pragma once
#include "Core/Scene/Scene.h"
#include "Core/Scene/Entity.h"

#include <imgui.h>

#include <memory>
#include <unordered_map>
#include <string>

namespace Clonemmings
{
	struct LabelText
	{
		std::string Text = "";
		ImVec2 SizeInImGui = { 0.0,0.0 };
		
		LabelText() = default;
		LabelText(const std::string& text)
		{
			Text = text;
			SizeInImGui = ImGui::CalcTextSize(Text.c_str());
		}
		bool operator <(const LabelText& other)
		{
			return (SizeInImGui.x < other.SizeInImGui.x);
		}
		bool operator >(const LabelText& other)
		{
			return !(*this < other);
		}
		bool operator <=(const LabelText& other)
		{
			return(SizeInImGui.x <= other.SizeInImGui.x);
		}
		bool operator >=(const LabelText& other)
		{
			return !(*this <= other);
		}
		bool operator ==(const LabelText& other)
		{
			return SizeInImGui.x == other.SizeInImGui.x;
		}
		bool operator !=(const LabelText& other)
		{
			return !(*this == other);
		}
	};
	class ControlPanel
	{
	public:
		ControlPanel() = default;
		ControlPanel(std::shared_ptr<Scene>& scene);
		void SetContext(std::shared_ptr<Scene>& context);
		void SetSelectedEntity(Entity entity);
		void OnImGuiRender();
		void LoadLabelText(const std::string& filename);

	private:
		std::shared_ptr<Scene> m_Context = nullptr;
		Entity m_SelectedEntity = {};
		std::unordered_map<uint32_t, LabelText> m_LabelMap;
		ImVec2 m_ButtonSize = { 0.0,0.0 };

		void GetLargestButtonSize();
		const char* GetLabelText(const uint32_t stringkey) const;
		const char* GetStartStopButtonText() const;
		const char* GetPauseUnpauseButtonText() const;
		void ChangeClonemmingStatus(const ClonemmingComponent::ClonemingStatus status);
		void RecycleClonemmingStatus();
		float GetReleaseRateLabelWidth() const;
		float GetWidestGameControllerLabel();
		float GetWidestStatisticsLabel();
		

		friend class LabelSerialiser;

	};
}