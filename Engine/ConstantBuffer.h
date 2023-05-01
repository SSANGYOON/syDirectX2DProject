#pragma once
enum class Constantbuffer_Type : UINT8
{
	TRANSFORM,
	SPRITE,
	MATERIAL,
	LIGHT,
	PANEL,
	SLIDER,
	SLOT,
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

CBUFFER(SpriteCB, CBSLOT_SPRITE)
{
	Vector2 sourceOffset;
	Vector2 sourceSize;
	Vector2 sourceSheetSize;
	Vector2 targetOffset;
	Vector2 targetSizeRatio;
	UINT textured;
	UINT lighted;
	Vector4 color;
};

CBUFFER(PanelCB, CBSLOT_PANEL)
{
	Vector2 size;
	Vector2 originalSize;
	Vector2 offset;
	Vector2 padding;
};

CBUFFER(SliderCB, CBSLOT_SLIDER)
{
	Vector2 barSize;
	Vector2 gaugeSize;
	float currentValue;
	float maxValue;
	Vector2 padding;
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

