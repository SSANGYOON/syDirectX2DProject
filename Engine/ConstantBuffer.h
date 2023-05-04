#pragma once
enum class Constantbuffer_Type : UINT8
{
	TRANSFORM,
	MATERIAL,
	LIGHT,
	END
};

CBUFFER(TransformCB, CBSLOT_TRANSFORM)
{
	Matrix world;
	Matrix view;
	Matrix projection;
	int entity;
	Vector3 padding;
};

struct LightInfo
{
	Vector3 dir;
	float angle;
	Vector3 position;
	float range;
	Vector4 color;
	UINT type;
	Vector3 padding;
};

CBUFFER(LightCB, CBSLOT_LIGHT)
{
	UINT		lightCount;
	Vector3		padding;
	LightInfo	lights[50];
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

