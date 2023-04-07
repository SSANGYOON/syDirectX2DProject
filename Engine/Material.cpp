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
    for (int i = 0; i < MAX_TEXTURE_COUNT; i++)
    {
        _textures[i] = nullptr;
    }

    for (int i = 0; i < _shaders.size(); i++)
    {
        _shaders[i] = nullptr;
    }
    _targetTextureIndex.clear();
    _inputTextureIndex.clear();

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
            const char* ctextureKey = texture["KEY"].asCString();
            wstring textureKey = wstring(ctextureKey, ctextureKey + strlen(ctextureKey));

            _textures[id] = GET_SINGLE(Resources)->Find<Texture>(textureKey);

            if (_textures[id] == nullptr) {
                _textures[id] = make_shared<Texture>();
                string path = texture["Path"].asCString();
                _textures[id]->Load(wstring(path.begin(), path.end()));
                GET_SINGLE(Resources)->Insert<Texture>(textureKey, _textures[id]);
            }
            else
                continue;
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
    for (auto ShaderItem : ShaderGroup)
    {
        auto shaderResource = ShaderItem["ShaderResource"];
        int itemID = ShaderItem["ID"].asInt();
        for (auto InputIndex : shaderResource)
        {
            _inputTextureIndex[itemID].push_back(InputIndex.asInt());
        }
        _targetTextureIndex[itemID] = ShaderItem["Target"].asInt();
        
        const char* cshaderKey = ShaderItem["KEY"].asCString();
        wstring shaderKey = wstring(cshaderKey, cshaderKey + strlen(cshaderKey));
        _shaders[itemID] = GET_SINGLE(Resources)->Find<Shader>(shaderKey);

        if (_shaders[itemID] == nullptr) {
            _shaders[itemID] = make_shared<Shader>();
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
            string path = ShaderItem["Path"].asString();
            _shaders[itemID]->CreateShader(info, entry, wstring(path.begin(), path.end()));
            GET_SINGLE(Resources)->Insert<Shader>(shaderKey, _shaders[itemID]);
        }
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

void Material::RenderIndexed(shared_ptr<Mesh> mesh, UINT count)
{
    UINT pathlength = _shaders.size();
    ConstantBuffer* cb = GEngine->GetConstantBuffer(Constantbuffer_Type::MATERIAL).get();
    for (UINT i = 0; i < pathlength; i++)
    {
        int target = _targetTextureIndex[i];

        cb->SetData(&_params);
        cb->SetPipline(ShaderStage::VS);
        cb->SetPipline(ShaderStage::GS);
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
                _textures[j]->BindSRV(ShaderStage::PS, t);
                _textures[j]->BindSRV(ShaderStage::GS, t++);
            }

        }

        _shaders[i]->BindShader();
        mesh->BindBuffer();
        mesh->RenderIndexed(count);

        t = 0;

        for (int j : _inputTextureIndex[i])
        {
            if (j > -1) {
                _textures[j]->ClearSRV(ShaderStage::VS, t);
                _textures[j]->ClearSRV(ShaderStage::PS, t);
                _textures[j]->ClearSRV(ShaderStage::GS, t++);
            }
        }
    }
}

void Material::Dispatch()
{
    ConstantBuffer* cb = GEngine->GetConstantBuffer(Constantbuffer_Type::MATERIAL).get();
    cb->SetData(&_params);
    cb->SetPipline(ShaderStage::CS);
    _computeShader->Dispatch();
}
