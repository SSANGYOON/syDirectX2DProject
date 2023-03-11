#pragma once
#include "Entity.h"
#include "Layer.h"
#include "Transform.h"
#include "Script.h"



class Collider;
class BaseRenderer;
class Transform;
class GameObject : public Entity, public enable_shared_from_this<GameObject>
{
public:
	enum STATE
	{
		ACTIVE,
		PAUSED,
		KILLED
	};
	GameObject();
	virtual ~GameObject();

	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FinalUpdate();
	virtual void Render();

	void OnTriggerEnter(Collider* collider);
	void OnTriggerStay(Collider* collider);
	void OnTriggerExit(Collider* collider);

	void OnCollisionEnter(Collider* collider);
	void OnCollisionStay(Collider* collider);
	void OnCollisionExit(Collider* collider);

	template <typename T>
	shared_ptr<T> AddComponent();

	STATE GetState() { return _state; }
	shared_ptr<Component> GetComponent(Component_Type type) { return _components[(UINT)type]; }
	shared_ptr<Transform> GetTransform();
	shared_ptr<BaseRenderer> GetRenderer();
private:
	friend class SceneManager;
	void SetGameObjectState(STATE state) { _state = state; }

	array<shared_ptr<Component>, (UINT)Component_Type::End> _components;
	vector<shared_ptr<Script>> _scripts;
	STATE _state;
};

template<typename T>
inline shared_ptr<T> GameObject::AddComponent()
{
	shared_ptr<T> comp = make_shared<T>();
	Component_Type order = comp->GetType();

	if (order == Component_Type::Script)
		_scripts.push_back(dynamic_pointer_cast<Script>(comp));
	else
		_components[(UINT)order] = comp;

	comp->SetOwner(shared_from_this());
	return comp;
}
