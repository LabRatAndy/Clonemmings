#pragma once

#include "Core/Renderer/Texture.h"
#include "Core/Renderer/SceneCamera.h"
#include "Core/Application/UUID.h"
#include "Game/GameComponents.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Clonemmings
{
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(const UUID& id) : ID(id) {}
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) : Tag(tag) {}
	};
	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0,0.0,0.0 };
		glm::vec3 Rotation = { 0.0,0.0,0.0 };
		glm::vec3 Scale = { 1.0,1.0,1.0 };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation) : Translation(translation) {}
		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0), Translation) * rotation * glm::scale(glm::mat4(1.0), Scale);
		}
	};
	struct SpriteRendererComponent
	{
		glm::vec4 Colour = { 1.0,1.0,1.0,1.0 };
		std::shared_ptr<Texture> Tex = nullptr;
		float TilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(glm::vec4 colour) : Colour(colour) {}
	};
	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};
	struct RectangleComponent
	{
		glm::vec3 Translation = { 0.0,0.0,0.0 };
		glm::vec3 Rotation = { 0.0,0.0,0.0 };
		glm::vec3 Scale = { 1.0,1.0,1.0 };
		glm::vec4 Colour = { 1.0,1.0,1.0,1.0 };
		float LineThickness = 1.0;

		RectangleComponent() = default;
		RectangleComponent(const RectangleComponent&) = default;
		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
			return glm::translate(glm::mat4(1.0), Translation) * rotation * glm::scale(glm::mat4(1.0), Scale);
		}
	};

	//Physiscs components
	struct RigidBody2DComponent
	{
		enum class BodyType
		{
			Static = 0,
			Dynamic,
			Kinematic
		};
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;
		void* RuntimeBody = nullptr;

		RigidBody2DComponent() = default;
		RigidBody2DComponent(const RigidBody2DComponent&) = default;
	};
	struct BoxCollider2DComponent
	{
		glm::vec2 Offset = { 0.0,0.0 };
		glm::vec2 Size = { 0.5,0.5 }; // half extents

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.5f;
		float RestitutionThreshold = 0.5f;
		uint16_t Category = 1;
		uint16_t Mask = 0xffff;

		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};
	struct CircleCollider2DComponent
	{
		glm::vec2 Offset = { 0.0,0.0 };
		float Radius = 0.5f;

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.5f;
		float RestitutionThreshold = 0.5f;
		uint16_t Category = 1;
		uint16_t Mask = 0xffff;

		void* RuntimeFixture = nullptr;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};
	struct ScriptComponent
	{
		std::string ClassName;
		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
	};

	template<typename... Component>
	struct ComponentGroup
	{

	};
	using  AllComponents = ComponentGroup<TransformComponent, SpriteRendererComponent, CameraComponent, RigidBody2DComponent, BoxCollider2DComponent,
		CircleCollider2DComponent, ScriptComponent, ClonemmingComponent, ClonemmingStartComponent, ClonemmingExitComponent, RectangleComponent>;
}