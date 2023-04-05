#include "pch.h"
#include "Animator.h"
#include "Timer.h"
#include "Material.h"
#include "GameObject.h"
#include "BaseRenderer.h"
#include "Texture.h"
#include "SpriteRenderer.h"

AnimationClip::AnimationClip(float duration, bool loop, wstring nextKey)
    :_loop(loop), _duration(duration), _nextKey(nextKey)
{
}

void AnimationClip::Update(GameObject* obj)
{
    auto anim = obj->GetComponent<Animator>();
    float currentTime = anim->GetClipTime();
    float lastTime = anim->GetLastTime();
    for (const auto& event : _events) {
        if (event.time <= currentTime && event.time > lastTime)
            event.func();
    }
    if (_duration < currentTime && _duration > lastTime && _endEvent)
        _endEvent();
}

AnimationClip2D::AnimationClip2D(Vector2 offset, Vector2 size, Vector2 step, UINT row, UINT frame, Vector2 targetOffset, float duration, bool loop, wstring nextKey)
    :AnimationClip(duration, loop, nextKey), _offset(offset), _size(size), _step(step), _row(row), _frame(frame), _targetOffset(targetOffset)
{
}

void AnimationClip2D::Update(GameObject* obj)
{
    Animator* anim = obj->GetComponent<Animator>();
    float currentTime = anim->GetClipTime();
    float lastTime = anim->GetLastTime();
    UINT curFrame = UINT(min(anim->GetClipTime(), _duration - EPSILON) * _frame / _duration);

    Vector2 LT = _offset + Vector2((curFrame % _row) * _step.x, (curFrame / _row) * _step.y);

    auto sr = obj->GetComponent<SpriteRenderer>();
    sr->SetSourceOffset(LT);
    sr->SetSourceSize(_size);
    sr->SetTargetOffset(_targetOffset);

    AnimationClip::Update(obj);
}

Animator::Animator(GameObject* owner)
    :Component(Component_Type::Animator, owner), _currentTime(0.f), _lastTime(-1.f), _spriteRenderer(nullptr), _transitionTime(0.f), _transitionElapsed(0.f), _endedAt(0.f)
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

    _currentClip->Update(_owner);

    _lastTime = _currentTime;
    _currentTime += GET_SINGLE(Timer)->DeltaTime();
    _transitionElapsed += GET_SINGLE(Timer)->DeltaTime();
    if (_currentTime >= _currentClip->_duration)
    {
        if (_currentClip->_loop)
            _currentTime -= _currentClip->_duration;
        else if (!_currentClip->_nextKey.empty())
            Play(_currentClip->_nextKey);
    }
}

void Animator::Play(const wstring& clip, float Time, float Transition)
{
    if (_currentClip){
        _prevClip = nullptr;
        _prevClip = _currentClip;
        if (_currentClip->_completeEvent)
            _currentClip->_completeEvent();

    }

    _endedAt = _currentTime;
    _currentClip = nullptr;
    _currentClip = _animations[clip];
    _currentTime = Time;
    _transitionElapsed = 0.f;
    _transitionTime = Transition;
    _currentKey = clip;
    _lastTime = -1.0f;
}

void Animator::Stop()
{
    _prevClip = nullptr;
    _currentClip = nullptr;
}

void Animator::Clear()
{
}

void Animator::LoadAnimation2dFromJson(const string& jsonFile)
{
    ifstream json_f;
    std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
    std::string fullPath = parentPath.string() + "\\Resources\\" + jsonFile;
    json_f.open(fullPath);
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

        Vector2 targetOffset = {};
        char s[] = "TargetOffset";
        if (c.find(s, s + strlen(s)) != nullptr)
            targetOffset = { c["TargetOffset"][0].asFloat(),c["TargetOffset"][1].asFloat()};

        float duration = c["Duration"].asFloat();
        bool loop = c["Loop"].asBool();
       
        wstring nextKey;

        if (c.isMember("nextKey")) {
            const char* nextanim = c["nextKey"].asCString();
            nextKey = wstring(nextanim, nextanim + strlen(nextanim));
        }

        clip = make_shared<AnimationClip2D>(originalOffset, originalSize, step, columns, frame, targetOffset, duration, loop, nextKey);
        const char* name = c["name"].asCString();
        AddAnimClip(wstring(name, name + strlen(name)), clip);
    }
}

void Animator::LoadTransformFromJson(const string& jsonFile)
{
    ifstream json_f;
    std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
    std::string fullPath = parentPath.string() + "\\Resources\\" + jsonFile;

    json_f.open(fullPath);
    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;
    Json::Value value;

    JSONCPP_STRING errs;
    assert(parseFromStream(builder, json_f, &value, &errs));
    json_f.close();

    auto clips = value["Clips"];
    for (auto clip : clips)
    {
        const char* name = clip["ClipName"].asCString();  
        float duration = clip["Duration"].asFloat();
        bool loop = clip["Loop"].asBool();
        shared_ptr<TransformClip> trClip = make_shared<TransformClip>(clip, duration, loop);
        AddAnimClip(wstring(name, name + strlen(name)), trClip);
    }
}

TransformClip::TransformClip(const Json::Value& value, float duration, bool loop, wstring nextKey)
    :AnimationClip(duration, loop, nextKey)
{
    transforms.resize(value["Frames"].asUInt());
    auto frames = value["Frame Transform"];
    _transformValid.reset();
    for (int i = 0; i < transforms.size(); i++)
    {
        if (frames[i].isMember("Offset")) {
            auto pos = frames[i]["Offset"];
            _transformValid[TRANSFORM_COMPONENT::TRANSLATION] = true;
            transforms[i].Position = Vector3(pos[0].asFloat(), pos[1].asFloat(), pos[2].asFloat()) * 10.f;
        }
        if (frames[i].isMember("Angle")) {
            auto rotation = frames[i]["Angle"];
            _transformValid[TRANSFORM_COMPONENT::ROTATION] = true;
            transforms[i].Rotation = rotation.asFloat();
        }
        if (frames[i].isMember("Scale")) {
            auto scale = frames[i]["Scale"];
            _transformValid[TRANSFORM_COMPONENT::TRANSLATION] = true;
            transforms[i].Scale = Vector3(scale[0].asFloat(), scale[1].asFloat(), scale[2].asFloat());
        }
    }
}

void TransformClip::Update(GameObject* obj)
{
    Animator* anim = obj->GetComponent<Animator>();
    float lastTime = anim->GetLastTime();
    float currentTime = anim->GetClipTime();
    float transitionElaped = anim->GetTransitionElapsed();
    float transitionLength = anim->GetTransitionLength();
    TransformStruct currentClipTrans;
     GetCurrentTransform(obj, lastTime, currentTime, OUT currentClipTrans);
    Transform* tr = obj->GetTransform();

    if (transitionElaped >= transitionLength){
        if (_transformValid[TRANSFORM_COMPONENT::TRANSLATION])
            tr->SetPosition(currentClipTrans.Position);
        if (_transformValid[TRANSFORM_COMPONENT::ROTATION])
            tr->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Backward,currentClipTrans.Rotation * DirectX::XM_PI / 180.f));
        if (_transformValid[TRANSFORM_COMPONENT::SCALE])
            tr->SetScale(currentClipTrans.Scale);
    }

    else {
        float transitionfrac = transitionElaped / transitionLength;
        TransformStruct prevClipTrans;
        TransformClip* prevClip = static_cast<TransformClip*>(anim->GetPrevClip());

        if (prevClip == nullptr)
        {
            if (_transformValid[TRANSFORM_COMPONENT::TRANSLATION]) {
                Vector3 nextPos = Vector3::Lerp(tr->GetLocalPosition(), currentClipTrans.Position, TIME->DeltaTime()/ transitionLength / (1 - transitionfrac));
                tr->SetPosition(nextPos);
            }
            if (_transformValid[TRANSFORM_COMPONENT::ROTATION]) {
                float rad = currentClipTrans.Rotation * DirectX::XM_PI / 180.f;
                Quaternion nextRot = Quaternion::Slerp(tr->GetRotation(), Quaternion::CreateFromAxisAngle(Vector3::Backward, rad), transitionfrac);
                tr->SetRotation(nextRot);
            }
        }
        else {
            prevClip->GetCurrentTransform(obj, anim->_endedAt + transitionElaped - TIME->DeltaTime(), anim->_endedAt + transitionElaped, OUT prevClipTrans);

            if (_transformValid[TRANSFORM_COMPONENT::TRANSLATION])
                tr->SetPosition(Vector3::Lerp(prevClipTrans.Position, currentClipTrans.Position, transitionfrac));

            else
                tr->SetPosition(prevClipTrans.Position);

            if (_transformValid[TRANSFORM_COMPONENT::ROTATION]) {
                float angle = (1 - transitionfrac) * prevClipTrans.Rotation + transitionfrac * currentClipTrans.Rotation;
                tr->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Backward, angle * DirectX::XM_PI / 180.f));
            }
            else
                tr->SetRotation(Quaternion::CreateFromAxisAngle(Vector3::Backward, prevClipTrans.Rotation * DirectX::XM_PI / 180.f));
        }
          
        //tr->SetScale(Vector3::Lerp(prevClipTrans.Scale, currentClipTrans.Scale, transitionfrac));
    }

    AnimationClip::Update(obj);
}

void TransformClip::GetCurrentTransform(GameObject* obj, float lastTime, float currentTime, OUT TransformStruct& ts)
{
    UINT frames = transforms.size();
    UINT curFrame = UINT(min(currentTime, _duration - EPSILON) * (frames - 1) / _duration);

    UINT nextFrame = curFrame + 1;

    Transform* tr = obj->GetTransform();

    float frameTime = currentTime / _duration * (frames - 1) - curFrame;

    ts = {};

    if (_transformValid[UINT(TRANSFORM_COMPONENT::TRANSLATION)])
        ts.Position = Vector3::Lerp(transforms[curFrame].Position, transforms[nextFrame].Position, frameTime);
    else
        ts.Position = tr->GetLocalPosition();

    if (_transformValid[UINT(TRANSFORM_COMPONENT::ROTATION)])
        ts.Rotation = (1 - frameTime) * transforms[curFrame].Rotation + frameTime * transforms[nextFrame].Rotation;
    else
        ts.Rotation = 0;

    if (_transformValid[UINT(TRANSFORM_COMPONENT::SCALE)])
        ts.Scale = Vector3::Lerp(transforms[curFrame].Scale, transforms[nextFrame].Scale, frameTime);
    else
        ts.Scale = tr->GetScale();
}
