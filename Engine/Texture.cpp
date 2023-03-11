#include "pch.h"
#include "Texture.h"
#include "Engine.h"

Texture::Texture()
	:Resource(Resource_Type::TEXTURE)
{
}

Texture::~Texture()
{
}

HRESULT Texture::Load(const std::wstring& path)
{
	std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
	std::wstring fullPath = parentPath.wstring() + L"\\Resources\\" + path;

	wchar_t szExtension[256] = {};
	_wsplitpath_s(path.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExtension, 256);

	std::wstring extension(szExtension);

	if (extension == L".dds" || extension == L".DDS")
	{
		if (FAILED(LoadFromDDSFile(fullPath.c_str(), DDS_FLAGS::DDS_FLAGS_NONE, nullptr, _image)))
			return S_FALSE;
	}
	else if (extension == L".tga" || extension == L".TGA")
	{
		if (FAILED(LoadFromTGAFile(fullPath.c_str(), nullptr, _image)))
			return S_FALSE;
	}
	else // WIC (png, jpg, jpeg, bmp )
	{
		if (FAILED(LoadFromWICFile(fullPath.c_str(), WIC_FLAGS::WIC_FLAGS_NONE, nullptr, _image)))
			return S_FALSE;
	}

	CreateShaderResourceView
	(
		DEVICE.Get(),
		_image.GetImages(),
		_image.GetImageCount(),
		_image.GetMetadata(),
		_SRV.GetAddressOf()
	);

	_SRV->GetResource((ID3D11Resource**)_texture.GetAddressOf());
	D3D11_TEXTURE2D_DESC desc;
	_texture->GetDesc(&desc);
	_size = { float(desc.Width), float(desc.Height) };
	return S_OK;
}

void Texture::BindSRV(ShaderStage stage, UINT slot)
{
	switch (stage)
	{
	case ShaderStage::VS:
		CONTEXT->VSSetShaderResources(slot, 1, _SRV.GetAddressOf());
		break;
	case ShaderStage::HS:
		CONTEXT->HSSetShaderResources(slot, 1, _SRV.GetAddressOf());
		break;
	case ShaderStage::DS:
		CONTEXT->DSSetShaderResources(slot, 1, _SRV.GetAddressOf());
		break;
	case ShaderStage::GS:
		CONTEXT->GSSetShaderResources(slot, 1, _SRV.GetAddressOf());
		break;
	case ShaderStage::PS:
		CONTEXT->PSSetShaderResources(slot, 1, _SRV.GetAddressOf());
		break;
	case ShaderStage::CS:
		CONTEXT->CSSetShaderResources(slot, 1, _SRV.GetAddressOf());
		break;
	default:
		break;
	}
}
