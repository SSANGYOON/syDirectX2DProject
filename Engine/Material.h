#pragma once
#include "Resource.h"
#include "Texture.h"

class Mesh;
class Shader;
class ComputeShader;

enum : UINT8
{
	MATERIAL_ARG_COUNT = 4,
	MAX_TEXTURE_COUNT = 4
};

struct MaterialCB
{
	MaterialCB()
	{
		for (UINT i = 0; i < MATERIAL_ARG_COUNT; i++)
		{
			SetInt(i, 0);
			SetFloat(i, 0.f);
		}
	}

	void SetInt(UINT8 index, int value) { intParams[index] = value; }
	void SetFloat(UINT8 index, float value) { floatParams[index] = value; }
	void SetVec2(UINT8 index, Vector2 value) { vec2Params[index] = value; }
	void SetVec4(UINT8 index, Vector4 value) { vec4Params[index] = value; }

	int GetInt(UINT8 index) { return intParams[index]; }
	float GetFloat(UINT8 index) { return floatParams[index]; }
	Vector2 GetVec2(UINT8 index) { return vec2Params[index]; }
	Vector4 GetVec4(UINT8 index) { return vec4Params[index]; }

	array<UINT32, MATERIAL_ARG_COUNT> texOn;
	array<int, MATERIAL_ARG_COUNT> intParams;
	array<float, MATERIAL_ARG_COUNT> floatParams;
	array<Vector2, MATERIAL_ARG_COUNT> vec2Params;
	array<Vector2, MATERIAL_ARG_COUNT> texSizes;
	array<Vector4, MATERIAL_ARG_COUNT> vec4Params;
};


union InstanceID
{
	struct
	{
		UINT shaderID;
		UINT TextureID;
	};
	UINT64 id;
};

class Material : public Resource
{
public :
	Material();
	virtual ~Material();

public:
	virtual HRESULT Load(const std::wstring& path, bool stockObject) override;

	void SetInt(UINT8 index, int value) { _params.SetInt(index, value); }
	void SetFloat(UINT8 index, float value) { _params.SetFloat(index, value); }
	void SetVec2(UINT8 index, Vector2 value) { _params.SetVec2(index, value); }
	void SetVec4(UINT8 index, Vector4 value) { _params.SetVec4(index, value); }

	int GetInt(UINT8 index) { return _params.GetInt(index); }
	float GetFloat(UINT8 index) { return _params.GetFloat(index); }
	Vector2 GetVec2(UINT8 index) { return _params.GetVec2(index); }
	Vector4 GetVec4(UINT8 index) { return _params.GetVec4(index); }

	void SetTexture(UINT8 index, shared_ptr<Texture> texture);
	void SetShader(shared_ptr<Shader> shader) { _shader = shader; }
	void SetComputeShader(shared_ptr<ComputeShader> computeShader) { _computeShader = computeShader; }

	shared_ptr<Texture> GetTexture(UINT index) { return _textures[index]; }
	shared_ptr<Shader> GetShader() { return _shader; }
	UINT64 GetInstanceID();
	void Bind();
	void Clear();

private:
	MaterialCB		_params;
	shared_ptr<Shader> _shader;
	shared_ptr<ComputeShader> _computeShader;
	array<shared_ptr<Texture>, MAX_TEXTURE_COUNT> _textures;
};

