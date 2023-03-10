#pragma once
#include "DirectXTex\Include\DirectXTex.h"


#ifdef _DEBUG
#pragma comment(lib, "..\\External\\DirectXTex\\lib\\Debug\\DirectXTex.lib") 
#else 
#pragma comment(lib, "..\\External\\DirectXTex\\lib\\Release\\DirectXTex.lib") 
#endif

#include "Resource.h"

class Texture : public Resource
{
public:
	Texture();
	virtual ~Texture();

	virtual HRESULT Load(const std::wstring& path) override;
	void BindSRV(ShaderStage stage, UINT slot);

private:
	ScratchImage _image;
	ComPtr<ID3D11Texture2D> _texture;
	ComPtr<ID3D11ShaderResourceView> _SRV;
};

