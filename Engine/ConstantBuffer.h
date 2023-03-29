#pragma once
enum class Constantbuffer_Type : UINT8
{
	TRANSFORM,
	SPRITE,
	GRID,
	MATERIAL,
	
	//LIGHT,	
	END
};

CBUFFER(TransformCB, CBSLOT_TRANSFORM)
{
	Matrix world;
	Matrix view;
	Matrix projection;
};

CBUFFER(SpriteCB, CBSLOT_SPRITE)
{
	Vector2 sourceOffset;
	Vector2 sourceSize;
	Vector2 sourceSheetSize;
	Vector2 targetOffset;
	Vector2 targetSizeRatio;
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

