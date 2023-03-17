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

	template <typename T>
	shared_ptr<T> GetComponent();

	template<typename T>
	const vector<shared_ptr<T>> GetComponents();

	shared_ptr<Transform> GetTransform();
	shared_ptr<BaseRenderer> GetRenderer();
private:
	friend class SceneManager;
	void SetGameObjectState(STATE state) { _state = state; }

	vector<shared_ptr<Component>> _components;
	STATE _state;
};

template<typename T>
inline shared_ptr<T> GameObject::AddComponent()
{
	shared_ptr<T> comp = make_shared<T>();
	Component_Type order = comp->GetType();

	_components.push_back(dynamic_pointer_cast<T>(comp));
	comp->SetOwner(shared_from_this());

	return comp;
}

template<typename T>
inline shared_ptr<T> GameObject::GetComponent()
{
	for (const auto& comp : _components)
	{
		shared_ptr<T> tcomp = dynamic_pointer_cast<T>(comp);
		if (tcomp)
			return tcomp;
	}
	return nullptr;
}

template<typename T>
inline const vector<shared_ptr<T>> GameObject::GetComponents()
{
	vector<shared_ptr<T>> vec;
	for (const auto& comp : _components)
	{
		shared_ptr<T> tcomp = dynamic_pointer_cast<T>(comp);
		if (tcomp)
		{
			vec.push_back(tcomp);
		}
	}
	return vec;
}
