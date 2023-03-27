#include "pch.h"
#include "ComputeShader.h"
#include "Engine.h"

ComputeShader::ComputeShader()
    : Resource(Resource_Type::COMPUTESHADER)
	, _CSBlob(nullptr)
	, _CS(nullptr)
	, mThreadGroupCountX(32)
	, mThreadGroupCountY(32)
	, mThreadGroupCountZ(1)
{
}

ComputeShader::~ComputeShader()
{
}

HRESULT ComputeShader::Load(const std::wstring& path)
{
	return E_NOTIMPL;
}

bool ComputeShader::Create(const std::wstring& file)
{
	ComPtr<ID3DBlob> mErrorBlob = nullptr;

	std::filesystem::path path = std::filesystem::current_path().parent_path();
	path += "\\SHADER_SOURCE\\";

	std::wstring shaderPath(path.c_str());
	shaderPath += file;

	D3DCompileFromFile(shaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, "CS_MAIN", "cs_5_0", 0, 0
		, _CSBlob.GetAddressOf()
		, mErrorBlob.GetAddressOf());

	DEVICE->CreateComputeShader(_CSBlob->GetBufferPointer()
		, _CSBlob->GetBufferSize()
		, nullptr
		, _CS.GetAddressOf());

	return true;
}

void ComputeShader::Dispatch()
{
	UINT i = 0;
	CONTEXT->CSSetShader(_CS.Get(), nullptr, 0);
	CONTEXT->Dispatch(mThreadGroupCountX, mThreadGroupCountY, mThreadGroupCountZ);
	ID3D11UnorderedAccessView* p = nullptr;
	CONTEXT->CSSetUnorderedAccessViews(0, 1, &p, &i);
}
