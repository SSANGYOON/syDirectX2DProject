#pragma once
#include "Resource.h"
class Mesh : public Resource
{
public:
	Mesh();
	virtual ~Mesh();

	virtual HRESULT Load(const std::wstring& path) override;

public:
	void CreateVertexBuffer(void* data, UINT count);
	void CreateIndexBuffer(void* data, UINT count);

	void BindBuffer();
	void Render();

private:
	ComPtr<ID3D11Buffer> _vertexBuffer;
	ComPtr<ID3D11Buffer> _indexBuffer;
	UINT _indexes;
};

