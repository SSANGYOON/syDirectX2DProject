// Engine.cpp : 정적 라이브러리를 위한 함수를 정의합니다.
//

#include "pch.h"
#include "Engine.h"
#include "PipelineStates.h"
#include "Resources.h"
#include "SceneManager.h"
#include "CollisionManager.h"
#include "Material.h"
#include "Texture.h"

#include "Input.h"
#include "Timer.h"
// TODO: 라이브러리 함수의 예제입니다.

void Engine::Init(const WindowInfo& info)
{
	_window = info;

	_device->Init();
	GET_SINGLE(Input)->Init(info.hwnd);
	GET_SINGLE(Timer)->Init();
	GET_SINGLE(Resources)->CreateDefaultResource();

	if(FAILED(_swapChain->Init(info, _device->GetDevice())))
		return;

	_renderTarget = make_shared<Texture>();
	_renderTarget->Create(info.width, info.height, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET);

	_depthStencilBuffer = make_shared<Texture>();
	_depthStencilBuffer->Create(info.width, info.height, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL);


	RECT rt = { 0, 0, (LONG)info.width , (LONG)info.height };
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);
	SetWindowPos(info.hwnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);
	ShowWindow(info.hwnd, true);
	UpdateWindow(info.hwnd);

	_viewPort = { 0.0f, 0.0f, (float)_window.width, (float)_window.height, 0.0f, 1.0f };
	_device->GetContext()->RSSetViewports(1, &_viewPort);
	_device->GetContext()->OMSetRenderTargets(1, _renderTarget->GetRTVRef(), _depthStencilBuffer->GetDSV());

	SetUpState();
	
	_constantBuffers[(UINT8)Constantbuffer_Type::TRANSFORM] = make_shared<ConstantBuffer>();
	_constantBuffers[(UINT8)Constantbuffer_Type::TRANSFORM]->Init(Constantbuffer_Type::TRANSFORM, sizeof(TransformCB));

	_constantBuffers[(UINT8)Constantbuffer_Type::GRID] = make_shared<ConstantBuffer>();
	_constantBuffers[(UINT8)Constantbuffer_Type::GRID]->Init(Constantbuffer_Type::GRID, sizeof(GridCB));

	_constantBuffers[(UINT8)Constantbuffer_Type::MATERIAL] = make_shared<ConstantBuffer>();
	_constantBuffers[(UINT8)Constantbuffer_Type::MATERIAL]->Init(Constantbuffer_Type::MATERIAL, sizeof(MaterialCB));
	
	GET_SINGLE(SceneManager)->LoadScene(L"TestScene");
	GET_SINGLE(CollisionManager)->Initialize();
}

void Engine::Update()
{
	GET_SINGLE(Timer)->Update();
	GET_SINGLE(Input)->Update();
	GET_SINGLE(SceneManager)->Update();
	GET_SINGLE(CollisionManager)->Update();
	Engine::Render();
}

void Engine::Render()
{
	FLOAT backgroundColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	_device->GetContext()->ClearRenderTargetView(_renderTarget->GetRTV(), backgroundColor);
	_device->GetContext()->ClearDepthStencilView(_depthStencilBuffer->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	GET_SINGLE(SceneManager)->Render();
	
}

void Engine::Present()
{
	_swapChain->Present();
}

HRESULT Engine::CreateRenderTarget()
{
	HRESULT hr = _swapChain->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)_renderTarget->GetRTVRef());

	// Create Rendertarget View
	hr = _device->GetDevice()->CreateRenderTargetView(_renderTarget->GetD3Texture(), nullptr, _renderTarget->GetRTVRef());
	return hr;
}
