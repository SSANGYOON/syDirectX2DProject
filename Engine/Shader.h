#pragma once
#include "Resource.h"
#include "PipelineStates.h"

struct ShaderEntry
{
	bool VS;
	bool HS;
	bool GS;
	bool DS;
	bool CS;
	bool PS;
};

struct ShaderInfo
{
	BSType bst;
	DSType dst;
	RSType rst;
	D3D11_PRIMITIVE_TOPOLOGY topology;
};
class Shader : public Resource
{
public:
	Shader();
	virtual ~Shader();

	virtual HRESULT Load(const wstring& path) override;

	void CreateShader(const ShaderInfo& info,const ShaderEntry& entry, const wstring& file);
	void BindShader();

private:
	ShaderInfo _info;
	
	ComPtr<ID3D11InputLayout> _inputLayout;
	ComPtr<ID3DBlob> _VSBlob;
	ComPtr<ID3DBlob> _HSBlob;
	ComPtr<ID3DBlob> _DSBlob;
	ComPtr<ID3DBlob> _GSBlob;
	ComPtr<ID3DBlob> _PSBlob;

	ComPtr<ID3D11VertexShader> _VS;
	ComPtr<ID3D11HullShader> _HS;
	ComPtr<ID3D11DomainShader> _DS;
	ComPtr<ID3D11GeometryShader> _GS;
	ComPtr<ID3D11PixelShader> _PS;

	ComPtr<ID3DBlob> _ErrorBlob;
};

