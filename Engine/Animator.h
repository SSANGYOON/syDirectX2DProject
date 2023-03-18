#pragma once
#include "Component.h"

class Material;
struct AnimationEvent
{
	float time;
	function<void()> func;
};

class AnimationClip
{
public:
	AnimationClip(float duration, bool loop);
	virtual void Update(Material* material, float lastTime, float currentime) = 0;
	
	void SetCompleteEvent(function<void()> completeEvent) { _completeEvent = completeEvent; }

	void addEvent(float time, function<void()> func) { _events.push_back({ time, func }); }
	const vector<AnimationEvent>& getEvents() { return _events; }

	bool IsLooping() { return _loop; }

public:
	friend class AnimationClip;
	const float _duration;
	const bool _loop;

	function<void()> _completeEvent;

	vector<AnimationEvent> _events;
	wstring nextAnim;
};

class AnimationClip2D : public AnimationClip
{
public:
	AnimationClip2D(Vector2 offset, Vector2 size, Vector2 step, UINT row, UINT frame, float duration, bool loop);
	virtual void Update(Material* material, float lastTime, float currentime) override;

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
		FLOAT_LESS
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

	Animator();
	~Animator();

	virtual void Start() override;
	virtual void Update() override;

	void Play(const wstring& clip);
	void Clear();

	void AddAnimClip(wstring name, shared_ptr<AnimationClip> clip) { _animations[name] = clip; }
	void AddTransitionRule(const vector<Condition>& conditions, const wstring& start, const wstring& target) { _transitionRules[start].push_back({conditions, target}); }

	const float GetFloatCondition(const wstring& name);
	const bool GetBooleanCondition(const wstring& name);

	void SetFloatCondition(const wstring& name, float* con) { _floatConditions[name] = con;}
	void SetBooleanCondition(const wstring& name, bool* con) { _boolConditions[name] = con;}

	bool checkTransitionRule(const TransitionRule& rule);

private:
	shared_ptr<Material> _material;

	map<wstring, shared_ptr<AnimationClip>> _animations;
	map<wstring, vector<TransitionRule>> _transitionRules;

	map<wstring, float*> _floatConditions;
	map<wstring, bool*> _boolConditions;

	float _currentTime;
	float _lastTime;
	shared_ptr<AnimationClip> _currentClip;
	wstring _currentKey;
};

