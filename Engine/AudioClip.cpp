#include "pch.h"
#include "AudioClip.h"

AudioClip::AudioClip()
	: Resource(RESOURCE_TYPE::AUDIO_CLIP)
	, mSound(nullptr)
{
}

AudioClip::~AudioClip()
{
	mSound->release();
	mSound = nullptr;
}

HRESULT AudioClip::Load(const std::wstring& path, bool stockObject)
{
	std::wstring fullPath;
	if (stockObject) {
		std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
		fullPath = parentPath.wstring() + L"\\Resources\\" + path;
	}
	else
	{
		fullPath = path;

		std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
		auto resourcedir = parentPath / "Resources/assets";
		SetPath(filesystem::relative(fullPath, resourcedir).wstring());
	}

	if (!FmodWrapper::CreateSound(wtos(fullPath), &mSound))
		return S_FALSE;

	mSound->set3DMinMaxDistance(20, 1000);

	return S_OK;
}