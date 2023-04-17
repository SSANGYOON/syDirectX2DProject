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
	AnimationClip(float duration, bool loop, wstring nextKey);
	virtual void Update(GameObject* obj);
	void SetCompleteEvent(function<void()> completeEvent) { _completeEvent = completeEvent; }
	void SetEndEvent(function<void()> endevent) { _endEvent = endevent; }
	void addEvent(float time, function<void()> func) { _events.push_back({ time, func }); }
	const vector<AnimationEvent>& getEvents() { return _events; }

	bool IsLooping() { return _loop; }
	float GetDuration() { return _duration; }

public:
	friend class Animator;
	const float _duration;
	const bool _loop;

	function<void()> _completeEvent;
	function<void()> _endEvent;

	wstring _nextKey;
	vector<AnimationEvent> _events;
};

class AnimationClip2D : public AnimationClip
{
public:
	AnimationClip2D(Vector2 offset, Vector2 size, Vector2 step, UINT row, UINT frame, Vector2 targetOffset, float duration, bool loop, wstring nextKey = L"");
	virtual void Update(GameObject* obj) override;

private:
	Vector2 _offset;
	Vector2 _size;
	Vector2 _step;
	Vector2 _targetOffset;
	UINT _row;
	UINT _frame;
};

enum TRANSFORM_COMPONENT
{
	SCALE = 0,
	ROTATION = 1,
	TRANSLATION = 2
};

struct TransformStruct
{
	Vector3 Position;
	Vector3 Scale;
	float Rotation;
};
class TransformClip : public AnimationClip
{
public:
	TransformClip(const Json::Value& value, float duration, bool loop, wstring nextKey = L"");
	virtual void Update(GameObject* obj) override;
	void SetCompleteEvent(function<void()> completeEvent) { _completeEvent = completeEvent; }

	void addEvent(float time, function<void()> func) { _events.push_back({ time, func }); }
	const vector<AnimationEvent>& getEvents() { return _events; }

	void GetCurrentTransform(GameObject* obj, float lastTime, float currentime, OUT TransformStruct& t);
	bool IsLooping() { return _loop; }

private:
	vector<TransformStruct> transforms;
	bitset<4> _transformValid;
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
		TRIGGER
	};

	struct Condition
	{
		ConditionType type;
		wstring name;
		float value;
	};

	Animator(class GameObject* owner);
	~Animator();

	virtual void Start() override;
	virtual void Update() override;

	void Play(const wstring& clip, float Time = 0.f, float Transition = 0.f);
	void Stop();
	void Clear();

	void AddAnimClip(wstring name, shared_ptr<AnimationClip> clip) { _animations[name] = clip; }
	void LoadAnimation2dFromJson(const string& jsonFile);
	void LoadTransformFromJson(const string& jsonFile);
	shared_ptr<AnimationClip> GetClip(const wstring& name) { return _animations[name]; }

	float GetLastTime() { return _lastTime; }
	float GetClipTime() { return _currentTime; }

	float GetTransitionLength() { return _transitionTime; }
	float GetTransitionElapsed() { return _transitionElapsed; }

	AnimationClip* GetPrevClip() { return _prevClip.get(); }


private:
	SpriteRenderer* _spriteRenderer;
	map<wstring, shared_ptr<AnimationClip>> _animations;

public:
	float _currentTime;
	float _lastTime;
	float _endedAt;
	float _transitionTime;
	float _transitionElapsed;
	shared_ptr<AnimationClip> _prevClip;
	shared_ptr<AnimationClip> _currentClip;
	wstring _currentKey;
};

