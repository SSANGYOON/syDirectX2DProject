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

	static void Clear(UINT startSlot);

	virtual HRESULT Load(const std::wstring& path) override;
	bool Create(UINT width, UINT height, DXGI_FORMAT format, UINT bindFlag);

	void BindSRV(ShaderStage stage, UINT slot);
	void ClearSRV(ShaderStage stage, UINT slot);
	void BindUAV(UINT slot);
	void ClearUAV(ShaderStage stage, UINT slot);

	const Vector2& GetSize() { return _size; }

	ID3D11DepthStencilView* GetDSV() { return _DSV.Get(); }
	ID3D11DepthStencilView** GetDSVRef() { return _DSV.GetAddressOf(); }

	ID3D11RenderTargetView* GetRTV();
	ID3D11RenderTargetView** GetRTVRef();

	ID3D11Texture2D* GetD3Texture() { return _texture.Get(); }

private:
	ScratchImage _image;
	ComPtr<ID3D11Texture2D> _texture;
	ComPtr<ID3D11DepthStencilView> _DSV;
	ComPtr<ID3D11RenderTargetView> _RTV;
	ComPtr<ID3D11UnorderedAccessView> _UAV;
	ComPtr<ID3D11ShaderResourceView> _SRV;
	D3D11_TEXTURE2D_DESC mDesc;
	Vector2 _size;
};

