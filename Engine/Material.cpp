#include "pch.h"
#include "Material.h"
#include "Texture.h"
#include "Shader.h"
#include "Engine.h"
#include "ConstantBuffer.h"

Material::Material()
	:Resource(Resource_Type::MATERIAL)
{
	_params = MaterialCB();
}

Material::~Material()
{
}

HRESULT Material::Load(const std::wstring& path)
{
	return E_NOTIMPL;
}

void Material::Bind()
{
	ConstantBuffer* cb = GEngine->GetConstantBuffer(Constantbuffer_Type::MATERIAL).get();
	cb->SetData(&_params);
	cb->SetPipline(ShaderStage::VS);
	cb->SetPipline(ShaderStage::PS);


	for (size_t i = 0; i < _textures.size(); i++)
	{
		if (_textures[i] == nullptr)
			continue;
		_textures[i]->BindSRV(ShaderStage::VS,i);
		_textures[i]->BindSRV(ShaderStage::PS, i);
	}
	_shader->BindShader();
}