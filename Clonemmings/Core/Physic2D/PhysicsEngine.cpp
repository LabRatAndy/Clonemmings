#include "Core/Physic2D/PhysicsEngine.h"
#include "Core/Application/Assert.h"
#include "Core/Application/Physics2D.h"
#include <box2d/b2_body.h>
#include <box2d/b2_world.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_contact.h>

#include <cmath>

namespace Clonemmings
{
	b2World* PhysicsEngine::s_PhysicsWorld = nullptr;

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

	b2Body* PhysicsEngine::CreateBody(const RigidBody2DComponent& rigidbody, const TransformComponent& transform)
	{
		b2BodyDef bodydef;
		bodydef.type = Utills::Rigidbody2DTypeToBox2DBody(rigidbody.Type);
		bodydef.position.Set(transform.Translation.x, transform.Translation.y);
		bodydef.angle = transform.Rotation.z;
		bodydef.angularDamping = 0.5f;
		ASSERT(s_PhysicsWorld, "Physics world is nullptr");
		return CreateBody(&bodydef, rigidbody.FixedRotation);
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
		ASSERT(body, "body is nullptr");
		CreateFixture(&fixturedef, body);
	}
	glm::vec2 PhysicsEngine::GetLinearVelocity(const b2Body* body)
	{
		ASSERT(body, "body is nullptr");
		b2Vec2 velocity = body->GetLinearVelocity();
		return { velocity.x,velocity.y };
	}
	void PhysicsEngine::SetLinearVelocity(b2Body* body, const glm::vec2& velocity)
	{
		ASSERT(body, "Body is nullptr");
		body->SetLinearVelocity({ velocity.x,velocity.y });
	}
	float PhysicsEngine::GetAngle(const b2Body* body)
	{
		ASSERT(body, "Body is nullptr");
		return body->GetAngle();
	}
	float PhysicsEngine::GetMass(const b2Body* body)
	{
		ASSERT(body, "Body is nullptr");
		return body->GetMass();
	}
	glm::vec2 PhysicsEngine::GetPosition(const b2Body* body)
	{
		ASSERT(body, "Body is nullptr");
		b2Vec2 position = body->GetPosition();
		return { position.x,position.y };
	}
	void PhysicsEngine::ApplyForce(b2Body* body, const glm::vec2& force, const glm::vec2& position, bool wake)
	{
		ASSERT(body, "Body is nullptr");
		body->ApplyForce({ force.x,force.y }, { position.x,position.y }, wake);
	}
	void PhysicsEngine::ApplyForceToCentre(b2Body* body, const glm::vec2& force, bool wake)
	{
		ASSERT(body, "Body is nullptr");
		body->ApplyForceToCenter({ force.x,force.y }, wake);
	}
	void PhysicsEngine::ApplyLinearImpulse(b2Body* body, const glm::vec2& impulse, const glm::vec2& position, bool wake)
	{
		ASSERT(body, "Body is nullptr");
		body->ApplyLinearImpulse({ impulse.x,impulse.y }, { position.x,position.y }, wake);
	}
	void PhysicsEngine::ApplyLinearImpulseToCentre(b2Body* body, const glm::vec2& impulse, bool wake)
	{
		ASSERT(body, "Body is nullptr");
		body->ApplyLinearImpulseToCenter({ impulse.x,impulse.y }, wake);
	}
	void PhysicsEngine::ApplyAngularImpulse(b2Body* body, const float impulse, bool wake)
	{
		ASSERT(body, "Body is nullptr");
		body->ApplyAngularImpulse(impulse, wake);
	}
	void PhysicsEngine::ApplyTorque(b2Body* body, const float torque, bool wake)
	{
		ASSERT(body, "Body is nullptr");
		body->ApplyTorque(torque, wake);
	}
	bool PhysicsEngine::IsContact(const b2Body* body)
	{
		ASSERT(body, "Body is nullptr");
		const b2ContactEdge* contactegde = body->GetContactList();
		while (contactegde != nullptr)
		{
			b2Contact* contact = contactegde->contact;
			if (contact->IsTouching())
				return true;
			contactegde = contactegde->next;
		}
		return false;
	}
	bool PhysicsEngine::IsContactBottom(const b2Body* body, const glm::vec2& size)
	{
		ASSERT(body, "Body is nullptr");
		const b2ContactEdge* contactedge = body->GetContactList();
		while (contactedge != nullptr)
		{
			b2Contact* contact = contactedge->contact;
			if (contact->IsTouching())
			{
				b2WorldManifold manifold;
				contact->GetWorldManifold(&manifold);
				// contact normal in worldmanifold is from Fixture A to fixture B 
				//indicating the direction of fixture A to fixtureB
				//therefore if our body has fixture A then to be above fixture B the normals Y value will be negative.
				//And if our body has fixtureB then to be above fixture A the normal's Y value will be positive
				// also need to check that the normal Y val is not zero as that would mean that it would be to one side of it 
				//std::signbit returns true if the value is negative!
				if (contact->GetFixtureA()->GetBody() == body)
				{
					//fixtureB is the other body
					if (std::signbit(manifold.normal.y) && std::fpclassify(manifold.normal.y) != FP_ZERO)
						return true;
				}
				else
				{
					//fixture A is the other body
					if (!std::signbit(manifold.normal.y) && std::fpclassify(manifold.normal.y) != FP_ZERO)
						return true;
				}
			}
			contactedge = contactedge->next;
		}
		return false;
	}
	bool PhysicsEngine::IsContactLeft(const b2Body* body, const glm::vec2& size)
	{
		ASSERT(body, "Body is nullptr");
		const b2ContactEdge* contactedge = body->GetContactList();
		while (contactedge != nullptr)
		{
			b2Contact* contact = contactedge->contact;
			if (contact->IsTouching())
			{
				b2WorldManifold manifold;
				contact->GetWorldManifold(&manifold);
				//see IsContactBottom for explaination of algorythem.
				if (contact->GetFixtureA()->GetBody() == body)
				{
					//fixture B is the other body
					if (std::signbit(manifold.normal.x) && std::fpclassify(manifold.normal.x) != FP_ZERO)
						return true;
				}
				else
				{
					//fixture A is the other body
					if (!std::signbit(manifold.normal.x) && std::fpclassify(manifold.normal.x) != FP_ZERO)
						return true;
				}
			}
			contactedge = contactedge->next;
		}
		return false;
	}
	bool PhysicsEngine::IsContactTop(const b2Body* body, const glm::vec2& size)
	{
		ASSERT(body, "Body is nullptr");
		const b2ContactEdge* contactedge = body->GetContactList();
		while (contactedge != nullptr)
		{
			b2Contact* contact = contactedge->contact;
			if (contact->IsTouching())
			{
				b2WorldManifold manifold;
				contact->GetWorldManifold(&manifold);
				//see IsContactBottom for explaination of algorhytem.
				if (contact->GetFixtureA()->GetBody() == body)
				{
					//fixture B is the other body
					if (!std::signbit(manifold.normal.y) && std::fpclassify(manifold.normal.y) != FP_ZERO)
						return true;
				}
				else
				{
					//fixture A is the other body
					if (std::signbit(manifold.normal.y) && std::fpclassify(manifold.normal.y) != FP_ZERO)
						return true;
				}
			}
			contactedge = contactedge->next;
		}
		return false;
	}
	bool PhysicsEngine::IsContactRight(const b2Body* body, const glm::vec2& size)
	{
		ASSERT(body, "Body is nullptr");
		const b2ContactEdge* contactedge = body->GetContactList();
		while (contactedge != nullptr)
		{
			b2Contact* contact = contactedge->contact;
			if (contact->IsTouching())
			{
				b2WorldManifold manifold;
				contact->GetWorldManifold(&manifold);
				//see IsContactBottom for explaination of algorythem.
				if (contact->GetFixtureA()->GetBody() == body)
				{
					//fixture B is the other body
					if (!std::signbit(manifold.normal.x) && std::fpclassify(manifold.normal.x) != FP_ZERO)
						return true;
				}
				else
				{
					//fixture A is the other body
					if (std::signbit(manifold.normal.x) && std::fpclassify(manifold.normal.x) != FP_ZERO)
						return true;
				}
			}
			contactedge = contactedge->next;
		}
		return false;
	}
	RigidBody2DComponent::BodyType PhysicsEngine::GetBodyType(const b2Body* body)
	{
		ASSERT(body,"body is nullptr");
		return Utills::Rigidbody2DTypeFromBox2DBody(body->GetType());
	}
	void PhysicsEngine::SetBodyType(b2Body* body, RigidBody2DComponent::BodyType bodytype)
	{
		ASSERT(body, "body is nullptr");
		body->SetType(Utills::Rigidbody2DTypeToBox2DBody(bodytype));
	}

}