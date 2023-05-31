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
    _params = {};
}

Material::~Material()
{
}

HRESULT Material::Load(const std::wstring& path, bool stockObject)
{
    return E_NOTIMPL;
}

void Material::SetTexture(UINT8 index, shared_ptr<Texture> texture)
{
    _textures[index] = texture; 
    
    if(texture){
        _params.texOn[index] = 1; 
        _params.texSizes[index] = texture->GetSize(); 
    }
    else{
        _params.texOn[index] = 0;
        _params.texSizes[index] = Vector2::Zero;
    }
}

UINT64 Material::GetInstanceID()
{
    InstanceID instanceID = {};
    if(_textures[0] == nullptr)
        instanceID = { _shader->GetId(), 0};
    else
        instanceID = { _shader->GetId(), _textures[0]->GetId()};

    return instanceID.id;
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
