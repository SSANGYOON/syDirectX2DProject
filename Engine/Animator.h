#pragma once
#include "Component.h"

class SpriteRenderer;
struct AnimationEvent
{
	float time;
	function<void()> func;
};

class AnimationClip
{
public:
	AnimationClip(float duration, bool loop);
	virtual void Update(SpriteRenderer* spriteRenderer, float lastTime, float currentime) = 0;
	void SetCompleteEvent(function<void()> completeEvent) { _completeEvent = completeEvent; }

	void addEvent(float time, function<void()> func) { _events.push_back({ time, func }); }
	const vector<AnimationEvent>& getEvents() { return _events; }

	bool IsLooping() { return _loop; }

public:
	friend class Animator;
	const float _duration;
	const bool _loop;

	function<void()> _completeEvent;

	vector<AnimationEvent> _events;
};

class AnimationClip2D : public AnimationClip
{
public:
	AnimationClip2D(Vector2 offset, Vector2 size, Vector2 step, UINT row, UINT frame, float duration, bool loop);
	virtual void Update(SpriteRenderer* material, float lastTime, float currentime) override;

private:
	Vector2 _offset;
	Vector2 _size;
	Vector2 _step;
	UINT _row;
	UINT _frame;
};

class Animator : public Component
{
public:
	enum class ConditionType
	{
		BOOL,
		FLOAT_GREATER,
		FLOAT_LESS,
		FLOAT_ABS_GREATER,
		FLOAT_ABS_LESS,
	};

	struct Condition
	{
		ConditionType type;
		wstring name;
		float value;
	};

	struct TransitionRule
	{
		vector<Condition> conditions;
		wstring targetAnimation;
	};

	Animator(class GameObject* owner);
	~Animator();

	virtual void Start() override;
	virtual void Update() override;

	void Play(const wstring& clip);
	void Clear();

	void AddAnimClip(wstring name, shared_ptr<AnimationClip> clip) { _animations[name] = clip; }
	void LoadAnimation2dFromJson(const string& jsonFile);
	shared_ptr<AnimationClip> GetClip(const wstring& name) { return _animations[name]; }

private:
	SpriteRenderer* _spriteRenderer;
	map<wstring, shared_ptr<AnimationClip>> _animations;

	float _currentTime;
	float _lastTime;
	shared_ptr<AnimationClip> _currentClip;
	wstring _currentKey;
};

