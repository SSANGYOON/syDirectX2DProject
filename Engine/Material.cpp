#include "pch.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "Shader.h"
#include "Engine.h"
#include "ConstantBuffer.h"


Material::Material()
	:Resource(Resource_Type::MATERIAL)
{
	_params = MaterialCB();
    _inputTextureIndex.resize(1);
    _inputTextureIndex[0].resize(1);
    _inputTextureIndex[0][0] = -1;
    _targetTextureIndex.resize(1);
    _targetTextureIndex[0] = -1;
    _shaders.resize(1);
}

Material::~Material()
{
}

HRESULT Material::Load(const std::wstring& path)
{
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
        string textureType = texture["Type"].asCString();
        if (textureType.compare("Resource") == 0)
        {
            _textures[id] = make_shared<Texture>();
            string path = texture["Path"].asCString();
            _textures[id]->Load(wstring(path.begin(), path.end()));

           
        }
        else if (textureType.compare("Runtime") == 0)
        {
            _textures[id] = make_shared<Texture>();
            _textures[id]->Create(texture["Width"].asUInt(), texture["Height"].asUInt(), DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET);
        }

        SetVec2(0, _textures[id]->GetSize());
    }

    auto ShaderGroup = value["ShaderGroup"];
    _targetTextureIndex.resize(ShaderGroup.size());
    _inputTextureIndex.resize(ShaderGroup.size());
    _shaders.resize(ShaderGroup.size());
    for (int i = 0; i < ShaderGroup.size(); i++)
    {
        auto shaderResource = ShaderGroup[i]["ShaderResource"];
        for (int j = 0; j < shaderResource.size(); j++)
        {
            _inputTextureIndex[i].push_back(shaderResource[j].asInt());
        }
        _targetTextureIndex[i] = ShaderGroup[i]["Target"].asInt();

        _shaders[i] = make_shared<Shader>();
        ShaderInfo info = {};
        info.bst = (BSType)ShaderGroup[i]["ShaderInfo"]["BSType"].asUInt();
        info.dst = (DSType)ShaderGroup[i]["ShaderInfo"]["DSType"].asUInt();
        info.rst = (RSType)ShaderGroup[i]["ShaderInfo"]["RSType"].asUInt();
        info.topology = (D3D11_PRIMITIVE_TOPOLOGY)ShaderGroup[i]["ShaderInfo"]["Topology"].asUInt();

        ShaderEntry entry = {};
        for (auto sp : ShaderGroup[i]["ShaderPhases"])
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
        string path = ShaderGroup[i]["Path"].asString();
        _shaders[i]->CreateShader(info, entry, wstring(path.begin(), path.end()));
    }
    return S_OK;
}

void Material::Render(shared_ptr<Mesh> mesh)
{
	UINT pathlength = _shaders.size();
	ConstantBuffer* cb = GEngine->GetConstantBuffer(Constantbuffer_Type::MATERIAL).get();
	for (UINT i = 0; i < pathlength; i++)
	{
        int target = _targetTextureIndex[i];

		cb->SetData(&_params);
		cb->SetPipline(ShaderStage::VS);
		cb->SetPipline(ShaderStage::PS);

        int t = 0;

        if (target != -1) {
            D3D11_VIEWPORT vp = { 0.f, 0.f,_textures[target]->GetSize().x, _textures[target]->GetSize().y, 0.f, 1.f };
            FLOAT backgroundColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
            CONTEXT->RSSetViewports(1, &vp);
            CONTEXT->OMSetRenderTargets(1, _textures[target]->GetRTVRef(), nullptr);
            CONTEXT->ClearRenderTargetView(_textures[target]->GetRTV(), backgroundColor);
        }
        else
            GEngine->SetOriginalRederTarget();

		for (int j : _inputTextureIndex[i])
		{
            if (j > -1) {
                _textures[j]->BindSRV(ShaderStage::VS, t);
                _textures[j]->BindSRV(ShaderStage::PS, t++);
            }
		}
		
		_shaders[i]->BindShader();
		mesh->BindBuffer();
		mesh->Render();

        t = 0;

        for (int j : _inputTextureIndex[i])
        {
            if (j > -1) {
                _textures[j]->ClearSRV(ShaderStage::VS, t);
                _textures[j]->ClearSRV(ShaderStage::PS, t++);
            }
        }
	}
}
