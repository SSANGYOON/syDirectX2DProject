#pragma once
#include "Resource.h"
#include "Texture.h"

class Mesh;
class Shader;
enum : UINT8
{
	MATERIAL_ARG_COUNT = 4,
	MAX_TEXTURE_COUNT = 8
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

	void SetInt(UINT8 index, UINT32 value) { intParams[index] = value; }
	void SetFloat(UINT8 index, float value) { floatParams[index] = value; }
	void SetVec2(UINT8 index, Vector2 value) { vec2Params[index] = value; }
	void SetVec4(UINT8 index, Vector4 value) { vec4Params[index] = value; }
	void SetMatrix(UINT8 index, Matrix& value) { matrixParams[index] = value; }

	array<UINT32, MATERIAL_ARG_COUNT> intParams;
	array<float, MATERIAL_ARG_COUNT> floatParams;
	array<Vector2, MATERIAL_ARG_COUNT> vec2Params;
	array<Vector4, MATERIAL_ARG_COUNT> vec4Params;
	array<Matrix, MATERIAL_ARG_COUNT> matrixParams;
};

class Material : public Resource
{
public :
	Material();
	virtual ~Material();

public:
	virtual HRESULT Load(const std::wstring& path) override;

	void SetInt(UINT8 index, UINT32 value) { _params.SetInt(index, value); }
	void SetFloat(UINT8 index, float value) { _params.SetFloat(index, value); }
	void SetVec2(UINT8 index, Vector2 value) { _params.SetVec2(index, value); }
	void SetVec4(UINT8 index, Vector4 value) { _params.SetVec4(index, value); }
	void SetMatrix(UINT8 index, Matrix& value) { _params.SetMatrix(index, value); }

	void SetTexture(UINT8 index, shared_ptr<Texture> texture) { _textures[index] = texture;}

	void SetShader(UINT8 index, shared_ptr<Shader> shader) { _shaders[index] = shader; }

	shared_ptr<Texture> GetTexture(UINT index) { return _textures[index]; }

	void Render(shared_ptr<Mesh> mesh);

private:
	MaterialCB		_params;
	vector<shared_ptr<Shader>> _shaders;
	vector<vector<int>> _inputTextureIndex;
	vector<int> _targetTextureIndex;
	array<shared_ptr<Texture>, MAX_TEXTURE_COUNT> _textures;
};

