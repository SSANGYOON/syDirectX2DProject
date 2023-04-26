#pragma once
#include "Resource.h"
class Animation : public Resource
{
public:
	Animation();
	virtual ~Animation();

	virtual HRESULT Load(const std::wstring& path, bool stockObject = false) override;
	virtual HRESULT Load(const Vector2 offset, const Vector2 size, const Vector2 step, const Vector2 targetOffset, UINT columns, UINT frame, float duration, string key, bool loop, string nextKey = "");

	float GetDuration() { return _duration; }
	const Vector2& GetOffset() { return _offset; }
	const Vector2& GetStep() { return _step; }
	const Vector2& GetSize() { return _size; }
	const Vector2& GetTargetOffset() { return _targetOffset; }
	const string& GetNextKey() { return _nextKey; }
	UINT GetColumns() { return _columns; }
	UINT GetFrames() { return _frame; }
	bool IsLoop() { return _loop; }

	string GetKey() { return _key; }
private:
	Vector2 _offset;
	Vector2 _size;
	Vector2 _step;
	Vector2 _targetOffset;
	UINT _columns;
	UINT _frame;
	float _duration;
	string _key;
	bool _loop;
	string _nextKey;
};

struct TransformFrame
{
	Vector3 position;
	float angle;
};

class TransformAnimation : public Resource
{
public:
	TransformAnimation();
	virtual ~TransformAnimation();

	virtual HRESULT Load(const std::wstring& path, bool stockObject = false) override;
	HRESULT Load(const std::string& key, float duration, bool loop, const vector<TransformFrame>& frames) { _key = key, _duration = duration, _loop = loop, _frames = frames; return S_OK; }
	float GetDuration() { return _duration; }
	const vector<TransformFrame>& GetFrames() { return _frames; }

	bool IsLoop() { return _loop; }
	string GetKey() { return _key; }
private:
	vector<TransformFrame> _frames;
	float _duration;
	string _key;
	bool _loop;
};

