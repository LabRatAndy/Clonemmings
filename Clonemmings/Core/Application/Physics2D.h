#pragma once
#include "Core/Scene/CoreComponents.h"
#include "Core/Application/Assert.h"
#include <box2d/b2_body.h>
namespace Clonemmings
{
	namespace Utills
	{
		inline b2BodyType Rigidbody2DTypeToBox2DBody(RigidBody2DComponent::BodyType bodytype)
		{
			switch (bodytype)
			{
			case RigidBody2DComponent::BodyType::Static: return b2BodyType::b2_staticBody;
			case RigidBody2DComponent::BodyType::Dynamic: return b2BodyType::b2_dynamicBody;
			case RigidBody2DComponent::BodyType::Kinematic: return b2BodyType::b2_kinematicBody;
			}
			ASSERT(false, "Unknown body type!");
			return b2_staticBody;
		}
		inline RigidBody2DComponent::BodyType Rigidbody2DTypeFromBox2DBody(b2BodyType bodytype)
		{
			switch (bodytype)
			{
			case b2BodyType::b2_staticBody: return RigidBody2DComponent::BodyType::Static;
			case b2BodyType::b2_dynamicBody: return RigidBody2DComponent::BodyType::Dynamic;
			case b2BodyType::b2_kinematicBody: return RigidBody2DComponent::BodyType::Kinematic;
			}
			ASSERT(false, "Unknown body type");
			return RigidBody2DComponent::BodyType::Static;
		}
	}
}
