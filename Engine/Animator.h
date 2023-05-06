class Animation;
class TransformAnimation;
namespace SY
{
	struct SpriteAnimatorComponent
	{
		float _currentTime = 0.f;
		float _lastTime = -1.f;
		float _endedAt = 0.f;
		shared_ptr<Animation> _currentClip;
		map<string, shared_ptr<Animation>> clips;
		map<string, string> _startEvent;
		map<string, string> _endEvent;
	};

	struct TransformAnimatorComponent
	{
		float _currentTime = 0.f;
		float _lastTime = -1.f;
		float _endedAt = 0.f;
		float _transitionTime = 0.f;
		float _transitionElapsed = 0.f;
		shared_ptr<TransformAnimation> _currentClip;
		map<string, shared_ptr<TransformAnimation>> clips;
	};
}

