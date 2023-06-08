#pragma once
enum class Constantbuffer_Type : UINT8
{
	TRANSFORM,
	MATERIAL,
	VISUALEFFECT,
	LIGHT,
	PARTICLE,
	BLOOM,
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

struct alignas(16) LightInfo
{
	Vector3 dir;
	float SpotlightFov;
	Vector3 position;
	float range;
	Vector3 color;
	UINT type;
};

CBUFFER(VECB, CBSLOT_VISUALEFFECT)
{
	float time;
	float DeltaTime;
	Vector2 ViewPort;
};

CBUFFER(LightCB, CBSLOT_LIGHT)
{
	UINT		lightCount;
	Vector3		padding;
	LightInfo	lights[50];
};

CBUFFER(ParticleCB, CBSLOT_PARTICLE)
{
	Vector2 Position, PositionVariation;

	Vector2 Velocity, VelocityVariation, VelocityEnd;

	Vector2 SizeBegin, SizeEnd, SizeVariation;

	Vector4 ColorBegin, ColorEnd;
	Vector4 EmissionBegin, EmissionEnd;

	Matrix worldTrans;

	float LifeTime;

	UINT addCount;
	UINT PositionPolar;
	UINT VelocityPolar;

	UINT maxParticles;
	UINT textureAttach;
	Vector2 texturePos;

	UINT useLocalCoord;
	UINT useAliveZone;
	Vector2 aliveZone;
};

CBUFFER(BloomCB, CBSLOT_BLOOM)
{
	float bloomThreshold;
	float bloomIntensity;
	Vector2 texSize;
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

