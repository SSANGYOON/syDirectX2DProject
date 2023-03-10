#include "pch.h"
#include "Animation.h"
#include "Timer.h"
#include "Engine.h"

Animation::Animation()
	: _atlas(nullptr)
	, _spriteSheet{}
	, _offset{}
	, _index(-1)
	, _time(0.0f)
	, _duration(0.0f)
	, _complete(false)
{
}

Animation::~Animation()
{
}

void Animation::Update()
{
	if (_complete)
		return;

	// �ð� üũ
	_time += GET_SINGLE(Timer)->DeltaTime();

	// ���� �ð��� �ش� �������� �����ð��� �Ѿ�� �������������� �̵�
	if (_duration < _time)
	{
		_time -= _duration;
		
		if (_spriteSheet.size() <= ++_index)
		{
			_complete = true;
			_index = _spriteSheet.size() - 1;
		}
	}
}

void Animation::Render()
{
}

void Animation::Create(const wstring& name, shared_ptr<Texture> atlas, Vector2 leftTop, Vector2 size, Vector2 offset, UINT columnLegth, UINT spriteLegth, float duration)
{
	_name = name;
	_atlas = atlas;
	_offset = offset;
	for (int i = 0; i < spriteLegth; i++)
	{
		Sprite s = { size,{leftTop.x + size.x * (i % columnLegth), leftTop.y + size.y * (i / columnLegth)} };
	}
}

void Animation::BindShader()
{

}

void Animation::Reset()
{
}

void Animation::Clear()
{
}
