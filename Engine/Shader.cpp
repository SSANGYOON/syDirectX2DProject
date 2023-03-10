#include "pch.h"
#include "Shader.h"
#include "Engine.h"

Shader::Shader()
    :Resource(Resource_Type::SHADER)
	, _info{}
{
}

Shader::~Shader()
{
}

HRESULT Shader::Load(const wstring& path)
{
    return E_NOTIMPL;
}

void Shader::CreateShader(const ShaderInfo& info, const ShaderEntry& entry, const wstring& file)
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
				_ErrorBlob->Release();
		}

		DEVICE->CreateVertexShader(_VSBlob->GetBufferPointer()
			, _VSBlob->GetBufferSize()
			, nullptr
			, _VS.GetAddressOf());

		D3D11_INPUT_ELEMENT_DESC arrLayoutDesc[3] = {};

		arrLayoutDesc[0].AlignedByteOffset = 0;
		arrLayoutDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		arrLayoutDesc[0].InputSlot = 0;
		arrLayoutDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		arrLayoutDesc[0].SemanticName = "POSITION";
		arrLayoutDesc[0].SemanticIndex = 0;

		arrLayoutDesc[1].AlignedByteOffset = 16;
		arrLayoutDesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		arrLayoutDesc[1].InputSlot = 0;
		arrLayoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		arrLayoutDesc[1].SemanticName = "COLOR";
		arrLayoutDesc[1].SemanticIndex = 0;

		arrLayoutDesc[2].AlignedByteOffset = 32;
		arrLayoutDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
		arrLayoutDesc[2].InputSlot = 0;
		arrLayoutDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		arrLayoutDesc[2].SemanticName = "TEXCOORD";
		arrLayoutDesc[2].SemanticIndex = 0;

		DEVICE->CreateInputLayout(arrLayoutDesc, 3
			, _VSBlob->GetBufferPointer()
			, _VSBlob->GetBufferSize()
			, _inputLayout.GetAddressOf());
	}

	if (entry.PS)
	{
		D3DCompileFromFile(shaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
			, "PS_MAIN", "ps_5_0", 0, 0
			, _PSBlob.GetAddressOf()
			, _ErrorBlob.GetAddressOf());

		DEVICE->CreatePixelShader(_PSBlob->GetBufferPointer()
			, _PSBlob->GetBufferSize()
			, nullptr
			, _PS.GetAddressOf());
	}
}
void Shader::BindShader()
{
		CONTEXT->IASetPrimitiveTopology(_info.topology);
		CONTEXT->IASetInputLayout(_inputLayout.Get());

		CONTEXT->VSSetShader(_VS.Get(), nullptr, 0);
		CONTEXT->PSSetShader(_PS.Get(), nullptr, 0);

		ComPtr<ID3D11RasterizerState> rs = rasterizerStates[(UINT)_info.rst];
		ComPtr<ID3D11DepthStencilState> ds = depthstencilStates[(UINT)_info.dst];
		ComPtr<ID3D11BlendState> bs = blendStates[(UINT)_info.bst];

		CONTEXT->RSSetState(rs.Get());
		CONTEXT->OMSetDepthStencilState(ds.Get(),0);
		CONTEXT->OMSetBlendState(bs.Get(),nullptr, 0xffffffff);
}

