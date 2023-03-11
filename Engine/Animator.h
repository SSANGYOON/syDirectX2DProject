#pragma once
#include "Component.h"

class GameObject;
class Texture;
//using AnimationEvent = function<void(GameObject&)>;

struct SpriteAnimClip
{
	Vector2 offset;
	Vector2 size;
	UINT row;
	UINT col;
	UINT frame_count;
	float duration;
};

enum
{
	MAX_ANIMATION_SIZE = 4
};
class Animator : public Component
{
public:
	Animator();
	~Animator();

	virtual void Start() override;
	virtual void FinalUpdate() override;

	void AddSpriteAnimation(UINT8 ind, const SpriteAnimClip& clip) { _animations[ind] = clip; }
	void SetSpriteSheet(shared_ptr<Texture> spriteSheet) { _spriteSheet = spriteSheet; }

	//AnimationEvent FindEvent(const wstring& name);

	//void AddEvent(AnimationEvent event, const wstring& name);
	void Play(UINT8 ind, bool loop = true);

	void SetSpriteData();
	void Clear();

private:
	shared_ptr<Texture> _spriteSheet;
	array<SpriteAnimClip, MAX_ANIMATION_SIZE> _animations;


	float _updateTime;
	UINT _clipIndex;
	UINT _frameindex;
	bool _loop;
};

