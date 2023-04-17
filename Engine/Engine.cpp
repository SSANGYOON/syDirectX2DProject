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

HRESULT Engine::Init(const WindowInfo& info)
{
	_window = info;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = info.width;
	sd.BufferDesc.Height = info.height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 144;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = info.hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = info.windowed;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, _swapChain.GetAddressOf(), _device.GetAddressOf(), &featureLevel, _context.GetAddressOf());
	if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, _swapChain.GetAddressOf(), _device.GetAddressOf(), &featureLevel, _context.GetAddressOf());
	if (res != S_OK)
		return res;

	GET_SINGLE(Input)->Init(info.hwnd);
	GET_SINGLE(Timer)->Init();
	GET_SINGLE(Resources)->CreateDefaultResource();

	
	CreateRenderTarget();


	RECT rt = { 0, 0, (LONG)info.width , (LONG)info.height };
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);
	SetWindowPos(info.hwnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);
	ShowWindow(info.hwnd, true);
	UpdateWindow(info.hwnd);

	_viewPort = { 0.0f, 0.0f, (float)_window.width, (float)_window.height, 0.0f, 1.0f };
	SetOriginalRenderTarget();

	SetUpState();
	
	_constantBuffers[(UINT8)Constantbuffer_Type::TRANSFORM] = make_shared<ConstantBuffer>();
	_constantBuffers[(UINT8)Constantbuffer_Type::TRANSFORM]->Init(Constantbuffer_Type::TRANSFORM, sizeof(TransformCB));

	//_constantBuffers[(UINT8)Constantbuffer_Type::GRID] = make_shared<ConstantBuffer>();
	//_constantBuffers[(UINT8)Constantbuffer_Type::GRID]->Init(Constantbuffer_Type::GRID, sizeof(GridCB));


	_constantBuffers[(UINT8)Constantbuffer_Type::SPRITE] = make_shared<ConstantBuffer>();
	_constantBuffers[(UINT8)Constantbuffer_Type::SPRITE]->Init(Constantbuffer_Type::SPRITE, sizeof(SpriteCB));

	_constantBuffers[(UINT8)Constantbuffer_Type::MATERIAL] = make_shared<ConstantBuffer>();
	_constantBuffers[(UINT8)Constantbuffer_Type::MATERIAL]->Init(Constantbuffer_Type::MATERIAL, sizeof(MaterialCB));

	_constantBuffers[(UINT8)Constantbuffer_Type::LIGHT] = make_shared<ConstantBuffer>();
	_constantBuffers[(UINT8)Constantbuffer_Type::LIGHT]->Init(Constantbuffer_Type::LIGHT, sizeof(LightCB));
	
	GET_SINGLE(CollisionManager)->Initialize();
	GET_SINGLE(SceneManager)->LoadScene(L"TestScene");

	return S_OK;
}

void Engine::Update()
{
	GET_SINGLE(Timer)->Update();
	GET_SINGLE(Input)->Update();
	GET_SINGLE(SceneManager)->Update();
	GET_SINGLE(CollisionManager)->Update();
}

void Engine::Render()
{
	FLOAT backgroundColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	ClearRenderTarget(backgroundColor);
	_context->ClearDepthStencilView(_depthStencilBuffer->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	GET_SINGLE(SceneManager)->Render();
	
}

void Engine::Present()
{
	_swapChain->Present(0,0);
}

void Engine::SetOriginalRenderTarget()
{
	_context->RSSetViewports(1, &_viewPort);
	_context->OMSetRenderTargets(1, _renderTarget->GetRTVRef(), _depthStencilBuffer->GetDSV());
}

void Engine::SetWindow(WindowInfo info)
{
	_window = info;
	_swapChain->ResizeBuffers(0, info.width, info.height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
}

HRESULT Engine::CreateRenderTarget()
{
	_renderTarget = make_shared<Texture>();
	_renderTarget->Create(_window.width, _window.height, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET);

	_depthStencilBuffer = make_shared<Texture>();
	_depthStencilBuffer->Create(_window.width, _window.height, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL);
	
	SetOriginalRenderTarget();
	return S_OK;
}

HRESULT Engine::CleanUpRenderTarget()
{
	_renderTarget = nullptr;
	_depthStencilBuffer = nullptr;

	return S_OK;
}

void Engine::ClearRenderTarget(const float* color)
{
	_context->ClearRenderTargetView(_renderTarget->GetRTV(), color);
}
