#include "Core/Scene/SceneSerialiser.h"
#include "Core/Scene/Entity.h"
#include "Core/Scene/CoreComponents.h"
#include "Core/Application/UUID.h"
#include "Core/Scripting/ScriptEngine.h"
#include "Game/GameLevelData.h"
#include "Core/Physic2D/PhysicsEngine.h"

#include <yaml-cpp/yaml.h>

#include <fstream>
#include <ostream>
#include <filesystem>

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
#define WRITE_SCRIPT_FIELD(FieldType, Type)		\
			case ScriptFieldType::FieldType:	\
				out<<scriptfield.GetValue<Type>();	\
				break

#define READ_SCRIPT_FIELD(FieldType, Type)		\
			case ScriptFieldType::FieldType:	\
			{									\
				Type data = scriptfield["Data"].as<Type>();		\
				fieldinstance.SetValue(data);					\
				break;											\
			}													\

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
	static void CheckAndCreateDirectory(const std::string& filepath)
	{
		std::filesystem::path path(filepath);
		std::filesystem::path directory = path.parent_path();
		//check if directory  is a  dir and if it exists
		if (!std::filesystem::is_directory(directory))
		{
			// if not, need to create the dir 
			std::filesystem::create_directory(directory);
		}
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
	static  std::string ClonemmingStatusToString(ClonemmingComponent::ClonemingStatus status)
	{
		switch (status)
		{
			case ClonemmingComponent::ClonemingStatus::Walker: return "Walker";
			case ClonemmingComponent::ClonemingStatus::Blocker: return "Blocker";
			case ClonemmingComponent::ClonemingStatus::Dead: return "Dead";
			case ClonemmingComponent::ClonemingStatus::Digger: return "Digger";
			case ClonemmingComponent::ClonemingStatus::Floater: return "Floater";
			case ClonemmingComponent::ClonemingStatus::Miner: return "Miner";
		}
		ASSERT(false, "Unknown clonemming status");
		return {};
	}
	static ClonemmingComponent::ClonemingStatus ClonemmingStatusFromString(const std::string& statusstring)
	{
		if (statusstring == "Walker") return ClonemmingComponent::ClonemingStatus::Walker;
		if (statusstring == "Blocker") return ClonemmingComponent::ClonemingStatus::Blocker;
		if (statusstring == "Dead") return ClonemmingComponent::ClonemingStatus::Dead;
		if (statusstring == "Digger") return ClonemmingComponent::ClonemingStatus::Digger;
		if (statusstring == "Floater") return ClonemmingComponent::ClonemingStatus::Floater;
		if (statusstring == "Miner") return ClonemmingComponent::ClonemingStatus::Miner;
		ASSERT(false, "Unknown clonemming status string");
		return ClonemmingComponent::ClonemingStatus::Walker;
	}
	static std::string CollisionCategoryToString(CollisionCategory category)
	{
		switch (category)
		{
		case CollisionCategory::Default: return "Default";
		case CollisionCategory::Clonemming: return "Clonemming";
		case CollisionCategory::DigableFloor: return "DigableFloor";
		case CollisionCategory::ExitPoint: return "ExitPoint";
		case CollisionCategory::Floor: return "Floor";
		case CollisionCategory::MineableWall: return "MineableWall";
		case CollisionCategory::SpawnPoint: return "SpawnPoint";
		case CollisionCategory::Wall: return "Wall";
		}
		ASSERT(false, "Unknown Collision Category");
		return {};
	}
	static CollisionCategory CollisionCategoryFromString(const std::string& categorystring)
	{
		if (categorystring == "Default") return CollisionCategory::Default;
		if (categorystring == "Clonemming") return CollisionCategory::Clonemming;
		if (categorystring == "DigableFloor") return CollisionCategory::DigableFloor;
		if (categorystring == "ExitPoint") return CollisionCategory::ExitPoint;
		if (categorystring == "Floor") return CollisionCategory::Floor;
		if (categorystring == "MineableWall") return CollisionCategory::MineableWall;
		if (categorystring == "SpawnPoint") return CollisionCategory::SpawnPoint;
		if (categorystring == "Wall") return CollisionCategory::Wall;
		ASSERT(false, "Unknown Collision Category string");
		return CollisionCategory::Default;
	}
	static CollisionMasks CollisionMaskFromString(const std::string& maskString)
	{
		if (maskString == "Clonemmings") return CollisionMasks::Clonemmings;
		if (maskString == "Everything") return CollisionMasks::Everything;
		if (maskString == "Nothing") return CollisionMasks::Nothing;
		if (maskString == "Scenary") return CollisionMasks::Scenary;
		ASSERT(false, "Unknown collision mask string");
		return CollisionMasks::Everything;
	}
	static std::string CollisionMaskToString(CollisionMasks mask)
	{
		switch (mask)
		{
		case CollisionMasks::Clonemmings: return "Clonemmings";
		case CollisionMasks::Everything: return "Everything";
		case CollisionMasks::Nothing: return "Nothing";
		case CollisionMasks::Scenary: return "Scenary";
		}
		ASSERT(false, "Unknown Collision mask");
		return {};
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
			out << YAML::Key << "LevelHeight" << YAML::Value << camera.GetLevelHeight();
			out << YAML::Key << "LevelWidth" << YAML::Value << camera.GetLevelWidth();
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
			out << YAML::Key << "FixedRotation" << YAML::Value << rb2d.FixedRotation;
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
			out << YAML::Key << "Restitution Threshold" << YAML::Value << bc2d.RestitutionThreshold;
			out << YAML::Key << "Category" << YAML::Value << CollisionCategoryToString((CollisionCategory)bc2d.Category);
			out << YAML::Key << "Mask" << YAML::Value << CollisionMaskToString((CollisionMasks)bc2d.Mask);
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
			out << YAML::Key << "Category" << YAML::Value << CollisionCategoryToString((CollisionCategory)cc2d.Category);
			out << YAML::Key << "Mask" << YAML::Value << CollisionMaskToString((CollisionMasks)cc2d.Mask);
			out << YAML::EndMap;
		}
		if (entity.HasComponent<ClonemmingComponent>())
		{
			auto& cc = entity.GetComponent<ClonemmingComponent>();
			out << YAML::Key << "ClonemmingComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Clonemming Status" << YAML::Value << ClonemmingStatusToString(cc.Status);
			out << YAML::Key << "Walk Speed" << YAML::Value << cc.WalkSpeed;
			out << YAML::Key << "Dig Speed" << YAML::Value << cc.DigSpeed;
			out << YAML::Key << "Mine Speed" << YAML::Value << cc.MineSpeed;
			out << YAML::EndMap;
		}
		if (entity.HasComponent<ClonemmingStartComponent>())
		{
			auto& csc = entity.GetComponent<ClonemmingStartComponent>();
			out << YAML::Key << "ClonemmingStartComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "NumberOfClonemmings" << YAML::Value << csc.NumberOfClonemmings;
			out << YAML::Key << "ClonemmingReleaseRate" << YAML::Value << csc.ClonemmingReleaseRate;
			out << YAML::EndMap;
		}
		if (entity.HasComponent<ClonemmingExitComponent>())
		{
			auto& cec = entity.GetComponent<ClonemmingExitComponent>();
			out << YAML::Key << "ClonemmingExitComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "NumberOfClonemmings" << YAML::Value << cec.NumberOfClonemmings;
			out << YAML::EndMap;
		}
		if (entity.HasComponent<ScriptComponent>())
		{
			auto& sc = entity.GetComponent<ScriptComponent>();
			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "ClassName" << YAML::Value << sc.ClassName;
			//save fields
			std::shared_ptr<ScriptClass> entityclass = ScriptEngine::GetEntityClass(sc.ClassName);
			const auto& fields = entityclass->GetFields();
			if (fields.size() > 0)
			{
				out << YAML::Key << "ScriptFields" << YAML::Value;
				auto& entityfields = ScriptEngine::GetScriptFieldMap(entity);
				out << YAML::BeginSeq;
				for (const auto& [name, field] : fields)
				{
					if (entityfields.find(name) == entityfields.end())
					{
						continue;
					}
					out << YAML::BeginMap;
					out << YAML::Key << "Name" << YAML::Value << name;
					out << YAML::Key << "Type" << YAML::Value << Utills::ScriptFieldTypeToString(field.Type);
					out << YAML::Key << "Data" << YAML::Value;
					ScriptFieldInstance& scriptfield = entityfields.at(name);
					switch (field.Type)
					{
						WRITE_SCRIPT_FIELD(Float, float);
						WRITE_SCRIPT_FIELD(Double, double);
						WRITE_SCRIPT_FIELD(Bool, bool);
						WRITE_SCRIPT_FIELD(Char, char);
						WRITE_SCRIPT_FIELD(Byte, int8_t);
						WRITE_SCRIPT_FIELD(Short, int16_t);
						WRITE_SCRIPT_FIELD(Int, int32_t);
						WRITE_SCRIPT_FIELD(Long, int64_t);
						WRITE_SCRIPT_FIELD(UByte, uint8_t);
						WRITE_SCRIPT_FIELD(UShort, uint16_t);
						WRITE_SCRIPT_FIELD(UInt, uint32_t);
						WRITE_SCRIPT_FIELD(ULong, uint64_t);
						WRITE_SCRIPT_FIELD(Vector2, glm::vec2);
						WRITE_SCRIPT_FIELD(Vector3, glm::vec3);
						WRITE_SCRIPT_FIELD(Vector4, glm::vec4);
						WRITE_SCRIPT_FIELD(Entity, UUID);						
					}
					out << YAML::EndMap;
				}
				out << YAML::EndSeq;
			}
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
		out << YAML::Key << "GameLevelData";
		out << YAML::BeginMap;
		out << YAML::Key << "MaxNumFloaters" << YAML::Value << m_Scene->m_GameLevelData.MaxNumFloaters;
		out << YAML::Key << "MaxNumBlockers" << YAML::Value << m_Scene->m_GameLevelData.MaxNumBlockers;
		out << YAML::Key << "MaxNumDiggers" << YAML::Value << m_Scene->m_GameLevelData.MaxNumDiggers;
		out << YAML::Key << "MaxNumMiners" << YAML::Value << m_Scene->m_GameLevelData.MaxNumMiners;
		out << YAML::Key << "CanRecycleFloaters" << YAML::Value << m_Scene->m_GameLevelData.CanRecycleFloaters;
		out << YAML::Key << "CanRecycleBlockers" << YAML::Value << m_Scene->m_GameLevelData.CanRecycleBlockers;
		out << YAML::Key << "CanRecycleDiggers" << YAML::Value << m_Scene->m_GameLevelData.CanRecycleDiggers;
		out << YAML::Key << "CanRecycleMiners" << YAML::Value << m_Scene->m_GameLevelData.CanRecycleMiners;
		out << YAML::Key << "TargetToSavePercentage" << YAML::Value << m_Scene->m_GameLevelData.PercentageToSave;
		out << YAML::Key << "MaxSurivivableVelocityChange" << YAML::Value << m_Scene->m_GameLevelData.DeadVelocityChange;
		out << YAML::EndMap;
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityid)
			{
				Entity entity = { entityid,m_Scene.get() };
				if (!entity) return;
				SerialiseEntity(out, entity);
			});
		out << YAML::EndSeq;
		out << YAML::EndMap;
		CheckAndCreateDirectory(filename);
		std::ofstream fout(filename.c_str(), std::ios::out | std::ios::trunc);
		if (fout.is_open())
		{
			fout << out.c_str();
		}
		fout.close();
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
		auto gldata = data["GameLevelData"];
		if (gldata)
		{
			m_Scene->m_GameLevelData.MaxNumBlockers = gldata["MaxNumBlockers"].as<uint32_t>();
			m_Scene->m_GameLevelData.MaxNumDiggers = gldata["MaxNumDiggers"].as<uint32_t>();
			m_Scene->m_GameLevelData.MaxNumFloaters = gldata["MaxNumFloaters"].as<uint32_t>();
			m_Scene->m_GameLevelData.MaxNumMiners = gldata["MaxNumMiners"].as<uint32_t>();
			m_Scene->m_GameLevelData.CanRecycleBlockers = gldata["CanRecycleBlockers"].as<bool>();
			m_Scene->m_GameLevelData.CanRecycleDiggers = gldata["CanRecycleBlockers"].as<bool>();
			m_Scene->m_GameLevelData.CanRecycleFloaters = gldata["CanRecycleFloaters"].as<bool>();
			m_Scene->m_GameLevelData.CanRecycleMiners = gldata["CanRecycleMiners"].as<bool>();
			m_Scene->m_GameLevelData.PercentageToSave = gldata["TargetToSavePercentage"].as<float>();
			m_Scene->m_GameLevelData.DeadVelocityChange = gldata["MaxSurivivableVelocityChange"].as<float>();
		}
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
					/*float x = tc.Translation.x;
					float y = tc.Translation.y;
					float sx = tc.Scale.x;
					float sy = tc.Scale.y;
					tc.Translation.x = x * 10.0f;
					tc.Translation.y = y * 10.0f;
					tc.Scale.x = sx * 10.0f;
					tc.Scale.y = sy * 10.0f;*/
					TRACE("Deserialised transform componenent: Translation: X  {0}, Y {1}, Z {2}", tc.Translation.x, tc.Translation.y, tc.Translation.z);
					TRACE("Deserialised transform componenent: Rotation: X  {0}, Y {1}, Z {2}", tc.Rotation.x, tc.Rotation.y, tc.Rotation.z);
					TRACE("Deserialised transform componenent: Scale: X  {0}, Y {1}, Z {2}", tc.Scale.x, tc.Scale.y, tc.Scale.z);

				}
				auto cameracomponent = entity["CameraComponent"];
				if (cameracomponent)
				{
					auto& cc = deserialisedentity.AddComponent<CameraComponent>();
					auto& cameraprops = cameracomponent["Camera"];
					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraprops["ProjectionType"].as<int>());
					TRACE("Camera component projection type: {0}", (int)cc.Camera.GetProjectionType());
					cc.Camera.SetPerspectiveVerticalFOV(cameraprops["PerspectiveFOV"].as<float>());
					TRACE("Camera component perspective FOV: {0}", cc.Camera.GetPerspectiveVerticalFOV());
					cc.Camera.SetPerspectiveNearClip(cameraprops["PerspectiveNear"].as<float>());
					TRACE("Camera component perspective near: {0}", cc.Camera.GetPerspectiveNearClip());
					cc.Camera.SetPerspectiveFarClip(cameraprops["PerspectiveFar"].as<float>());
					TRACE("Camera component perspective far: {0}", cc.Camera.GetPerspectiveFarClip());
					cc.Camera.SetOrthographicSize(cameraprops["OrthographicSize"].as<float>());
					TRACE("Camera component orthographic size: {0}", cc.Camera.GetOrthographicSize());
					cc.Camera.SetOrthographicNearClip(cameraprops["OrthographicNear"].as<float>());
					TRACE("Camera component orthographic near: {0}", cc.Camera.GetOthographicNearClip());
					cc.Camera.SetOrthographicFarClip(cameraprops["OrthographicFar"].as<float>());
					TRACE("Camera component orthographic far: {0}", cc.Camera.GetOrthographicFarClip());
					if (cameraprops["LevelHeight"])
					{
						cc.Camera.SetLevelHeight(cameraprops["LevelHeight"].as<float>());
						TRACE("Camera Component Level height: {0}", cc.Camera.GetLevelHeight());
					}
					if (cameraprops["LevelWidth"])
					{
						cc.Camera.SetLevelWidth(cameraprops["LevelWidth"].as<float>());
						TRACE("Camera Component Level width: {0}", cc.Camera.GetLevelWidth());
					}
					cc.Primary = cameracomponent["Primary"].as<bool>();
					TRACE("Camera component primary: {0}", cc.Primary);
					cc.FixedAspectRatio = cameracomponent["FixedAspectRatio"].as<bool>();
					TRACE("Camera component fixed aspect ratio: {0}", cc.FixedAspectRatio);
				}
				auto spriterenderercomponent = entity["SpriteRendererComponent"];
				if (spriterenderercomponent)
				{
					auto& src = deserialisedentity.AddComponent<SpriteRendererComponent>();
					src.Colour = spriterenderercomponent["Colour"].as<glm::vec4>();
					TRACE("SRC colour: Red {0}, Green {1}, Blue {2}, Alpha {3}", src.Colour.r, src.Colour.g, src.Colour.b, src.Colour.a);
					if (spriterenderercomponent["TexturePath"])
					{
						std::string texturepath = spriterenderercomponent["TexturePath"].as<std::string>();
						TRACE("SRC texture : {0}", texturepath);
						src.Tex = std::make_shared<Texture>(texturepath);
					}
					if (spriterenderercomponent["TilingFactor"])
					{
						src.TilingFactor = spriterenderercomponent["TilingFactor"].as<float>();
						TRACE("SRC tiling factor: {0}", src.TilingFactor);
					}
				}
				auto rigidbody2Dcomponent = entity["RigidBody2DComponent"];
				if (rigidbody2Dcomponent)
				{
					auto& rb2d = deserialisedentity.AddComponent<RigidBody2DComponent>();
					rb2d.Type = RigidBody2DTypeFromString(rigidbody2Dcomponent["BodyType"].as<std::string>());
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
					bc2d.RestitutionThreshold = boxcollider2Dcomponent["Restitution Threshold"].as<float>();
					bc2d.Category = (uint16_t)CollisionCategoryFromString(boxcollider2Dcomponent["Category"].as<std::string>());
					bc2d.Mask = (uint16_t)CollisionMaskFromString(boxcollider2Dcomponent["Mask"].as<std::string>());
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
					cc2d.RestitutionThreshold = circlecollider2Dcomponent["Restitution Threshold"].as<float>();
					cc2d.Category = (uint16_t)CollisionCategoryFromString(circlecollider2Dcomponent["Category"].as<std::string>());
					cc2d.Mask = (uint16_t)CollisionMaskFromString(circlecollider2Dcomponent["Mask"].as<std::string>());
				}
				auto clonemmingcomponent = entity["ClonemmingComponent"];
				if (clonemmingcomponent)
				{
					auto& cc = deserialisedentity.AddComponent<ClonemmingComponent>();
					TRACE("Clonemming Component status: {}", clonemmingcomponent["Clonemming Status"].as<std::string>());
					cc.Status = ClonemmingStatusFromString(clonemmingcomponent["Clonemming Status"].as<std::string>());
					if (clonemmingcomponent["Walk Speed"])
					{
						cc.WalkSpeed = clonemmingcomponent["Walk Speed"].as<float>();
						TRACE("Clonemming Component: walk speed {0}", cc.WalkSpeed);
					}
					else
					{
						cc.WalkSpeed = 1.0f;
						TRACE("no walk speed in file using 1.0");
					}
					if (clonemmingcomponent["Dig Speed"])
					{
						cc.DigSpeed = clonemmingcomponent["Dig Speed"].as<float>();
						TRACE("Clonemming Component: dig speed {0}", cc.DigSpeed);
					}
					else
					{
						cc.DigSpeed = 1.0f;
						TRACE("no dig speed in file using 1.0");
					}
					if (clonemmingcomponent["Mine Speed"])
					{
						cc.MineSpeed = clonemmingcomponent["Mine Speed"].as<float>();
						TRACE("Clonemming Component: mine speed {0}", cc.MineSpeed);
					}
					else
					{
						cc.MineSpeed = 1.0f;
						TRACE("no mine speed in file using 1.0");
					}
					m_Scene->GetGameLevelData().StartClonemmingCount++;
				}
				auto clonemmingstartcomponent = entity["ClonemmingStartComponent"];
				if (clonemmingstartcomponent)
				{
					auto& csc = deserialisedentity.AddComponent<ClonemmingStartComponent>();
					csc.NumberOfClonemmings = clonemmingstartcomponent["NumberOfClonemmings"].as<uint32_t>();
					csc.ClonemmingReleaseRate = clonemmingstartcomponent["ClonemmingReleaseRate"].as<float>();
					TRACE("Clonemming start point, number of clonemmings: {}", csc.NumberOfClonemmings);
					TRACE("Clonemming Startpoint, Release Rate: {}", csc.ClonemmingReleaseRate);
					m_Scene->GetGameLevelData().StartClonemmingCount =+ csc.NumberOfClonemmings;
				}
				auto clonemmingexitcomponent = entity["ClonemmingExitComponent"];
				if (clonemmingexitcomponent)
				{
					auto& cec = deserialisedentity.AddComponent<ClonemmingExitComponent>();
					cec.NumberOfClonemmings = clonemmingexitcomponent["NumberOfClonemmings"].as<uint32_t>();
					TRACE("Clonemming exit point, number of clonemmings: {}", cec.NumberOfClonemmings);
				}
				auto scriptcomponent = entity["ScriptComponent"];
				if (scriptcomponent)
				{
					auto& sc = deserialisedentity.AddComponent<ScriptComponent>();
					sc.ClassName = scriptcomponent["ClassName"].as<std::string>();
					auto scriptfields = scriptcomponent["ScriptFields"];
					if (scriptfields)
					{
						std::shared_ptr<ScriptClass> entityclass = ScriptEngine::GetEntityClass(sc.ClassName);
						if (entityclass)
						{
							const auto& fields = entityclass->GetFields();
							auto& entityfields = ScriptEngine::GetScriptFieldMap(deserialisedentity);
							for (auto scriptfield : scriptfields)
							{
								std::string name = scriptfield["Name"].as<std::string>();
								std::string typestring = scriptfield["Type"].as<std::string>();
								ScriptFieldType type = Utills::ScriptFieldTypeFromString(typestring);
								ScriptFieldInstance& fieldinstance = entityfields[name];
								ASSERT(fields.find(name) != fields.end());
								if (fields.find(name) == fields.end())
								{
									continue;
								}
								fieldinstance.Field = fields.at(name);
								switch (type)
								{
									READ_SCRIPT_FIELD(Float, float);
									READ_SCRIPT_FIELD(Double, double);
									READ_SCRIPT_FIELD(Bool, bool);
									READ_SCRIPT_FIELD(Byte, int8_t);
									READ_SCRIPT_FIELD(Short, int16_t);
									READ_SCRIPT_FIELD(Int, int32_t);
									READ_SCRIPT_FIELD(Long, int64_t);
									READ_SCRIPT_FIELD(UByte, uint8_t);
									READ_SCRIPT_FIELD(UShort, uint16_t);
									READ_SCRIPT_FIELD(UInt, uint32_t);
									READ_SCRIPT_FIELD(ULong, uint64_t);
									READ_SCRIPT_FIELD(Vector2, glm::vec2);
									READ_SCRIPT_FIELD(Vector3, glm::vec3);
									READ_SCRIPT_FIELD(Vector4, glm::vec4);
									READ_SCRIPT_FIELD(Entity, UUID);

								}
							}
						}
					}
				}
			}
		}
		return true;
	}
}

