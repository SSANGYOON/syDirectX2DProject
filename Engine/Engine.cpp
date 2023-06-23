

#include "pch.h"
#include "Engine.h"
#include "PipelineStates.h"
#include "Resources.h"

#include "Material.h"
#include "Texture.h"
#include "FW1FontWrapper\Include\\FW1FontWrapper.h"
#include "FW1FontWrapper\Include\\FW1CompileSettings.h"
#include "FmodWrapper.h"

#ifdef _DEBUG
#pragma comment(lib, "FW1FontWrapper\\Library\\Debug\\FW1FontWrapperL.lib")
#else
#pragma comment(lib, "FW1FontWrapper\\Library\\Debug\\FW1FontWrapper.lib")
#endif

Engine::~Engine()
{
	mFW1Factory->Release();
	mFW1Factory = nullptr;

	mFontWrapper->Release();
	mFontWrapper = nullptr;
}

HRESULT Engine::Init(const WindowInfo& info)
{
	_window = info;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = info.width;
	sd.BufferDesc.Height = info.height;
	sd.BufferDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
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

	
	CreateRenderTargetGroup();


	RECT rt = { 0, 0, (LONG)info.width , (LONG)info.height };
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);
	SetWindowPos(info.hwnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);
	ShowWindow(info.hwnd, true);
	UpdateWindow(info.hwnd);

	_viewPort = { 0.0f, 0.0f, (float)_window.width, (float)_window.height, 0.0f, 1.0f };

	SetUpState();
	
	_constantBuffers[(UINT8)Constantbuffer_Type::TRANSFORM] = make_shared<ConstantBuffer>();
	_constantBuffers[(UINT8)Constantbuffer_Type::TRANSFORM]->Init(Constantbuffer_Type::TRANSFORM, sizeof(TransformCB));
	
	_constantBuffers[(UINT8)Constantbuffer_Type::VISUALEFFECT] = make_shared<ConstantBuffer>();
	_constantBuffers[(UINT8)Constantbuffer_Type::VISUALEFFECT]->Init(Constantbuffer_Type::VISUALEFFECT, sizeof(VECB));

	_constantBuffers[(UINT8)Constantbuffer_Type::MATERIAL] = make_shared<ConstantBuffer>();
	_constantBuffers[(UINT8)Constantbuffer_Type::MATERIAL]->Init(Constantbuffer_Type::MATERIAL, sizeof(MaterialCB));

	_constantBuffers[(UINT8)Constantbuffer_Type::LIGHT] = make_shared<ConstantBuffer>();
	_constantBuffers[(UINT8)Constantbuffer_Type::LIGHT]->Init(Constantbuffer_Type::LIGHT, sizeof(LightCB));

	_constantBuffers[(UINT8)Constantbuffer_Type::PARTICLE] = make_shared<ConstantBuffer>();
	_constantBuffers[(UINT8)Constantbuffer_Type::PARTICLE]->Init(Constantbuffer_Type::PARTICLE, sizeof(ParticleCB));

	_constantBuffers[(UINT8)Constantbuffer_Type::BLOOM] = make_shared<ConstantBuffer>();
	_constantBuffers[(UINT8)Constantbuffer_Type::BLOOM]->Init(Constantbuffer_Type::BLOOM, sizeof(BloomCB));

	auto r1 = FAILED(FW1CreateFactory(FW1_VERSION, &mFW1Factory));

	auto r2 = FAILED(mFW1Factory->CreateFontWrapper(_device.Get(), L"Arial", &mFontWrapper));

	FmodWrapper::Initialize();

	return S_OK;
}

void Engine::SetWindow(WindowInfo info)
{
	_window = info;
	_swapChain->ResizeBuffers(0, info.width, info.height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
}

void Engine::SetViewport(UINT left, UINT right, UINT width, UINT height)
{
	_viewPort.TopLeftX = (float)left;
	_viewPort.TopLeftY = (float)right;
	_viewPort.Width = (float)width;
	_viewPort.Height = (float)height;

	CONTEXT->RSSetViewports(1, &_viewPort);
}

void Engine::BindSwapChain()
{
	CONTEXT->OMSetRenderTargets(1, _rtv.GetAddressOf(), _depth->GetDSV());
}

void Engine::ClearSwapChain()
{
	float clearColor[4] = { 0.1f,0.1f,0.1f,1.f };
	CONTEXT->ClearRenderTargetView(_rtv.Get(), clearColor);

	CONTEXT->ClearDepthStencilView(_depth->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

void Engine::BindRenderTargetGroup(RENDER_TARGET_GROUP_TYPE group)
{
	_rtGroups[(UINT)group]->OMSetRenderTargets();
	CONTEXT->RSSetViewports(1,&_viewPort);
}

void Engine::ClearRenderTargetGroup(RENDER_TARGET_GROUP_TYPE group, float* color)
{
	_rtGroups[(UINT)group]->ClearRenderTargets(color);
}

void Engine::ClearRenderTargetGroup(RENDER_TARGET_GROUP_TYPE group, UINT ind, float* color)
{
	_rtGroups[(UINT)group]->ClearRenderTargets(color, ind);
}

HRESULT Engine::CreateRenderTargetGroup()
{
	_swapChain->GetBuffer(0, IID_PPV_ARGS(_renderTarget.GetAddressOf()));
	_device->CreateRenderTargetView(_renderTarget.Get(), nullptr, _rtv.GetAddressOf());
	_depth = make_shared<Texture>();
	_depth->Create(_window.width, _window.height, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL);


	shared_ptr<Texture> emission = make_shared<Texture>();
	emission->Create(_window.width, _window.height, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE);
	shared_ptr<Texture> dsTexture = make_shared<Texture>();
	dsTexture->Create(_window.width, _window.height, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL);

	{
		vector<RenderTarget> rtVec(RENDER_TARGET_HDR_BUFFER_MEMBER_COUNT);

		rtVec[0].target = make_shared<Texture>();
		rtVec[0].target->Create(_window.width, _window.height, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE);

		rtVec[1].target = emission;

		_rtGroups[static_cast<UINT8>(RENDER_TARGET_GROUP_TYPE::HDR)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<UINT8>(RENDER_TARGET_GROUP_TYPE::HDR)]->Create(RENDER_TARGET_GROUP_TYPE::HDR, rtVec, dsTexture);
	}

	{
		vector<RenderTarget> rtVec(RENDER_TARGET_DEFFERED_GROUP_MEMBER_COUNT);

		rtVec[0].target = make_shared<Texture>();
		rtVec[0].target->Create(_window.width, _window.height, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE);

		rtVec[1].target = emission;

		rtVec[2].target = make_shared<Texture>();
		rtVec[2].target->Create(_window.width, _window.height, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE);

		rtVec[3].target = make_shared<Texture>();
		rtVec[3].target->Create(_window.width, _window.height, DXGI_FORMAT_R8_UNORM, D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE);

		

		_rtGroups[static_cast<UINT8>(RENDER_TARGET_GROUP_TYPE::DEFFERED)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<UINT8>(RENDER_TARGET_GROUP_TYPE::DEFFERED)]->Create(RENDER_TARGET_GROUP_TYPE::DEFFERED, rtVec, dsTexture);
	}

	{
		vector<RenderTarget> rtVec(RENDER_TARGET_EDITER_GROUP_MEMBER_COUNT);

		rtVec[0].target = make_shared<Texture>();
		rtVec[0].target->Create(_window.width, _window.height, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE);



		_rtGroups[static_cast<UINT8>(RENDER_TARGET_GROUP_TYPE::EDITOR)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<UINT8>(RENDER_TARGET_GROUP_TYPE::EDITOR)]->Create(RENDER_TARGET_GROUP_TYPE::EDITOR, rtVec, dsTexture);
	}

	return S_OK;
}
