#include "Core/Scene/SceneSerialiser.h"
#include "Core/Scene/Entity.h"
#include "Core/Scene/Components.h"
#include "Core/Application/UUID.h"

#include <yaml-cpp/yaml.h>

#include <fstream>

namespace YAML
{
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}
		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2) return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};
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
	template<>
	struct convert<Clonemmings::UUID>
	{
		static Node encode(const Clonemmings::UUID& uuid)
		{
			Node node;
			node.push_back((uint64_t)uuid);
			return node;
		}
		static bool decode(const Node& node, Clonemmings::UUID uuid)
		{
			uuid = node.as<uint64_t>();
			return true;
		}
	};
}
namespace Clonemmings
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& value)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << value.x << value.y << YAML::EndSeq;
		return out;
	}
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
	static std::string RigidBody2DTypeToString(RigidBody2DComponent::BodyType bodytype)
	{
		switch (bodytype)
		{
		case RigidBody2DComponent::BodyType::Static: return "Static";
		case RigidBody2DComponent::BodyType::Dynamic: return "Dynamic";
		case RigidBody2DComponent::BodyType::Kinematic: return "Kinematic";
		}
		ASSERT(false, "Unknown body type");
		return {};
	}
	static RigidBody2DComponent::BodyType RigidBody2DTypeFromString(const std::string& bodytypestring)
	{
		if (bodytypestring == "Static") return RigidBody2DComponent::BodyType::Static;
		if (bodytypestring == "Dynamic") return RigidBody2DComponent::BodyType::Dynamic;
		if (bodytypestring == "Kinematic") return RigidBody2DComponent::BodyType::Kinematic;
		ASSERT(false, "Unknown body type string");
		return RigidBody2DComponent::BodyType::Static;
	}
	static void SerialiseEntity(YAML::Emitter& out, Entity entity)
	{
		ASSERT(entity.HasComponent<IDComponent>())
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

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
			if (src.Tex)
			{
				out << YAML::Key << "TexturePath" << YAML::Value << src.Tex->GetPath();
			}
			out << YAML::Key << "TilingFactor" << YAML::Value << src.TilingFactor;
			out << YAML::EndMap;
		}
		if (entity.HasComponent<RigidBody2DComponent>())
		{
			out << YAML::Key << "RigidBody2DComponent";
			out << YAML::BeginMap;
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			out << YAML::Key << "BodyType" << YAML::Value << RigidBody2DTypeToString(rb2d.Type);
			out << YAML::Key << "FixedRoatation" << YAML::Value << rb2d.FixedRotation;
			out << YAML::EndMap;
		}
		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap;
			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << bc2d.Offset;
			out << YAML::Key << "Size" << YAML::Value << bc2d.Size;
			out << YAML::Key << "Density" << YAML::Value << bc2d.Density;
			out << YAML::Key << "Friction" << YAML::Value << bc2d.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << bc2d.Restitution;
			out << YAML::Key << "Restutution Threshold" << YAML::Value << bc2d.RestitutionThreshold;
			out << YAML::EndMap;
		}
		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap;
			auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << cc2d.Offset;
			out << YAML::Key << "Radius" << YAML::Value << cc2d.Radius;
			out << YAML::Key << "Density" << YAML::Value << cc2d.Density;
			out << YAML::Key << "Friction" << YAML::Value << cc2d.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << cc2d.Restitution;
			out << YAML::Key << "Restitution Threshold" << YAML::Value << cc2d.RestitutionThreshold;
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
				Entity deserialisedentity = m_Scene->CreateEntityWithUUID(uuid, name);
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
					if (spriterenderercomponent["Texture"])
					{
						std::string texturepath = spriterenderercomponent["TexturePath"].as<std::string>();
						src.Tex = std::make_shared<Texture>(texturepath);
					}
					if (spriterenderercomponent["TilingFactor"])
					{
						src.TilingFactor = spriterenderercomponent["TilingFactor"].as<float>();
					}
				}
				auto rigidbody2Dcomponent = entity["RigidBody2DComponent"];
				if (rigidbody2Dcomponent)
				{
					auto& rb2d = deserialisedentity.AddComponent<RigidBody2DComponent>();
					rb2d.Type = RigidBody2DTypeFromString(rigidbody2Dcomponent["Type"].as<std::string>());
					rb2d.FixedRotation = rigidbody2Dcomponent["FixedRotation"].as<bool>();
				}
				auto boxcollider2Dcomponent = entity["BoxCollider2DComponent"];
				if (boxcollider2Dcomponent)
				{
					auto& bc2d = deserialisedentity.AddComponent<BoxCollider2DComponent>();
					bc2d.Offset = boxcollider2Dcomponent["Offset"].as<glm::vec2>();
					bc2d.Size = boxcollider2Dcomponent["Size"].as<glm::vec2>();
					bc2d.Density = boxcollider2Dcomponent["Density"].as<float>();
					bc2d.Friction = boxcollider2Dcomponent["Friction"].as<float>();
					bc2d.Restitution = boxcollider2Dcomponent["Restitution"].as<float>();
					bc2d.RestitutionThreshold = boxcollider2Dcomponent["RestitutionThreshold"].as<float>();
				}
				auto circlecollider2Dcomponent = entity["CircleCollider2DComponent"];
				if (circlecollider2Dcomponent)
				{
					auto& cc2d = deserialisedentity.AddComponent<CircleCollider2DComponent>();
					cc2d.Offset = circlecollider2Dcomponent["Offset"].as<glm::vec2>();
					cc2d.Radius = circlecollider2Dcomponent["Radius"].as<float>();
					cc2d.Density = circlecollider2Dcomponent["Density"].as<float>();
					cc2d.Friction = circlecollider2Dcomponent["Friction"].as<float>();
					cc2d.Restitution = circlecollider2Dcomponent["Restitution"].as<float>();
					cc2d.RestitutionThreshold = circlecollider2Dcomponent["RestitutionThreshold"].as<float>();
				}
			}
		}
		return true;
	}
}

