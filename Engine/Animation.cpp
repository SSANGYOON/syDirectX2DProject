#include "pch.h"
#include "Animation.h"
#include "SerializerUtils.h"

Animation::Animation()
    :Resource(RESOURCE_TYPE::SPRITEANIMATION_CLIP)
{
}

Animation::~Animation()
{
}

HRESULT Animation::Load(const std::wstring& path, bool stockObject)
{
    return E_NOTIMPL;
}

HRESULT Animation::Load(const Vector2 offset, const Vector2 size, const Vector2 step, const Vector2 targetOffset, UINT columns, UINT frame, float duration, string key, bool loop, string nextKey)
{
    _offset = offset;
    _size = size;
    _step = step;
    _targetOffset = targetOffset;
    _columns = columns;
    _frame = frame;
    _duration = duration;
    _key = key;
    _loop = loop;
    _nextKey = nextKey;

    return S_OK;
}

TransformAnimation::TransformAnimation()
    :Resource(RESOURCE_TYPE::SPRITEANIMATION_CLIP)
{
}

TransformAnimation::~TransformAnimation()
{
}

HRESULT TransformAnimation::Load(const std::wstring& path, bool stockObject)
{
    return E_NOTIMPL;
}
