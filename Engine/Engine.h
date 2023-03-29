#pragma once

#include "Device.h"
#include "SwapChain.h"
#include "ConstantBuffer.h"

class Engine
{
public:
	void Init(const WindowInfo& info);
	void Update();

	shared_ptr<Device> GetDevice() { return _device; }
	shared_ptr<SwapChain> GetSwapChain() { return _swapChain; }

	void Render();
	void Present();
	void SetOriginalRederTarget();
	WindowInfo GetWindow() { return _window; }
	shared_ptr<ConstantBuffer> GetConstantBuffer(Constantbuffer_Type type) { return _constantBuffers[static_cast<UINT8>(type)];}


private:
	HRESULT CreateRenderTarget();

private:
	WindowInfo _window;
	D3D11_VIEWPORT _viewPort;

	shared_ptr<Device> _device = make_shared<Device>();
	shared_ptr<SwapChain> _swapChain = make_shared<SwapChain>();
	shared_ptr<class Texture> _renderTarget;
	shared_ptr<class Texture> _depthStencilBuffer;

	array<shared_ptr<ConstantBuffer>, (size_t)Constantbuffer_Type::END> _constantBuffers;
};