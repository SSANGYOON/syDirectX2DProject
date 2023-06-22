#include "pch.h"
#include "AudioSource.h"
#include "AudioClip.h"
#include "Resources.h"

void SY::AudioSource::Play(bool loop)
{
	if (loop)
		mSound->GetSound()->setMode(FMOD_LOOP_NORMAL);
	else
		mSound->GetSound()->setMode(FMOD_LOOP_OFF);

	FmodWrapper::SoundPlay(mSound->GetSound(), &mChannel);
}

void SY::AudioSource::Stop()
{
	mChannel->stop();
}

void SY::AudioSource::Set3DAttributes(const Vector3& pos, const Vector3& vel)
{
	FMOD_VECTOR fmodPos(pos.x, pos.y, pos.z);
	FMOD_VECTOR fmodVel(vel.x, vel.y, vel.z);

	mChannel->set3DAttributes(&fmodPos, &fmodVel);
}

void SY::AudioSource::DrawImGui(AudioSource& component)
{
	ImGui::Text("Clip", ImVec2(100.0f, 0.0f));
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			std::filesystem::path clipPath(path);
			shared_ptr<AudioClip> clip = GET_SINGLE(Resources)->Load<AudioClip>(clipPath.wstring(), clipPath.wstring(), false);
			assert(clip->GetSound());
			component.mSound = clip;
		}
		ImGui::EndDragDropTarget();
	}

	if (component.mSound) {
		ImGui::Text(wtos(component.mSound->GetPath()).c_str());

		if (ImGui::Button("Remove Clip"))
			component.mSound = nullptr;
	}
}
