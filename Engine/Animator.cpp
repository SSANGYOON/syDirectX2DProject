#include "pch.h"
#include "Animator.h"
#include "Timer.h"
#include "Material.h"
#include "GameObject.h"
#include "BaseRenderer.h"
#include "Texture.h"

AnimationClip::AnimationClip(float duration, bool loop)
    :_loop(loop), _duration(duration)
{
}

AnimationClip2D::AnimationClip2D(Vector2 offset, Vector2 size, Vector2 step, UINT row, UINT frame, float duration, bool loop)
    :AnimationClip(duration, loop), _offset(offset), _size(size), _step(step), _row(row), _frame(frame)
{
}

void AnimationClip2D::Update(Material* material, float lastTime, float currentTime)
{
    material->SetVec2(2, material->GetTexture(0)->GetSize());
    UINT curFrame = UINT(min(currentTime, _duration- EPSILON) *_frame / _duration);

    Vector2 LT = _offset + Vector2((curFrame % _row) * _step.x, (curFrame / _row) * _step.y);
    Vector2 RB = LT + _size;

    material->SetVec2(0, LT);
    material->SetVec2(1, RB);
    material->SetInt(0, 1);

    for (const auto& event : _events) {
        if (event.time >= currentTime && event.time < lastTime) {
            event.func();
        }
    }
}

Animator::Animator()
    :Component(Component_Type::Animator), _currentTime(0.f), _lastTime(-1.f)
{
}

Animator::~Animator()
{
}

void Animator::Start()
{
    _material = _owner.lock()->GetComponent<BaseRenderer>()->GetMaterial();
}

void Animator::Update()
{
    if (!_currentClip)
        return;

    _currentTime += GET_SINGLE(Timer)->DeltaTime();
    _currentClip->Update(_material.get(), _lastTime, _currentTime);

    if (_currentTime >= _currentClip->_duration)
    {
        if (_currentClip->_loop)
            _currentTime -= _currentClip->_duration;

        else if(!_currentClip->nextAnim.empty())
            Play(_currentClip->nextAnim);
    }
    else
    {
        auto it = _transitionRules.find(_currentKey);
        if (it != _transitionRules.end())
        {
            for (const TransitionRule& transition : it->second)
            {
                if (checkTransitionRule(transition))
                {
                    Play(transition.targetAnimation);
                    break;
                }
            }
        }
    }

    _lastTime = _currentTime;
}

void Animator::Play(const wstring& clip)
{
    if (_currentClip && _currentClip->_completeEvent)
        _currentClip->_completeEvent();
    _currentClip = _animations[clip];
    _currentTime = 0.0f;
    _lastTime = -1.0f;
}

const float Animator::GetFloatCondition(const wstring& name)
{
    auto it = _floatConditions.find(name);
    assert(it != _floatConditions.end());
    return *(it->second);
}

const bool Animator::GetBooleanCondition(const wstring& name)
{
    auto it = _boolConditions.find(name);
    assert(it != _boolConditions.end());
    return *(it->second);
}

bool Animator::checkTransitionRule(const TransitionRule& rule)
{
    for (const auto& condition : rule.conditions) {
        assert(!condition.name.empty());
        switch (condition.type) {
        case ConditionType::BOOL:
            if (GetBooleanCondition(condition.name) != (bool)condition.value)
                return false;
            break;
        case ConditionType::FLOAT_GREATER:
            if (GetFloatCondition(condition.name) <= condition.value)
                return false;
            break;
        case ConditionType::FLOAT_LESS:
            if (GetFloatCondition(condition.name) >= condition.value)
                return false;
            break;
        default:
            return false;
            break;
        }
    }

    return true;
}

void Animator::Clear()
{
}


