#include "Game/LabelSerialiser.h"
#include "Game/ControlPanel.h"

#include <yaml-cpp/yaml.h>

#include <ostream>
#include <filesystem>

namespace Clonemmings
{
	static void CheckAndCreateDirectory(const std::string& filepath)
	{
		std::filesystem::path path(filepath);
		std::filesystem::path directory = path.parent_path();
		if (!std::filesystem::is_directory(directory))
		{
			std::filesystem::create_directory(directory);
		}
	}

	LabelSerialiser::LabelSerialiser(ControlPanel* controlpanel) : m_ControlPanelPtr(controlpanel)
	{

	}

	void LabelSerialiser::Serialise(const std::string& filename)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "LabelText" << YAML::Value << "Label strings for Clonemmings control panel";
		out << YAML::Key << "labels" << YAML::Value << YAML::BeginSeq;
		uint32_t count = 0;
		for (auto it = m_ControlPanelPtr->m_LabelMap.begin(); it != m_ControlPanelPtr->m_LabelMap.end(); ++it)
		{
			out << YAML::Key << YAML::BeginMap;
			out << YAML::Key << "LabelID" << YAML::Value << it->first;
			LabelText text = it->second;
			out << YAML::Key << "LabelText" << YAML::Value << text.Text;
			out << YAML::EndMap;
			count++;
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;
		CheckAndCreateDirectory(filename);
		std::ofstream output(filename.c_str(), std::ios::out | std::ios::trunc);
		if (output.is_open())
		{
			output << out.c_str();
		}
		output.close();
	}

	bool LabelSerialiser::Deserialise(const std::string& filename)
	{
		YAML::Node data;
		
		try
		{
			data = YAML::LoadFile(filename);
		}
		catch (YAML::ParserException e)
		{
			return false;
		}

		if (!data["LabelText"])
			return false;
		auto labels = data["Labels"];
		if (labels)
		{
			m_ControlPanelPtr->m_LabelMap.clear();
			for (auto label : labels)
			{
				uint32_t labelID = label["LabelID"].as<uint32_t>();
				std::string labeltext = label["LabelText"].as<std::string>();
				LabelText text(labeltext);
				m_ControlPanelPtr->m_LabelMap[labelID] = text;
			}
		}
		return true;
	}
}