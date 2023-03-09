#include "pch.h"
#include "SwapChain.h"

HRESULT SwapChain::Init(const WindowInfo& info, ComPtr<ID3D11Device> device)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

	swapChainDesc.OutputWindow = info.hwnd;
	swapChainDesc.Windowed = info.windowed;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferDesc.Width = info.width;
	swapChainDesc.BufferDesc.Height = info.height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 144;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	
	Microsoft::WRL::ComPtr<IDXGIDevice> pDXGIDevice = nullptr;
	Microsoft::WRL::ComPtr<IDXGIAdapter> pDXGIAdapter = nullptr;
	Microsoft::WRL::ComPtr<IDXGIFactory> pDXGIFactory = nullptr;

	if (FAILED(device->QueryInterface(__uuidof(IDXGIDevice), (void**)pDXGIDevice.GetAddressOf())))
		return S_FALSE;

	if (FAILED(pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)pDXGIAdapter.GetAddressOf())))
		return S_FALSE;

	if (FAILED(pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)pDXGIFactory.GetAddressOf())))
		return S_FALSE;

	if (FAILED(pDXGIFactory->CreateSwapChain(device.Get(), &swapChainDesc, _swapChain.GetAddressOf())))
		return S_FALSE;

	return S_OK;
}

void SwapChain::Present()
{
	_swapChain->Present(0,0);
}
