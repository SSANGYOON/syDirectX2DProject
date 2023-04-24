#pragma once
#include "Resource.h"
class Animation : public Resource
{
public:
	Animation();
	virtual ~Animation();

	virtual HRESULT Load(const std::wstring& path, bool stockObject = false) override;
	virtual HRESULT Load(const Vector2 offset, const Vector2 size, const Vector2 step, const Vector2 targetOffset, UINT columns, UINT frame, float duration, string key);

	void Progress();
	string GetKey() { return _key; }
	Vector2 GetOffset() { return _offset; }
	Vector2 GetSize() { return _size; }
private:
	Vector2 _offset;
	Vector2 _size;
	Vector2 _step;
	Vector2 _targetOffset;
	UINT _columns;
	UINT _frame;
	float _duration;
	string _key;
};

