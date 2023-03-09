#include "pch.h"
#include "Material.h"
#include "Texture.h"
#include "Shader.h"
#include "Engine.h"

Material::Material()
	:Resource(Resource_Type::MATERIAL)
{
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
	if (_texture)
		_texture->BindSRV(ShaderStage::PS,0);

	_shader->BindShader();
}