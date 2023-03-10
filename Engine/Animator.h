#pragma once
#include "Component.h"

class GameObject;
class Animation;
class Texture;

using AnimationEvent = function<void(GameObject&)>;
class Animator : public Component
{
public:
	Animator();
	~Animator();

	virtual void Start() override;
	virtual void Update() override;
	virtual void Render() override;

	bool Create(const wstring& name, shared_ptr<Texture> atlas
		, Vector2 leftTop, Vector2 size, Vector2 offset
		, UINT columnLegth, UINT spriteLegth, float duration);

	shared_ptr<Animation> FindAnimation(const wstring& name);
	AnimationEvent FindEvent(const wstring& name);

	void AddEvent(AnimationEvent event, const wstring& name);
	void Play(wstring& name, bool loop = true);

	void Binds();
	void Clear();

private:
	map<wstring, shared_ptr<Animation>> _animations;
	map<wstring, AnimationEvent> _events;
	shared_ptr<Animation> _activeAnimation;
	wstring t;
	bool _loop;
};

