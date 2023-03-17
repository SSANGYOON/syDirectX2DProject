#include "pch.h"
#include "Animator.h"
#include "Timer.h"
#include "Material.h"
#include "GameObject.h"
#include "BaseRenderer.h"
#include "Texture.h"

Animator::Animator()
    :Component(Component_Type::Animator)
    ,_frameindex(-1)
    ,_clipIndex(-1)
    ,_updateTime(0.f)
    ,_loop(false)
    , _animations{}
{
}

Animator::~Animator()
{
}

void Animator::Start()
{
}

void Animator::FinalUpdate()
{
    _updateTime += GET_SINGLE(Timer)->DeltaTime();
    if (_clipIndex < 0)
        return;
    if (_updateTime >= _animations[_clipIndex].duration && _loop)
        _updateTime -= _animations[_clipIndex].duration;
    else if(_updateTime >= _animations[_clipIndex].duration)
        _updateTime = _animations[_clipIndex].duration;

    shared_ptr<Material> material = _owner.lock()->GetComponent<BaseRenderer>()->GetMaterial();
    material->SetVec2(2, material->GetTexture(0)->GetSize());
    SetSpriteData();
}

/*AnimationEvent Animator::FindEvent(const wstring& name)
{
    if (_events.find(name) != _events.end())
        return _events.find(name)->second;
    else
        return nullptr;
}

void Animator::AddEvent(AnimationEvent event, const wstring& name)
{
    _events[name] = event;
}*/

void Animator::Play(UINT8 ind, bool loop)
{
    //Animation* prevAnimation = _activeAnimation.get();
    //AnimationEvent event= FindEvent(prevAnimation->GetName());

    //if (event)
      //  event(*GetOwner());

    _clipIndex = ind;
    _updateTime = 0.f;
    _loop = loop;
}

void Animator::SetSpriteData()
{
    if (_clipIndex < 0)
        return;

    UINT curCount = (UINT)(_updateTime / _animations[_clipIndex].duration * _animations[_clipIndex].frame_count);

    Vector2 slice = _animations[_clipIndex].size;
    UINT col = _animations[_clipIndex].col;

    Vector2 LT = _animations[_clipIndex].offset +
        Vector2((curCount % col) * slice.x,
                (curCount / col) * slice.y);
    Vector2 RB = LT + slice;

    shared_ptr<Material> material = GetOwner()->GetRenderer()->GetMaterial();

    Vector2 SheetSize = _spriteSheet->GetSize();
    material->SetVec2(0,LT);
    material->SetVec2(1,RB);
    material->SetInt(0, 1);
}

void Animator::Clear()
{
}
