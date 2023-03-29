#pragma once
#include "Entity.h"
#include "Layer.h"
#include "Transform.h"

class Collider;
class BaseRenderer;
class Transform;
class GameObject : public Entity
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
	virtual void FinalUpdate();
	virtual void Render();

	void OnTriggerEnter(Collider* collider);
	void OnTriggerStay(Collider* collider);
	void OnTriggerExit(Collider* collider);

	void OnCollisionEnter(Collider* collider);
	void OnCollisionStay(Collider* collider);
	void OnCollisionExit(Collider* collider);

	void SetType(LAYER_TYPE type) { _type = type; }
	LAYER_TYPE GetType() { return _type; }

	template <typename T>
	T* AddComponent();

	STATE GetState() { return _state; }

	template <typename T>
	T* GetComponent();

	template<typename T>
	const vector<T*> GetComponents();

	Transform* GetTransform();
	BaseRenderer* GetRenderer();
private:
	friend class SceneManager;
	void SetGameObjectState(STATE state) { _state = state; }
	unique_ptr<Transform> _transform;
	vector<unique_ptr<Component>> _components;
	LAYER_TYPE _type;
	STATE _state;
};

template<typename T>
inline T* GameObject::AddComponent()
{
	_components.push_back(make_unique<T>(this));
	_components.back()->SetOwner(this);

	return static_cast<T*>(_components.back().get());
}

template<typename T>
inline T* GameObject::GetComponent()
{
	for (const auto& comp : _components)
	{
		T* tcomp = dynamic_cast<T*>(comp.get());
		if (tcomp)
			return tcomp;
	}
	return nullptr;
}

template<typename T>
inline const vector<T*> GameObject::GetComponents()
{
	vector<T*> vec;
	for (const auto& comp : _components)
	{
		T* tcomp = dynamic_cast<T*>(comp.get());
		if (tcomp)
			vec.push_back(tcomp);
	}
	return vec;
}
