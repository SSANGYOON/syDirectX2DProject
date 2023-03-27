#pragma once
#include "Resources.h"
class ComputeShader : public Resource
{
public:
	ComputeShader();
	~ComputeShader();

	virtual HRESULT Load(const std::wstring& path) override;

	bool Create(const std::wstring& file);
	void Dispatch();

protected:
	ComPtr<ID3DBlob> _CSBlob;
	ComPtr<ID3D11ComputeShader> _CS;

	UINT mThreadGroupCountX;
	UINT mThreadGroupCountY;
	UINT mThreadGroupCountZ;
};

