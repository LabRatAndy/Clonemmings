#include "Core/Scene/SceneSerialiser.h"
#include "Core/Scene/Entity.h"
#include "Core/Scene/Components.h"

#include <yaml-cpp/yaml.h>

#include <fstream>

namespace YAML
{
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}
		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3) return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};
	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}
		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4) return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}
namespace Clonemmings
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& value)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << value.x << value.y << value.z << YAML::EndSeq;
		return out;
	}
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& value)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << value.x << value.y << value.z << value.w << YAML::EndSeq;
		return out;
	}
	static void SerialiseEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << "1234567890123456"; //change to entity UUID  here!

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;
			out << YAML::EndMap;
		}
		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;
			auto& transform = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << transform.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << transform.Scale;
			out << YAML::EndMap;
		}
		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;
			auto& cameracomponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameracomponent.Camera;
			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::EndMap;
			out << YAML::Key << "Primary" << YAML::Value << cameracomponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameracomponent.FixedAspectRatio;
			out << YAML::EndMap;
		}
		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;
			auto& src = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Colour" << YAML::Value << src.Colour;
			out << YAML::EndMap;
		}
		out << YAML::EndMap;
	}

	SceneSerialiser::SceneSerialiser(const std::shared_ptr<Scene>& scene) :m_Scene(scene)
	{

	}

	void SceneSerialiser::Serialise(const std::string& filename)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityid)
			{
				Entity entity = { entityid,m_Scene.get() };
				if (!entity) return;
				SerialiseEntity(out, entity);
			});
		out << YAML::EndSeq;
		out << YAML::EndMap;
		std::ofstream fout(filename);
		fout << out.c_str();
	}
	bool SceneSerialiser::Deserialise(const std::string& filename)
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
		if (!data["Scene"]) return false;
		std::string scenename = data["Scene"].as<std::string>();
		TRACE("Deserialising Scene '{0}'", scenename);
		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();
				std::string name;
				auto tagcomponent = entity["TagComponent"];
				if (tagcomponent)
				{
					name = tagcomponent["Tag"].as<std::string>();
				}
				TRACE("Deserialised Entity with ID = {0}, name = {1}", uuid, name);
				Entity deserialisedentity = m_Scene->CreateEntity(name);
				auto transformcomponent = entity["TransformComponent"];
				if (transformcomponent)
				{
					auto& tc = deserialisedentity.GetComponent<TransformComponent>();
					tc.Translation = transformcomponent["Translation"].as<glm::vec3>();
					tc.Rotation = transformcomponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformcomponent["Scale"].as<glm::vec3>();
				}
				auto cameracomponent = entity["CameraComponent"];
				if (cameracomponent)
				{
					auto& cc = deserialisedentity.AddComponent<CameraComponent>();
					auto& cameraprops = cameracomponent["Camera"];
					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraprops["ProjectionType"].as<int>());
					cc.Camera.SetPerspectiveVerticalFOV(cameraprops["PerspectiveFOV"].as<float>());
					cc.Camera.SetPerspectiveNearClip(cameraprops["PerspectiveNear"].as<float>());
					cc.Camera.SetPerspectiveFarClip(cameraprops["PerspectiveFar"].as<float>());
					cc.Camera.SetOrthographicSize(cameraprops["OrthographicSize"].as<float>());
					cc.Camera.SetOrthographicNearClip(cameraprops["OrthographicNear"].as<float>());
					cc.Camera.SetOrthographicFarClip(cameraprops["OrthographicFar"].as<float>());
					cc.Primary = cameracomponent["Primary"].as<bool>();
					cc.FixedAspectRatio = cameracomponent["FixedAspectRatio"].as<bool>();
				}
				auto spriterenderercomponent = entity["SpriteRendererComponent"];
				if (spriterenderercomponent)
				{
					auto& src = deserialisedentity.AddComponent<SpriteRendererComponent>();
					src.Colour = spriterenderercomponent["Colour"].as<glm::vec4>();
				}
			}
		}
		return true;
	}
}

