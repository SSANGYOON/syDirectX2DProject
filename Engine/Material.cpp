#include "pch.h"
#include "Material.h"
#include "Texture.h"
#include "Shader.h"
#include "Resources.h"
#include "Engine.h"
#include "ConstantBuffer.h"
#include "ComputeShader.h"

Material::Material()
	:Resource(RESOURCE_TYPE::MATERIAL)
{
	_params = MaterialCB();
}

Material::~Material()
{
}

HRESULT Material::Load(const std::wstring& path, bool stockObject)
{
    return E_NOTIMPL;
}

void Material::Bind()
{
	ConstantBuffer* cb = GEngine->GetConstantBuffer(Constantbuffer_Type::MATERIAL).get();


	cb->SetData(&_params);
	cb->SetPipline(ShaderStage::VS);
	cb->SetPipline(ShaderStage::PS);
    cb->SetPipline(ShaderStage::GS);


    for(UINT i=0; i < MAX_TEXTURE_COUNT;i++){
        if (_textures[i] == nullptr)
            continue;

        _textures[i]->BindSRV(ShaderStage::VS, i);
        _textures[i]->BindSRV(ShaderStage::GS, i);
        _textures[i]->BindSRV(ShaderStage::PS, i);
	}
    _shader->BindShader();
}

void Material::Clear()
{
    for (UINT i = 0; i < MAX_TEXTURE_COUNT; i++) {
        if (_textures[i] == nullptr)
            continue;

        _textures[i]->ClearSRV(ShaderStage::VS, i);
        _textures[i]->ClearSRV(ShaderStage::GS, i);
        _textures[i]->ClearSRV(ShaderStage::PS, i);
    }
}

void Material::Dispatch()
{
    ConstantBuffer* cb = GEngine->GetConstantBuffer(Constantbuffer_Type::MATERIAL).get();
    cb->SetData(&_params);
    cb->SetPipline(ShaderStage::CS);
    _computeShader->Dispatch();
}
