#include "pch.h"
#include "CollisionManager.h"
#include "SceneManager.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_contact.h"
#include "Entity.h"
#include "Component.h"
#include "ScriptEngine.h"

namespace SY {
	void CollisionListener::BeginContact(b2Contact* contact)
	{

		b2Fixture* f1 = contact->GetFixtureA();
		b2Fixture* f2 = contact->GetFixtureB();

		uintptr_t p1 = f1->GetUserData().pointer;
		uintptr_t p2 = f2->GetUserData().pointer;

		SY::Entity entity1 = { (entt::entity)p1, ScriptEngine::GetSceneContext()};
		SY::Entity entity2 = { (entt::entity)p2, ScriptEngine::GetSceneContext() };

		string tag1 = entity1.GetComponent<SY::TagComponent>().Tag;
		string tag2 = entity2.GetComponent<SY::TagComponent>().Tag;

		if (entity1.HasComponent<ScriptComponent>())
		{
			const b2Filter& filter = f2->GetFilterData();
			Collision col = {};
			col.entityID = entity2.GetUUID();
			col.CollisionLayer =  filter.categoryBits;
			if (f1->IsSensor())
				ScriptEngine::OnTriggerEnter(entity1, &col);
			else
				ScriptEngine::OnCollisionEnter(entity1, &col);
		}
		if (entity2.HasComponent<ScriptComponent>())
		{
			const b2Filter& filter = f1->GetFilterData();
			Collision col = {};
			col.entityID = entity1.GetUUID();
			col.CollisionLayer = filter.categoryBits;
			if (f1->IsSensor())
				ScriptEngine::OnTriggerEnter(entity2, &col);
			else
				ScriptEngine::OnCollisionEnter(entity2, &col);
		}
	}

	void CollisionListener::EndContact(b2Contact* contact)
	{
		b2Fixture* f1 = contact->GetFixtureA();
		b2Fixture* f2 = contact->GetFixtureB();

		uintptr_t p1 = f1->GetUserData().pointer;
		uintptr_t p2 = f2->GetUserData().pointer;

		SY::Entity entity1 = { (entt::entity)p1, ScriptEngine::GetSceneContext() };
		SY::Entity entity2 = { (entt::entity)p2, ScriptEngine::GetSceneContext() };

		string tag1 = entity1.GetComponent<SY::TagComponent>().Tag;
		string tag2 = entity2.GetComponent<SY::TagComponent>().Tag;

		if (entity1.HasComponent<ScriptComponent>())
		{
			const b2Filter& filter = f2->GetFilterData();
			Collision col = {};
			col.entityID = entity2.GetUUID();
			col.CollisionLayer = filter.categoryBits;
			if (f1->IsSensor())
				ScriptEngine::OnTriggerExit(entity1, &col);
			else
				ScriptEngine::OnCollisionExit(entity1, &col);
		}
		if (entity2.HasComponent<ScriptComponent>())
		{
			const b2Filter& filter = f1->GetFilterData();
			Collision col = {};
			col.entityID = entity1.GetUUID();
			col.CollisionLayer = filter.categoryBits;
			if (f1->IsSensor())
				ScriptEngine::OnTriggerExit(entity2, &col);
			else
				ScriptEngine::OnCollisionExit(entity2, &col);
		}
	}
}