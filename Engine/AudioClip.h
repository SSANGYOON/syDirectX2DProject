#pragma once
#include "Resource.h"
#include "FmodWrapper.h"

class AudioClip : public Resource
{
public:
	AudioClip();
	virtual ~AudioClip();

	virtual HRESULT Load(const std::wstring& path, bool stockObject = true) override;
	FMOD::Sound* GetSound() { return mSound; }

private:
	FMOD::Sound* mSound;
};

