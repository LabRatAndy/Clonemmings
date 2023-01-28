#pragma once

#include "Core/Renderer/Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Clonemmings
{
	class SceneCamera;
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
	struct SpriterendererComponent
	{
		glm::vec4 Colour = { 1.0,1.0,1.0,1.0 };
		std::shared_ptr<Texture> Tex = nullptr;
		float TilingFactor = 1.0f;

		SpriterendererComponent() = default;
		SpriterendererComponent(const SpriterendererComponent&) = default;
		SpriterendererComponent(glm::vec4 colour) : Colour(colour) {}
	};
	struct CameraComponent
	{
		std::shared_ptr<SceneCamera> Camera = nullptr;
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};
}