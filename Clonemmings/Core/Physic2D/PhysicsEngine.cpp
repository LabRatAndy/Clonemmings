#include "Core/Physic2D/PhysicsEngine.h"
#include "Core/Application/Assert.h"
#include "Core/Application/Physics2D.h"
#include <box2d/b2_body.h>
#include <box2d/b2_world.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_contact.h>


namespace Clonemmings
{
	b2World* PhysicsEngine::s_PhysicsWorld = nullptr;
	std::unordered_map<UUID, b2Body*> PhysicsEngine::s_BodyMap;


	void PhysicsEngine::Initialise(const glm::vec2& gravity)
	{
		s_PhysicsWorld = new b2World({ gravity.x,gravity.y });
	}

	void PhysicsEngine::Shutdown()
	{
		delete s_PhysicsWorld;
		s_PhysicsWorld = nullptr;
	}
	void PhysicsEngine::PhysicsWorldUpdate(TimeStep ts, int32_t veliterations, int32_t positerations)
	{
		ASSERT(s_PhysicsWorld, "Physics world is null! check PhysicsEngine has been initialised!");
		s_PhysicsWorld->Step(ts, veliterations, positerations);
	}
	b2Body* PhysicsEngine::CreateBody(const b2BodyDef* bodydef, bool fixedrotation)
	{
		ASSERT(bodydef, "bodydef is nullptr!");
		b2Body* retval = s_PhysicsWorld->CreateBody(bodydef);
		retval->SetFixedRotation(fixedrotation);
		return retval;
	}
	void PhysicsEngine::DestroyBody(b2Body* body)
	{
		ASSERT(body, "Body is nullptr!");
		s_PhysicsWorld->DestroyBody(body);
	}
	b2PolygonShape PhysicsEngine::CreateBoxShape(const glm::vec2& size)
	{
		b2PolygonShape shape;
		shape.SetAsBox(size.x, size.y);
		return shape;
	}
	b2CircleShape PhysicsEngine::CreateCircleShape(const glm::vec2& offset, float radius)
	{
		b2CircleShape shape;
		shape.m_p.Set(offset.x, offset.y);
		shape.m_radius = radius;
		return shape;
	}

	b2Fixture* PhysicsEngine::CreateFixture(const b2FixtureDef* fixturedef, b2Body* body)
	{
		ASSERT(fixturedef, "fixturedef is nullptr");
		return body->CreateFixture(fixturedef);
	}

	b2Body* PhysicsEngine::CreateBody(const RigidBody2DComponent& rigidbody, const TransformComponent& transform, bool fixedrotation, UUID uuid)
	{
		b2BodyDef bodydef;
		bodydef.type = Utills::Rigidbody2DTypeToBox2DBody(rigidbody.Type);
		bodydef.position.Set(transform.Translation.x, transform.Translation.y);
		bodydef.angle = transform.Rotation.z;
		bodydef.userData.pointer = (uintptr_t)uuid;
		ASSERT(s_PhysicsWorld, "Physics world is nullptr");
		return CreateBody(&bodydef, fixedrotation);
	}
	void PhysicsEngine::CreateBoxCollider(const BoxCollider2DComponent& boxcollider, const TransformComponent& transform, b2Body* body)
	{
		b2FixtureDef fixturedef;
		b2PolygonShape shape =  CreateBoxShape(glm::vec2(boxcollider.Size.x * transform.Scale.x, boxcollider.Size.y * transform.Scale.y));
		fixturedef.shape = &shape;
		fixturedef.density = boxcollider.Density;
		fixturedef.friction = boxcollider.Friction;
		fixturedef.restitution = boxcollider.Restitution;
		fixturedef.restitutionThreshold = boxcollider.RestitutionThreshold;
		fixturedef.filter.maskBits = boxcollider.Mask;
		fixturedef.filter.categoryBits = boxcollider.Category;
		ASSERT(body, "Body is nullptr");
		CreateFixture(&fixturedef, body);
	}
	void PhysicsEngine::CreateCircleCollider(const CircleCollider2DComponent& circlecollider, const TransformComponent& transform, b2Body* body)
	{
		b2FixtureDef fixturedef;
		b2CircleShape shape = CreateCircleShape(glm::vec2(circlecollider.Offset.x, circlecollider.Offset.y), transform.Scale.x * circlecollider.Radius);
		fixturedef.shape = &shape;
		fixturedef.density = circlecollider.Density;
		fixturedef.friction = circlecollider.Friction;
		fixturedef.restitution = circlecollider.Restitution;
		fixturedef.restitutionThreshold = circlecollider.RestitutionThreshold;
		fixturedef.filter.maskBits = circlecollider.Mask;
		fixturedef.filter.categoryBits = circlecollider.Category;
		ASSERT(body, "body is nullptr");
		CreateFixture(&fixturedef, body);
	}
	glm::vec2 PhysicsEngine::GetLinearVelocity(UUID uuid)
	{
		ASSERT(s_BodyMap.find(uuid) != s_BodyMap.end(), "UUID not found");
		b2Vec2 velocity = s_BodyMap.at(uuid)->GetLinearVelocity();
		return { velocity.x,velocity.y };
	}
	void PhysicsEngine::SetLinearVelocity(UUID uuid, const glm::vec2& velocity)
	{
		ASSERT(s_BodyMap.find(uuid) != s_BodyMap.end(), "UUID not found");
		s_BodyMap.at(uuid)->SetLinearVelocity({ velocity.x,velocity.y });
	}
	float PhysicsEngine::GetAngle(UUID uuid)
	{
		ASSERT(s_BodyMap.find(uuid) != s_BodyMap.end(), "UUID not found");
		return s_BodyMap.at(uuid)->GetAngle();
	}
	float PhysicsEngine::GetMass(UUID uuid)
	{
		ASSERT(s_BodyMap.find(uuid) != s_BodyMap.end(), "UUID not found");
		return s_BodyMap.at(uuid)->GetMass();
	}
	glm::vec2 PhysicsEngine::GetPosition(UUID uuid)
	{
		ASSERT(s_BodyMap.find(uuid) != s_BodyMap.end(), "UUID not found");
		b2Vec2 position = s_BodyMap.at(uuid)->GetPosition();
		return { position.x,position.y };
	}
	void PhysicsEngine::ApplyForce(UUID uuid, const glm::vec2& force, const glm::vec2& position, bool wake)
	{
		ASSERT(s_BodyMap.find(uuid) != s_BodyMap.end(), "UUID not found");
		s_BodyMap.at(uuid)->ApplyForce({ force.x,force.y }, { position.x,position.y }, wake);
	}
	void PhysicsEngine::ApplyForceToCentre(UUID uuid, const glm::vec2& force, bool wake)
	{
		ASSERT(s_BodyMap.find(uuid) != s_BodyMap.end(), "UUID not found");
		s_BodyMap.at(uuid)->ApplyForceToCenter({ force.x,force.y }, wake);
	}
	void PhysicsEngine::ApplyLinearImpulse(UUID uuid, const glm::vec2& impulse, const glm::vec2& position, bool wake)
	{
		ASSERT(s_BodyMap.find(uuid) != s_BodyMap.end(), "UUID not found");
		s_BodyMap.at(uuid)->ApplyLinearImpulse({ impulse.x,impulse.y }, { position.x,position.y }, wake);
	}
	void PhysicsEngine::ApplyLinearImpulseToCentre(UUID uuid, const glm::vec2& impulse, bool wake)
	{
		ASSERT(s_BodyMap.find(uuid) != s_BodyMap.end(), "UUID not found");
		s_BodyMap.at(uuid)->ApplyLinearImpulseToCenter({ impulse.x,impulse.y }, wake);
	}
	void PhysicsEngine::ApplyAngularImpulse(UUID uuid, const float impulse, bool wake)
	{
		ASSERT(s_BodyMap.find(uuid) != s_BodyMap.end(), "UUID not found");
		s_BodyMap.at(uuid)->ApplyAngularImpulse(impulse, wake);
	}
	void PhysicsEngine::ApplyTorque(UUID uuid, const float torque, bool wake)
	{
		ASSERT(s_BodyMap.find(uuid) != s_BodyMap.end(), "UUID not found");
		s_BodyMap.at(uuid)->ApplyTorque(torque, wake);
	}
	bool PhysicsEngine::IsContact(UUID uuid)
	{
		ASSERT(s_BodyMap.find(uuid) != s_BodyMap.end(), "UUID not found");
		const b2ContactEdge* contactegde = s_BodyMap.at(uuid)->GetContactList();
		while (contactegde != nullptr)
		{
			b2Contact* contact = contactegde->contact;
			if (contact->IsTouching())
				return true;
			contactegde = contactegde->next;
		}
		return false;
	}
	bool PhysicsEngine::IsContactBottom(UUID uuid)
	{
		ASSERT(s_BodyMap.find(uuid) != s_BodyMap.end(), "UUID not found");
		const b2ContactEdge* contactedge = s_BodyMap.at(uuid)->GetContactList();
		while (contactedge != nullptr)
		{
			b2Contact* contact = contactedge->contact;
			if (contact->IsTouching())
			{
				if (contact->GetFixtureA()->GetBody() == s_BodyMap.at(uuid))
				{
					SensorSide side = (SensorSide)contact->GetFixtureA()->GetUserData().pointer;
					if (side == SensorSide::Bottom)
						return true;
				}
				if (contact->GetFixtureB()->GetBody() == s_BodyMap.at(uuid))
				{
					SensorSide side = (SensorSide)contact->GetFixtureB()->GetUserData().pointer;
					if (side == SensorSide::Bottom)
						return true;
				}
			}
			contactedge = contactedge->next;
		}
		return false;
		
	}
	bool PhysicsEngine::IsContactLeft(UUID uuid)
	{
		ASSERT(s_BodyMap.find(uuid) != s_BodyMap.end(), "UUID not found");
		const b2ContactEdge* contactedge = s_BodyMap.at(uuid)->GetContactList();
		while (contactedge != nullptr)
		{
			b2Contact* contact = contactedge->contact;
			if (contact->IsTouching())
			{
				if (contact->GetFixtureA()->GetBody() == s_BodyMap.at(uuid))
				{
					SensorSide side = (SensorSide)contact->GetFixtureA()->GetUserData().pointer;
					if (side == SensorSide::Left)
						return true;
				}
				if (contact->GetFixtureB()->GetBody() == s_BodyMap.at(uuid))
				{
					SensorSide side = (SensorSide)contact->GetFixtureB()->GetUserData().pointer;
					if (side == SensorSide::Left)
						return true;
				}
			}
			contactedge = contactedge->next;
		}
		return false;
	}
	bool PhysicsEngine::IsContactTop(UUID uuid)
	{
		ASSERT(s_BodyMap.find(uuid) != s_BodyMap.end(), "UUID not found");
		const b2ContactEdge* contactedge = s_BodyMap.at(uuid)->GetContactList();
		while (contactedge != nullptr)
		{
			b2Contact* contact = contactedge->contact;
			if (contact->IsTouching())
			{
				if (contact->GetFixtureA()->GetBody() == s_BodyMap.at(uuid))
				{
					SensorSide side = (SensorSide)contact->GetFixtureA()->GetUserData().pointer;
					if (side == SensorSide::Top)
						return true;
				}
				if (contact->GetFixtureB()->GetBody() == s_BodyMap.at(uuid))
				{
					SensorSide side = (SensorSide)contact->GetFixtureB()->GetUserData().pointer;
					if (side == SensorSide::Top)
						return true;
				}
			}
			contactedge = contactedge->next;
		}
		return false;
	}
	bool PhysicsEngine::IsContactRight(UUID uuid)
	{
		ASSERT(s_BodyMap.find(uuid) != s_BodyMap.end(), "UUID not found");
		const b2ContactEdge* contactedge = s_BodyMap.at(uuid)->GetContactList();
		while (contactedge != nullptr)
		{
			b2Contact* contact = contactedge->contact;
			if (contact->IsTouching())
			{
				if (contact->GetFixtureA()->GetBody() == s_BodyMap.at(uuid))
				{
					SensorSide side = (SensorSide)contact->GetFixtureA()->GetUserData().pointer;
					if (side == SensorSide::Right)
						return true;
				}
				if (contact->GetFixtureB()->GetBody() == s_BodyMap.at(uuid))
				{
					SensorSide side = (SensorSide)contact->GetFixtureB()->GetUserData().pointer;
					if (side == SensorSide::Right)
						return true;
				}
			}
			contactedge = contactedge->next;
		}
		return false;
	}
	RigidBody2DComponent::BodyType PhysicsEngine::GetBodyType(UUID uuid)
	{
		ASSERT(s_BodyMap.find(uuid) != s_BodyMap.end(), "UUID not found");
		return Utills::Rigidbody2DTypeFromBox2DBody(s_BodyMap.at(uuid)->GetType());
	}
	void PhysicsEngine::SetBodyType(UUID uuid, RigidBody2DComponent::BodyType bodytype)
	{
		ASSERT(s_BodyMap.find(uuid) != s_BodyMap.end(), "UUID not found");
		s_BodyMap.at(uuid)->SetType(Utills::Rigidbody2DTypeToBox2DBody(bodytype));
	}

	b2PolygonShape PhysicsEngine::CreateBoxShape(const glm::vec2& size, const glm::vec2& centre, float angle)
	{
		b2PolygonShape shape; 
		shape.SetAsBox(size.x, size.y, b2Vec2(centre.x, centre.y), angle);
		return shape;
	}
	void PhysicsEngine::AddSensorToBody(b2Body* body, const glm::vec2& size, const glm::vec2& centre, float angle, const SensorSide side)
	{
		ASSERT(body, "body is nullptr!");
		b2PolygonShape shape = CreateBoxShape(size, centre, angle);
		b2FixtureDef fdef;
		fdef.shape = &shape;
		fdef.isSensor = true;
		fdef.filter.categoryBits = CollisionCategory::Clonemming;
		fdef.filter.maskBits = CollisionMasks::Clonemmings | CollisionMasks::Scenary;
		if (side != SensorSide::None)
			fdef.userData.pointer = (uintptr_t)side;
		CreateFixture(&fdef, body);
	}
	void PhysicsEngine::AddPhysicsEntity(Entity entity)
	{
		ASSERT(entity, "Entity is not valid");
		ASSERT(entity.HasComponent<RigidBody2DComponent>(), "Entity must have a rigidbody2D component to be added to physics");
		const auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		const auto& transform = entity.GetComponent<TransformComponent>();
		b2Body* body = CreateBody(rb2d, transform, false, entity.GetUUID());
		ASSERT(body, "Failed to create b2Body");
		if (entity.HasComponent<BoxCollider2DComponent>()) 
		{
			const auto& b2dc = entity.GetComponent<BoxCollider2DComponent>();
			CreateBoxCollider(b2dc, transform, body);
		}
		else if (entity.HasComponent<CircleCollider2DComponent>())
		{
			const auto& c2dc = entity.GetComponent<CircleCollider2DComponent>();
			CreateCircleCollider(c2dc, transform, body);
		}
		else
			ASSERT(false, "Entity must have either a box or a circle collider component to be added to physics");
		ASSERT(!(s_BodyMap.find(entity.GetUUID()) != s_BodyMap.end()), "Entity has already been added to physics");
		s_BodyMap[entity.GetUUID()] = body;

	}
	void PhysicsEngine::AddClonemming(Entity entity)
	{
		ASSERT(entity, "Entity is not valid");
		ASSERT(entity.HasComponent<ClonemmingComponent>(), "Entity must have a Clonemming component to be added as a clonemming");
		ASSERT(entity.HasComponent<RigidBody2DComponent>(), "Entity must have a rigidbody2D component to be added as a clonemming");
		const auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
		const auto& transform = entity.GetComponent<TransformComponent>();
		b2Body* body = CreateBody(rb2d, transform, true, entity.GetUUID());
		ASSERT(body, "Failed to create b2Body");
		ASSERT(entity.HasComponent<BoxCollider2DComponent>(), "Entity must have a box collider component to be added as a clonemming");
		const auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
		CreateBoxCollider(bc2d, transform, body);
		//add sensors to left right top bottom
		glm::vec2 sensorsize = { (bc2d.Size.x * 0.8f) * transform.Scale.x,(bc2d.Size.y * 0.5f) * transform.Scale.y };
		glm::vec2 sensorcentre = { 0.0f,-bc2d.Size.y };
		AddSensorToBody(body, sensorsize,sensorcentre, 0, SensorSide::Bottom);
		sensorcentre = { 0.0f,bc2d.Size.y };
		AddSensorToBody(body, sensorsize, sensorcentre, 0.0f, SensorSide::Top);
		sensorsize = { (bc2d.Size.x * 0.25f) * transform.Scale.x,(bc2d.Size.y * 0.5f) * transform.Scale.y };
		sensorcentre = { -(bc2d.Size.x * 0.75f),0.0f };
		AddSensorToBody(body, sensorsize, sensorcentre, 0.0f, SensorSide::Left);
		sensorcentre = { (bc2d.Size.x * 0.75f), 0.0f };
		AddSensorToBody(body, sensorsize, sensorcentre, 0.0f, SensorSide::Right);
		ASSERT(!(s_BodyMap.find(entity.GetUUID()) != s_BodyMap.end()), "Clonemming entity has already been added to physics");
		s_BodyMap[entity.GetUUID()] = body;
	}
	void PhysicsEngine::RemovePhysicsEntity(UUID uuid)
	{
		ASSERT(s_BodyMap.find(uuid) != s_BodyMap.end(), "Entity has not been added to physics!");
		DestroyBody(s_BodyMap.at(uuid));
		s_BodyMap.erase(uuid);
	}
	UUID PhysicsEngine::GetContactUUID(UUID uuid)
	{
		ASSERT(s_BodyMap.find(uuid) != s_BodyMap.end(), "Entity has not been added to physics!");
		const b2ContactEdge* contactedge = s_BodyMap.at(uuid)->GetContactList();
		while (contactedge != nullptr)
		{
			b2Contact* contact = contactedge->contact;
			if (contact->IsTouching())
			{
				if (contact->GetFixtureA()->GetBody() == s_BodyMap.at(uuid))
				{
					//fixture A is the test body return fixture B's uuid stored in body userdata
					UUID retval = (UUID)contact->GetFixtureB()->GetBody()->GetUserData().pointer;
					return retval;
				}
				if (contact->GetFixtureB()->GetBody() == s_BodyMap.at(uuid))
				{
					//fixture B is the test body return fixture A's uuid stored in body userdata
					UUID retval = (UUID)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
					return retval;
				}
			}
			contactedge = contactedge->next;
		}
		return UUID(0);
	}
	
}