#pragma once

#include "Fmod\\Include\\fmod_studio.hpp"
#include "Fmod\\Include\\fmod.hpp"
#include "Fmod\\Include\\fmod_common.h"
#include "Fmod\\Include\\fmod_codec.h"

class FMOD::Studio::System;
class FMOD::System;

class FmodWrapper
{
public:
	static void Initialize();
	static bool CreateSound(const std::string& path, FMOD::Sound** sound);
	static void SoundPlay(FMOD::Sound* sound, FMOD::Channel** channel);
	static void Set3DListenerAttributes(const Vector3& pos, const Vector3& vel, const Vector3& forward, const Vector3& up);
	static void Release();

private:
	static FMOD::Studio::System* mSystem;
	static FMOD::System* mCoreSystem;
};

