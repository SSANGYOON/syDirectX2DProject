#include "pch.h"
#include "Animator.h"
#include "Timer.h"
#include "Material.h"
#include "GameObject.h"
#include "BaseRenderer.h"
#include "Texture.h"
#include "SpriteRenderer.h"

AnimationClip::AnimationClip(float duration, bool loop)
    :_loop(loop), _duration(duration)
{
}

AnimationClip2D::AnimationClip2D(Vector2 offset, Vector2 size, Vector2 step, UINT row, UINT frame, float duration, bool loop)
    :AnimationClip(duration, loop), _offset(offset), _size(size), _step(step), _row(row), _frame(frame)
{
}

void AnimationClip2D::Update(SpriteRenderer* spriteRenderer, float lastTime, float currentTime)
{

    UINT curFrame = UINT(min(currentTime, _duration- EPSILON) *_frame / _duration);

    Vector2 LT = _offset + Vector2((curFrame % _row) * _step.x, (curFrame / _row) * _step.y);


    spriteRenderer->SetSourceOffset(LT);
    spriteRenderer->SetSourceSize(_size);

    for (const auto& event : _events) {
        if (event.time <= currentTime && event.time > lastTime) {
            event.func();
        }
    }
}

Animator::Animator(GameObject* owner)
    :Component(Component_Type::Animator, owner), _currentTime(0.f), _lastTime(-1.f), _spriteRenderer(nullptr)
{
}

Animator::~Animator()
{
}

void Animator::Start()
{
    _spriteRenderer = _owner->GetComponent<SpriteRenderer>();
}

void Animator::Update()
{
    if (!_currentClip)
        return;

    _currentTime += GET_SINGLE(Timer)->DeltaTime();
    _currentClip->Update(_spriteRenderer, _lastTime, _currentTime);
    _lastTime = _currentTime;
    if (_currentTime >= _currentClip->_duration)
    {
        if (_currentClip->_loop)
            _currentTime -= _currentClip->_duration;
    }
}

void Animator::Play(const wstring& clip)
{
    if (_currentClip && _currentClip->_completeEvent)
        _currentClip->_completeEvent();
    _currentClip = _animations[clip];
    _currentTime = 0.0f;
    _currentKey = clip;
    _lastTime = -1.0f;
}

void Animator::Clear()
{
}

void Animator::LoadAnimation2dFromJson(const string& jsonFile)
{
    ifstream json_f;
    json_f.open(jsonFile);
    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;
    Json::Value value;

    JSONCPP_STRING errs;
    assert(parseFromStream(builder, json_f, &value, &errs));
    json_f.close();

    for (auto c : value["Clips"])
    {
        shared_ptr<AnimationClip2D> clip;
        Vector2 originalOffset = Vector2(c["OriginalOffset"][0].asFloat(), c["OriginalOffset"][1].asFloat());
        Vector2 originalSize = Vector2(c["SpriteSize"][0].asFloat(), c["SpriteSize"][1].asFloat());
        Vector2 step = Vector2(c["Step"][0].asFloat(), c["Step"][1].asFloat());
        UINT columns = c["Columns"].asUInt();
        UINT frame = c["Sprites"].asUInt();
        float duration = c["Duration"].asFloat();
        bool loop = c["Loop"].asBool();
        clip = make_shared<AnimationClip2D>(originalOffset, originalSize, step, columns, frame, duration, loop);
        const char* name = c["name"].asCString();
        AddAnimClip(wstring(name, name + strlen(name)), clip);
    }
}


