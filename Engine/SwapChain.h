#pragma once
class SwapChain
{
public:
	HRESULT Init(const WindowInfo& info, ComPtr<ID3D11Device> device);
	void Present();

	ComPtr<IDXGISwapChain> GetSwapChain() { return _swapChain; }

private:
	ComPtr<IDXGISwapChain> _swapChain;
};

