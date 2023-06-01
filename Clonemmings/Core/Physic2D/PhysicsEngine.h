#pragma once
#include "Core/Application/TimeStep.h"
#include "core/Scene/CoreComponents.h"
#include "Core/Application/Physics2D.h"
#include <glm/glm.hpp>
//fwd declarations for box2d
class b2World;
class b2Body;
class b2Fixture;
struct b2BodyDef;
class b2PolygonShape;
class b2CircleShape;
struct b2FixtureDef;
class b2Shape;

namespace Clonemmings
{
	
	class PhysicsEngine
	{
	public:
		static void Initialise(const glm::vec2& gravity);
		static void Shutdown();

		static void PhysicsWorldUpdate(TimeStep ts, int32_t veliterations = 6, int32_t positeration = 2);

		static b2Body* CreateBody(const b2BodyDef* bodydef, bool fixedroatation);
		static b2Body* CreateBody(const RigidBody2DComponent& rigidbody, const TransformComponent& transform);
		static void DestroyBody(b2Body* body);
		static b2PolygonShape CreateBoxShape(const glm::vec2& size);
		static void CreateBoxCollider(const BoxCollider2DComponent& boxcollider, const TransformComponent& transform, b2Body* body);
		static b2CircleShape CreateCircleShape(const glm::vec2& offset, float radius);
		static void CreateCircleCollider(const CircleCollider2DComponent& circlecollider, const TransformComponent& transform, b2Body* body);
		static b2Fixture* CreateFixture(const b2FixtureDef* fixturedef, b2Body* body);
		static glm::vec2 GetLinearVelocity(const b2Body* body);
		static void SetLinearVelocity(b2Body* body, const glm::vec2& velocity);
		static float GetAngle(const b2Body* body);
		static float GetMass(const b2Body* body);
		static glm::vec2 GetPosition(const b2Body* body);
		static void ApplyForceToCentre(b2Body* body, const glm::vec2& force, bool wake);
		static void ApplyForce(b2Body* body, const glm::vec2& force, const glm::vec2& position, bool wake);
		static void ApplyLinearImpulseToCentre(b2Body* body, const glm::vec2& impulse, bool wake);
		static void ApplyLinearImpulse(b2Body* body, const glm::vec2& impulse, const glm::vec2& position, bool wake);
		static void ApplyTorque(b2Body* body, const float torque, bool wake);
		static void ApplyAngularImpulse(b2Body* body, const float impulse, bool wake);
		static bool IsContact(const b2Body* body);
		static bool IsContactTop(const b2Body* body, const glm::vec2& size);
		static bool IsContactBottom(const b2Body* body, const glm::vec2& size);
		static bool IsContactRight(const b2Body* body, const glm::vec2& size);
		static bool IsContactLeft(const b2Body* body, const glm::vec2& size);
		static RigidBody2DComponent::BodyType GetBodyType(const b2Body* body);
		static void SetBodyType(b2Body* body, RigidBody2DComponent::BodyType bodytype);
		

	private:
		static b2World* s_PhysicsWorld;
	};
}