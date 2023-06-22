#include "pch.h"
#include "FmodWrapper.h"



#ifdef _DEBUG
#pragma comment(lib, "Fmod\\Library\\Debug\\fmodL_vc.lib")
#pragma comment(lib, "Fmod\\Library\\Debug\\fmodstudioL_vc.lib")
#else
#pragma comment(lib, "Fmod\\Library\\Debug\\fmod_vc.lib")
#pragma comment(lib, "Fmod\\Library\\Release\\fmodstudio_vc.lib")
#endif

FMOD::Studio::System* FmodWrapper::mSystem = nullptr;
FMOD::System* FmodWrapper::mCoreSystem = nullptr;

void FmodWrapper::Initialize()
{
	void* extraDriverData = NULL;
	FMOD::Studio::System::create(&mSystem);

	mSystem->getCoreSystem(&mCoreSystem);
	mCoreSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0);

	mSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, extraDriverData);
}

bool FmodWrapper::CreateSound(const std::string& path, FMOD::Sound** sound)
{
	if (FMOD_OK != mCoreSystem->createSound(path.c_str(), FMOD_2D, 0, sound))
		return false;

	return true;
}

void FmodWrapper::SoundPlay(FMOD::Sound* sound, FMOD::Channel** channel)
{
	mCoreSystem->playSound(sound, 0, false, channel);
}

void FmodWrapper::Set3DListenerAttributes(const Vector3& pos, const Vector3& vel, const Vector3& forward, const Vector3& up)
{
#define FVECTOR_PTR reinterpret_cast<const FMOD_VECTOR*>

	mCoreSystem->set3DListenerAttributes(0, FVECTOR_PTR(&pos), FVECTOR_PTR(&vel), FVECTOR_PTR(&forward), FVECTOR_PTR(&up));
}

void FmodWrapper::Release()
{
	mCoreSystem->release();
	mCoreSystem = nullptr;

	mSystem->release();
	mSystem = nullptr;
}
