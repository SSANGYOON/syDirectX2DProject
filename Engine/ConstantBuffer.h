#pragma once
enum class Constantbuffer_Type : UINT8
{
	TRANSFORM,
	MATERIAL,
	GRID,
	//LIGHT,
	
	
	END
};

CBUFFER(TransformCB, CBSLOT_TRANSFORM)
{
	Matrix world;
	Matrix view;
	Matrix projection;
};

CBUFFER(MaterialCB, CBSLOT_MATERIAL)
{
	int iData;
	float fData;
	Vector2 xy;
	Vector3 xyz;
	Vector4 xyzw;
	Matrix matrix;
};

CBUFFER(GridCB, CBSLOT_GRID)
{
	Vector4 cameraPosition;
	Vector2 cameraScale;
	Vector2 resolution;
};

class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();

	void Init(Constantbuffer_Type type, UINT size);
	void SetData(void* Data);
	void SetPipline(ShaderStage stage);

private:
	Constantbuffer_Type _type;
	ComPtr<ID3D11Buffer> _buffer;
	D3D11_BUFFER_DESC desc;
};

