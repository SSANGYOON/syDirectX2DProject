#pragma once


#include "ConstantBuffer.h"

class Engine
{
public:
	HRESULT Init(const WindowInfo& info);
	void Update();

	ID3D11Device* GetDevice() { return _device.Get(); }
	ID3D11DeviceContext* GetContext() { return _context.Get(); }
	IDXGISwapChain* GetSwapChain() { return _swapChain.Get(); }

	void Render();
	void Present();
	void SetOriginalRenderTarget();

	void SetWindow(WindowInfo info);
	WindowInfo GetWindow() { return _window; }
	shared_ptr<ConstantBuffer> GetConstantBuffer(Constantbuffer_Type type) { return _constantBuffers[static_cast<UINT8>(type)];}


public:
	HRESULT CreateRenderTarget();
	HRESULT CleanUpRenderTarget();
	void ClearRenderTarget(const float* color);

private:
	WindowInfo _window;
	D3D11_VIEWPORT _viewPort;

	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11Debug> _debug;
	ComPtr<ID3D11DeviceContext> _context;
	ComPtr<IDXGISwapChain> _swapChain;

	shared_ptr<class Texture> _renderTarget;
	shared_ptr<class Texture> _depthStencilBuffer;

	array<shared_ptr<ConstantBuffer>, (size_t)Constantbuffer_Type::END> _constantBuffers;
};