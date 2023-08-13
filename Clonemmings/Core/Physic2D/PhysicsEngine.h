#pragma once
#include "Core/Application/TimeStep.h"
#include "core/Scene/CoreComponents.h"
#include "Core/Application/Physics2D.h"
#include "Core/Application/UUID.h"
#include "Core/Scene/Entity.h"
#include <glm/glm.hpp>
#include <unordered_map>
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
	//collision Cateogories (I am a ...
	enum CollisionCategory : uint16_t
	{
		Default = 1 << 0,
		Clonemming = 1 << 1,
		SpawnPoint = 1 << 2,
		ExitPoint = 1 << 3,
		Wall = 1 << 4,
		Floor = 1 << 5,
		MineableWall = 1 << 6,
		DigableFloor = 1 << 7,
		ClonemmingLeftSensor = 1 << 8,
		ClonemmingRightSensor = 1 << 9,
		ClonemmingTopSensor = 1 << 10,
		ClonemmingBottomSensor = 1 << 11
	};
	//collision masks (Say I can collide with...)
	enum CollisionMasks : uint16_t
	{
		Scenary = CollisionCategory::Floor | CollisionCategory::Wall | CollisionCategory::DigableFloor | CollisionCategory::MineableWall,
		Clonemmings = CollisionCategory::Clonemming,
		ClonemmingSensorSide = CollisionCategory::ClonemmingLeftSensor | ClonemmingRightSensor,
		ClonemmingSensorTop = CollisionCategory::ClonemmingTopSensor,
		ClonemmingSensorBottom = CollisionCategory::ClonemmingBottomSensor,
		ClonemmingSensorAny = CollisionCategory::ClonemmingBottomSensor | CollisionCategory::ClonemmingTopSensor | CollisionCategory::ClonemmingLeftSensor | CollisionCategory::ClonemmingRightSensor,
		Everything = 0xFFFF,
		Nothing = 0x0000
	};
	enum class SensorSide
	{
		None = 0,
		Left,
		Right,
		Top,
		Bottom,
		Whole	//sensor is the whole of the entity used for the exit point to detect the clonemming  colliding/ reaching it!.
	};

	class PhysicsEngine
	{
	public:
		static void Initialise(const glm::vec2& gravity);
		static void Shutdown();

		static void PhysicsWorldUpdate(TimeStep ts, int32_t veliterations = 6, int32_t positeration = 2);

		static void AddClonemming(Entity entity);
		static void AddPhysicsEntity(Entity entity);
		static void RemovePhysicsEntity(UUID uuid);


		//Keep as public functions , but need to remove dependency on b2Body pointers outside of this calls may be use  uuids to identify entity bodies 
		static glm::vec2 GetLinearVelocity(UUID uuid);
		static void SetLinearVelocity(UUID uuid, const glm::vec2& velocity);
		static float GetAngle(UUID uuid);
		static float GetMass(UUID uuid);
		static glm::vec2 GetPosition(UUID uuid);
		static void ApplyForceToCentre(UUID uuid, const glm::vec2& force, bool wake);
		static void ApplyForce(UUID uuid, const glm::vec2& force, const glm::vec2& position, bool wake);
		static void ApplyLinearImpulseToCentre(UUID uuid, const glm::vec2& impulse, bool wake);
		static void ApplyLinearImpulse(UUID uuid, const glm::vec2& impulse, const glm::vec2& position, bool wake);
		static void ApplyTorque(UUID uuid, const float torque, bool wake);
		static void ApplyAngularImpulse(UUID uuid, const float impulse, bool wake);
		static bool IsContact(UUID uuid);
		static bool IsContactTop(UUID uuid);
		static bool IsContactBottom(UUID uuid);
		static bool IsContactRight(UUID uuid);
		static bool IsContactLeft(UUID uuid);
		static RigidBody2DComponent::BodyType GetBodyType(UUID uuid);
		static void SetBodyType(UUID uuid, RigidBody2DComponent::BodyType bodytype);
		static UUID GetContactUUID(UUID uuid, SensorSide side = SensorSide::None);
		static UUID GetBottomContactUUID(UUID uuid);
		static UUID GetLeftContactUUID(UUID uuid);
		static UUID GetRightContactUUID(UUID uuid);

		

	private:
		static b2World* s_PhysicsWorld;
		static std::unordered_map<UUID, b2Body*> s_BodyMap;


		//Fix me will have broken things by makeing these methods private
		static b2Body* CreateBody(const b2BodyDef* bodydef, bool fixedroatation);
		static b2Body* CreateBody(const RigidBody2DComponent& rigidbody, const TransformComponent& transform, bool fixedrotation = false, UUID uuid = UUID(0));
		static void DestroyBody(b2Body* body);
		static b2PolygonShape CreateBoxShape(const glm::vec2& size);
		static void CreateBoxCollider(const BoxCollider2DComponent& boxcollider, const TransformComponent& transform, b2Body* body);
		static b2CircleShape CreateCircleShape(const glm::vec2& offset, float radius);
		static void CreateCircleCollider(const CircleCollider2DComponent& circlecollider, const TransformComponent& transform, b2Body* body);
		static b2Fixture* CreateFixture(const b2FixtureDef* fixturedef, b2Body* body);
		static b2PolygonShape CreateBoxShape(const glm::vec2& size, const glm::vec2& centre, float angle);
		static void AddSensorToBody(b2Body* body, const glm::vec2&, const glm::vec2& centre, float angle, const SensorSide side = SensorSide::None);
	};
}