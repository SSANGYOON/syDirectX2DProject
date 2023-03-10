#pragma once
#include "Resource.h"
class Texture;
class Animation
{
	struct Sprite
	{
		Vector2 _leftTop;	// ���� ��� ��ǥ
		Vector2 _size;		// ������ܺ��� �߶� ���� ������ �ȼ� ����

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
	Vector2 _offset;		// ������ ��ġ �����ϱ����� ��ǥ
	int _index;
	float _time;
	float _duration;
	bool _complete;
};

