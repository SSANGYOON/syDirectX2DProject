#pragma once
#include "Resource.h"
class Mesh : public Resource
{
public:
	Mesh();
	virtual ~Mesh();

	virtual HRESULT Load(const std::wstring& path) override;

public:
	void CreateVertexBuffer(void* data, UINT count, D3D11_USAGE usage = D3D11_USAGE::D3D11_USAGE_DEFAULT);
	void SetVertexData(void* data, UINT count);
	void CreateIndexBuffer(void* data, UINT count);

	void SetIndexSize(UINT index) { _indexes = 0; }
	void BindBuffer();
	void Render();
	void RenderIndexed(UINT count);

private:
	ComPtr<ID3D11Buffer> _vertexBuffer;
	ComPtr<ID3D11Buffer> _indexBuffer;
	UINT _indexes;
};

