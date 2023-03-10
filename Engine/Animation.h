#pragma once
#include "Resource.h"
class Texture;
class Animation
{
	struct Sprite
	{
		Vector2 _leftTop;	// 좌측 상단 좌표
		Vector2 _size;		// 좌측상단부터 잘라낼 가로 세로의 픽셀 길이

		Sprite(Vector2 leftTop, Vector2 size)
			: _leftTop(leftTop)
			, _size(size)
		{

		}
	};

public:
	Animation();
	~Animation();

	void Update();
	void Render();

	void Create(const wstring& name, shared_ptr<Texture> atlas
		, Vector2 leftTop, Vector2 size, Vector2 offset
		, UINT columnLegth, UINT spriteLegth, float duration);
	void BindShader();
	void Reset();
	void Clear();

	bool IsComplete() { return _complete; }
	wstring& GetName() { return _name; }

private:
	wstring _name;
	shared_ptr<Texture> _atlas;
	vector<Sprite> _spriteSheet;
	Vector2 _offset;		// 렌더링 위치 조정하기위한 좌표
	int _index;
	float _time;
	float _duration;
	bool _complete;
};

