#pragma once
#include "FmodWrapper.h"

class AudioClip;
namespace SY {
	struct AudioSource
	{
		AudioSource() = default;
		AudioSource(const AudioSource&) = default;

		void Play(bool loop = false);
		void Stop();
		void Set3DAttributes(const Vector3& pos, const Vector3& vel);

		shared_ptr<AudioClip> mSound;
		FMOD::Channel* mChannel;

		static void DrawImGui(AudioSource& component);
	};
}

