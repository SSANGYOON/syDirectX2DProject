#include "pch.h"
#include "Animator.h"
#include "Animation.h"

Animator::Animator()
    :Component(Component_Type::Animator)
    , _animations{}
    , _events{}
    , _activeAnimation(nullptr)

    , _loop(false)
{
}

Animator::~Animator()
{
}

void Animator::Start()
{
}

void Animator::Update()
{
    if (_activeAnimation == nullptr)
        return;

    if (_activeAnimation->IsComplete() && _loop)
    {
        AnimationEvent event
            = FindEvent(_activeAnimation->GetName());

        if (event)
            event(*GetOwner());

        _activeAnimation->Reset();
    }

    _activeAnimation->Update();
}

void Animator::Render()
{
}

bool Animator::Create(const std::wstring& name, std::shared_ptr<Texture> atlas, Vector2 leftTop, Vector2 size, Vector2 offset, UINT columnLegth, UINT spriteLegth, float duration)
{
    return false;
}

shared_ptr<Animation> Animator::FindAnimation(const std::wstring& name)
{
    return shared_ptr<Animation>();
}

AnimationEvent Animator::FindEvent(const wstring& name)
{
    if (_events.find(name) != _events.end())
        return _events.find(name)->second;
    else
        return nullptr;
}

void Animator::AddEvent(AnimationEvent event, const wstring& name)
{
    _events[name] = event;
}

void Animator::Play(std::wstring& name, bool loop)
{
    Animation* prevAnimation = _activeAnimation.get();
    AnimationEvent event= FindEvent(prevAnimation->GetName());

    if (event)
        event(*GetOwner());

    _activeAnimation = FindAnimation(name);
    _activeAnimation->Reset();
    _loop = loop;
}

void Animator::Binds()
{
    if (_activeAnimation == nullptr)
        return;

    _activeAnimation->BindShader();
}

void Animator::Clear()
{
    if (_activeAnimation == nullptr)
        return;

    _activeAnimation->Clear();
}
