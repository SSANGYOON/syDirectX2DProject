#include "pch.h"
#include "Mesh.h"
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

HRESULT Material::Load(const std::wstring& path)
{
    for (int i = 0; i < MAX_TEXTURE_COUNT; i++)
    {
        _textures[i] = nullptr;
    }


    ifstream json_f;
    std::filesystem::path parentPath = std::filesystem::current_path().parent_path();

    std::string fullPath = parentPath.string() + "\\Resources\\" + std::string(path.begin(),path.end());
    json_f.open(fullPath);
    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;
    Json::Value value;

    JSONCPP_STRING errs;
    assert(parseFromStream(builder, json_f, &value, &errs));
    json_f.close();

    auto Textures = value["Textures"];
    for (auto texture : Textures)
    {
        int id = texture["ID"].asUInt();
        string textureKey = texture["KEY"].asCString();
        string texturePath = texture["Path"].asCString();
        _textures[id] = GET_SINGLE(Resources)->Load<Texture>(wstring(textureKey.begin(), textureKey.end()), wstring(texturePath.begin(), texturePath.end()));
    }

    auto ShaderItem = value["Shader"];
    auto shaderResource = ShaderItem["ShaderResource"];
    int itemID = ShaderItem["ID"].asInt();
        
    const char* cshaderKey = ShaderItem["KEY"].asCString();
    wstring shaderKey = wstring(cshaderKey, cshaderKey + strlen(cshaderKey));

    _shader = make_shared<Shader>();
    ShaderInfo info = {};
    info.bst = (BSType)ShaderItem["ShaderInfo"]["BSType"].asUInt();
    info.dst = (DSType)ShaderItem["ShaderInfo"]["DSType"].asUInt();
    info.rst = (RSType)ShaderItem["ShaderInfo"]["RSType"].asUInt();
    info.topology = (D3D11_PRIMITIVE_TOPOLOGY)ShaderItem["ShaderInfo"]["Topology"].asUInt();

    ShaderEntry entry = {};
    for (auto sp : ShaderItem["ShaderPhases"])
    {
        string str = sp.asCString();
        if (str.compare("VS") == 0)
            entry.VS = true;
        if (str.compare("PS") == 0)
            entry.PS = true;
        if (str.compare("GS") == 0)
            entry.GS = true;
        if (str.compare("DS") == 0)
            entry.DS = true;
        if (str.compare("HS") == 0)
            entry.HS = true;
    }
    string shaderpath = ShaderItem["Path"].asString();
    _shader->CreateShader(info, entry, wstring(shaderpath.begin(), shaderpath.end()));
    GET_SINGLE(Resources)->Insert<Shader>(shaderKey, _shader);

    return S_OK;
}

void Material::Bind()
{
	ConstantBuffer* cb = GEngine->GetConstantBuffer(Constantbuffer_Type::MATERIAL).get();


	cb->SetData(&_params);
	cb->SetPipline(ShaderStage::VS);
	cb->SetPipline(ShaderStage::PS);


    for(UINT i=0; i < MAX_TEXTURE_COUNT;i++){
        if (_textures[i] == nullptr)
            continue;

        _textures[i]->BindSRV(ShaderStage::VS, i);
        _textures[i]->BindSRV(ShaderStage::GS, i);
        _textures[i]->BindSRV(ShaderStage::PS, i);
	}
    _shader->BindShader();
}

void Material::Dispatch()
{
    ConstantBuffer* cb = GEngine->GetConstantBuffer(Constantbuffer_Type::MATERIAL).get();
    cb->SetData(&_params);
    cb->SetPipline(ShaderStage::CS);
    _computeShader->Dispatch();
}
