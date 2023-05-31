#include "pch.h"
#include "Shader.h"
#include "Engine.h"
#include "Resources.h"

Shader::Shader()
    :Resource(RESOURCE_TYPE::GRAPHIC_SHADER)
	, _info{}
{
}

Shader::~Shader()
{
}

HRESULT Shader::Load(const wstring& path, bool stockObject)
{
    return E_NOTIMPL;
}

void Shader::CreateShader(const ShaderInfo& info, const ShaderEntry& entry, const wstring& file, bool noInstanceBuffer)
{
	std::filesystem::path path = std::filesystem::current_path().parent_path();
	path += "\\SHADER_SOURCE\\";

	std::wstring shaderPath(path.c_str());
	shaderPath += file;
	_info = info;
	if (entry.VS)
	{
		D3DCompileFromFile(shaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
			, "VS_MAIN", "vs_5_0", 0, 0
			, _VSBlob.GetAddressOf()
			, _ErrorBlob.GetAddressOf());

		if (_ErrorBlob)
		{
				OutputDebugStringA((char*)_ErrorBlob->GetBufferPointer());
				//_ErrorBlob->Release();
		}

		DEVICE->CreateVertexShader(_VSBlob->GetBufferPointer()
			, _VSBlob->GetBufferSize()
			, nullptr
			, _VS.GetAddressOf());

		if (noInstanceBuffer)
		{
			D3D11_INPUT_ELEMENT_DESC arrLayoutDesc[] = {

			{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }

			};

			DEVICE->CreateInputLayout(arrLayoutDesc, 2
				, _VSBlob->GetBufferPointer()
				, _VSBlob->GetBufferSize()
				, _inputLayout.GetAddressOf());
		}

		else {

			D3D11_INPUT_ELEMENT_DESC arrLayoutDesc[] = {

			{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},

			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"EMISSION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},

			{ "W", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{ "W", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{ "W", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{ "W", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D11_INPUT_PER_INSTANCE_DATA, 1},

			{ "WVP", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 96, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{ "WVP", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 112, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{ "WVP", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 128, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{ "WVP", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 144, D3D11_INPUT_PER_INSTANCE_DATA, 1},

			{"TILE", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 160, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"OFFSET", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 168, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			};

			DEVICE->CreateInputLayout(arrLayoutDesc, 14
				, _VSBlob->GetBufferPointer()
				, _VSBlob->GetBufferSize()
				, _inputLayout.GetAddressOf());
		}
	}

	if (entry.PS)
	{
		D3DCompileFromFile(shaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
			, "PS_MAIN", "ps_5_0", 0, 0
			, _PSBlob.GetAddressOf()
			, _ErrorBlob.GetAddressOf());

		if (_ErrorBlob)
		{
			OutputDebugStringA((char*)_ErrorBlob->GetBufferPointer());
			//_ErrorBlob->Release();
		}

		DEVICE->CreatePixelShader(_PSBlob->GetBufferPointer()
			, _PSBlob->GetBufferSize()
			, nullptr
			, _PS.GetAddressOf());
	}

	if (entry.GS)
	{
		D3DCompileFromFile(shaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
			, "GS_MAIN", "gs_5_0", 0, 0
			, _GSBlob.GetAddressOf()
			, _ErrorBlob.GetAddressOf());

		if (_ErrorBlob)
		{
			OutputDebugStringA((char*)_ErrorBlob->GetBufferPointer());
		//	mErrorBlob->Release();
		}

		DEVICE->CreateGeometryShader(_GSBlob->GetBufferPointer()
			, _GSBlob->GetBufferSize()
			, nullptr
			, _GS.GetAddressOf());
	}
}

void Shader::BindShader()
{
		CONTEXT->IASetPrimitiveTopology(_info.topology);
		CONTEXT->IASetInputLayout(_inputLayout.Get());

		CONTEXT->VSSetShader(_VS.Get(), nullptr, 0);
		CONTEXT->PSSetShader(_PS.Get(), nullptr, 0);
		CONTEXT->GSSetShader(_GS.Get(), nullptr, 0);

		ComPtr<ID3D11RasterizerState> rs = rasterizerStates[(UINT)_info.rst];
		ComPtr<ID3D11DepthStencilState> ds = depthstencilStates[(UINT)_info.dst];
		ComPtr<ID3D11BlendState> bs = blendStates[(UINT)_info.bst];

		CONTEXT->RSSetState(rs.Get());
		CONTEXT->OMSetDepthStencilState(ds.Get(),0);
		CONTEXT->OMSetBlendState(bs.Get(),nullptr, 0xffffffff);
}

