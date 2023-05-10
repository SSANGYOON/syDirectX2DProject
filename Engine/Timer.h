#pragma once

class Timer
{
DECLARE_SINGLE(Timer);

public:
	float DeltaTime() { return mDeltaTime * mTimeScale; }

	void Init();
	float GetTimeScale() { return mTimeScale; }
	void SetTimeScale(float timeScale) { mTimeScale = timeScale; }
	void Update();
	void Render(HDC hdc);

private:
	LARGE_INTEGER	mCpuFrequency = {};
	LARGE_INTEGER   mPrevFrequency = {};
	LARGE_INTEGER	mCurFrequency = {};

	float			mDeltaTime = 0.f;
	float			mOneSecond = 0.f;
	float			mTimeScale = 1.f;
};
