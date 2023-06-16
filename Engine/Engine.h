#pragma once

#include "ConstantBuffer.h"
#include "RenderTargetGroup.h"
class Texture;
class IFW1Factory;
class IFW1FontWrapper;
class Engine
{
public:
	HRESULT Init(const WindowInfo& info);
	void SetVSync(bool enabled) { _window.VSync = enabled; }

	ID3D11Device* GetDevice() { return _device.Get(); }
	ID3D11DeviceContext* GetContext() { return _context.Get(); }
	IDXGISwapChain* GetSwapChain() { return _swapChain.Get(); }

	void SetWindow(WindowInfo info);

	void SetViewport(UINT left, UINT bottom, UINT right, UINT top);
	WindowInfo GetWindow() { return _window; }
	shared_ptr<ConstantBuffer> GetConstantBuffer(Constantbuffer_Type type) { return _constantBuffers[static_cast<UINT8>(type)];}

	void BindSwapChain();
	void ClearSwapChain();

	void BindRenderTargetGroup(RENDER_TARGET_GROUP_TYPE group);
	void ClearRenderTargetGroup(RENDER_TARGET_GROUP_TYPE group, float* color = nullptr);
	void ClearRenderTargetGroup(RENDER_TARGET_GROUP_TYPE group, UINT ind, float* color = nullptr);
	shared_ptr<Texture> GetRenderTargetTex(RENDER_TARGET_GROUP_TYPE rtType, UINT index) { return _rtGroups[(UINT)rtType]->GetRTTexture(index); }

public:
	HRESULT CreateRenderTargetGroup();
	IFW1FontWrapper* mFontWrapper;

private:
	WindowInfo _window;
	D3D11_VIEWPORT _viewPort;

	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11Debug> _debug;
	ComPtr<ID3D11DeviceContext> _context;
	ComPtr<IDXGISwapChain> _swapChain;

	ComPtr<ID3D11Texture2D> _renderTarget;
	ComPtr<ID3D11RenderTargetView> _rtv;
	shared_ptr<Texture> _depth;

	IFW1Factory* mFW1Factory;

	array<shared_ptr<ConstantBuffer>, (size_t)Constantbuffer_Type::END> _constantBuffers;
	array<shared_ptr<RenderTargetGroup>, RENDER_TARGET_GROUP_COUNT> _rtGroups;
};