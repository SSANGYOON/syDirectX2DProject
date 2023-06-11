#include "pch.h"
#include "Resources.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "ComputeShader.h"

void Resources::CreateDefaultResource()
{
#pragma region POINT MESH
	Vertex v = {};
	v.pos.w = 1.f;
	std::shared_ptr<Mesh> pointMesh = std::make_shared<Mesh>();
	Resources::Insert<Mesh>(L"PointMesh", pointMesh);
	pointMesh->CreateVertexBuffer(&v, 1);
	UINT pointIndex = 0;
	pointMesh->CreateIndexBuffer(&pointIndex, 1);
#pragma endregion

#pragma region Default Rect
	shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	Resources::Insert<Mesh>(L"RectMesh", mesh);
	Vertex vertexes[4] = {};

	vertexes[0].pos = Vector4(-0.5f, 0.5f, 0.f, 1.0f);
	vertexes[0].uv = Vector2(0.f, 0.f);

	vertexes[1].pos = Vector4(0.5f, 0.5f, 0.f, 1.0f);
	vertexes[1].uv = Vector2(1.0f, 0.0f);

	vertexes[2].pos = Vector4(0.5f, -0.5f, 0.f, 1.0f);
	vertexes[2].uv = Vector2(1.0f, 1.0f);

	vertexes[3].pos = Vector4(-0.5f, -0.5f, 0.f, 1.0f);
	vertexes[3].uv = Vector2(0.0f, 1.0f);

	mesh->CreateVertexBuffer(vertexes, 4);

	std::vector<UINT> indexes;
	indexes.push_back(0);
	indexes.push_back(1);
	indexes.push_back(2);

	indexes.push_back(2);
	indexes.push_back(3);
	indexes.push_back(0);

	mesh->CreateIndexBuffer(indexes.data(), (UINT)indexes.size());
#pragma endregion

#pragma region OutlinedRect

	shared_ptr<Mesh> outlinedRect = std::make_shared<Mesh>();
	Resources::Insert<Mesh>(L"OutlinedRect", outlinedRect);
	outlinedRect->CreateVertexBuffer(vertexes, 4);

	outlinedRect->CreateIndexBuffer(indexes.data(), (UINT)indexes.size());
#pragma endregion

#pragma region DebugCircle
	{
		vector<Vertex> circleVertexes;
		Vertex center = {};
		center.pos = Vector4(0.0f, 0.0f, 0.f, 1.0f);
		center.uv = Vector2::Zero;

		circleVertexes.push_back(center);

		int iSlice = 80;
		float fRadius = 0.5f;
		float fTheta = XM_2PI / (float)iSlice;


		for (size_t i = 0; i < iSlice; i++)
		{
			Vertex vtx = {};
			vtx.pos = Vector4
			(
				fRadius * cosf(fTheta * (float)i)
				, fRadius * sinf(fTheta * (float)i)
				, -0.00001f, 1.0f
			);

			circleVertexes.push_back(vtx);
		}

		vector<UINT> circleIndexes;
		for (size_t i = 0; i < iSlice; i++)
		{
			circleIndexes.push_back(i + 1);
		}
		circleIndexes.push_back(1);

		std::shared_ptr<Mesh> cirlceMesh = std::make_shared<Mesh>();
		Resources::Insert<Mesh>(L"CircleMesh", cirlceMesh);
		cirlceMesh->CreateVertexBuffer(circleVertexes.data(), circleVertexes.size());
		cirlceMesh->CreateIndexBuffer(circleIndexes.data(), circleIndexes.size());
	}
#pragma endregion

#pragma region 2DCircle
	{
		vector<Vertex> circleVertexes;
		Vertex center = {};
		center.pos = Vector4(0.0f, 0.0f, 0.1f, 1.0f);
		center.uv = Vector2::Zero;

		circleVertexes.push_back(center);

		int iSlice = 120;
		float fRadius = 1.f;
		float fTheta = XM_2PI / (float)iSlice;


		for (size_t i = 0; i < iSlice; i++)
		{
			Vertex vtx = {};
			vtx.pos = Vector4
			(
				fRadius * cosf(fTheta * (float)i) / 2.f
				, fRadius * sinf(fTheta * (float)i) / 2.f
				, 0.1f, 1.0f
			);

			circleVertexes.push_back(vtx);
		}

		vector<UINT> circleIndexes;
		for (size_t i = 1; i <= iSlice; i++)
		{
			circleIndexes.push_back(i);
			circleIndexes.push_back( i % iSlice + 1);
			circleIndexes.push_back(0);
		}
		
		std::shared_ptr<Mesh> cirlceMesh = std::make_shared<Mesh>();
		Resources::Insert<Mesh>(L"Circle2D", cirlceMesh);
		cirlceMesh->CreateVertexBuffer(circleVertexes.data(), (UINT)circleVertexes.size());
		cirlceMesh->CreateIndexBuffer(circleIndexes.data(), circleIndexes.size());
	}
#pragma endregion

#pragma region DefaultShader
	ShaderInfo _info;
	ShaderEntry _entry;

#pragma endregion

#pragma region DebugShader
	shared_ptr<Shader> debugShader = std::make_shared<Shader>();
	Resources::Insert<Shader>(L"DebugShader", debugShader);
	_info.bst = BSType::AlphaBlend;
	_info.dst = DSType::Less;
	_info.rst = RSType::WireframeNone;
	_info.topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
	_entry = {};
	_entry.VS = true;
	_entry.PS = true;
	debugShader->CreateShader(_info, _entry, L"DebugShader.hlsl");
#pragma 

#pragma region CircleShader
	shared_ptr<Shader> circleShader = std::make_shared<Shader>();
	Resources::Insert<Shader>(L"CircleShader", circleShader);
	_info.bst = BSType::AlphaBlend;
	_info.dst = DSType::Less;
	_info.rst = RSType::SolidNone;
	_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	_entry = {};
	_entry.VS = true;
	_entry.PS = true;
	circleShader->CreateShader(_info, _entry, L"DebugShader.hlsl");
#pragma endregion

#pragma region TrailShader
	shared_ptr<Shader> trailShader = std::make_shared<Shader>();
	Resources::Insert<Shader>(L"TrailShader", trailShader);
	_info.bst = BSType::AlphaBlend;
	_info.dst = DSType::Less;
	_info.rst = RSType::SolidNone;
	_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	_entry = {};
	_entry.VS = true;
	_entry.PS = true;
	trailShader->CreateShader(_info, _entry, L"TrailShader.hlsl", true);
#pragma endregion

#pragma region ParticleCompute
	shared_ptr<ComputeShader> ParticleCompute = std::make_shared<ComputeShader>();
	Resources::Insert<ComputeShader>(L"ParticleShader", ParticleCompute);
	ParticleCompute->Create(L"ParticleCS.hlsl");
#pragma endregion

#pragma region SpriteShader
	{
		shared_ptr<Shader> spriteShader = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"SpriteFoward", spriteShader);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::AlphaBlend;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		spriteShader->CreateShader(_info, _entry, L"DefaultSpriteFoward.hlsl");
	}
#pragma endregion

#pragma region Halo
	{
		shared_ptr<Shader> haloShader = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"HaloShader", haloShader);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::AlphaBlend;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		haloShader->CreateShader(_info, _entry, L"Halo.hlsl");
	}
#pragma endregion

#pragma region DefferedShader
	{
		shared_ptr<Shader> defferedShader = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"SpriteDeffered", defferedShader);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::Default;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		defferedShader->CreateShader(_info, _entry, L"DefaultSpriteDeffered.hlsl");
	}
#pragma endregion

#pragma region LightTingShader
	{
		shared_ptr<Shader> lightingShader = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"Lighting", lightingShader);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::Default;
		_info.dst = DSType::NoWrite;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		lightingShader->CreateShader(_info, _entry, L"Lighting.hlsl");
	}
#pragma endregion

#pragma region GlowSword
	{
		shared_ptr<Shader> glowSword = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"GlowSword", glowSword);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::AlphaBlend;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		glowSword->CreateShader(_info, _entry, L"GlowSword.hlsl");
	}
#pragma endregion

#pragma region UIShader
	{
		shared_ptr<Shader> uiShader = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"UIShader", uiShader);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::AlphaBlend;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		_entry.GS = true;
		uiShader->CreateShader(_info, _entry, L"UIShader.hlsl");
	}
#pragma endregion

#pragma region GaugeShader
	{
		shared_ptr<Shader> GaugeShader = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"GaugeShader", GaugeShader);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::AlphaBlend;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		GaugeShader->CreateShader(_info, _entry, L"GaugeShader.hlsl");
	}
#pragma endregion

#pragma region SlotShader
	{
		shared_ptr<Shader> SlotShader = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"SlotShader", SlotShader);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::AlphaBlend;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		SlotShader->CreateShader(_info, _entry, L"SlotShader.hlsl");
	}
#pragma endregion

#pragma region IconShader
	{
		shared_ptr<Shader> IconShader = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"IconShader", IconShader);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::AlphaBlend;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		IconShader->CreateShader(_info, _entry, L"IconShader.hlsl");
	}
#pragma endregion

#pragma region SkyShader
{
		shared_ptr<Shader> skyShader = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"SkyShader", skyShader);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::AlphaBlend;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		skyShader->CreateShader(_info, _entry, L"Sky.hlsl");
}
#pragma endregion

#pragma region WingShader
{
		shared_ptr<Shader> wingShader = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"WingShader", wingShader);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::AlphaBlend;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		wingShader->CreateShader(_info, _entry, L"WingShader.hlsl");
}
#pragma endregion

#pragma region Eraser
	{
		shared_ptr<Shader> eraser = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"Eraser", eraser);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::AlphaBlend;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		eraser->CreateShader(_info, _entry, L"Eraser.hlsl");
	}
#pragma endregion

#pragma region ParticleGraphic
	{
		shared_ptr<Shader> particle = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"ParticleGraphics", particle);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::AlphaBlend;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		_entry.GS = true;
		particle->CreateShader(_info, _entry, L"Particle.hlsl", true);
	}
#pragma endregion

#pragma region ThunderParticle
	{
		shared_ptr<Shader> particle = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"ThunderParticle", particle);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::AlphaBlend;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		_entry.GS = true;
		particle->CreateShader(_info, _entry, L"ThunderParticle.hlsl", true);
	}
#pragma endregion

#pragma region Resize
	{
		shared_ptr<Shader> resizer = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"Resizer", resizer);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::Default;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		resizer->CreateShader(_info, _entry, L"Downscale.hlsl");
	}
#pragma endregion

#pragma region HorizonalBlur
	shared_ptr<ComputeShader> HorigentalShader = std::make_shared<ComputeShader>();
	Resources::Insert<ComputeShader>(L"HorizonalBlurShader", HorigentalShader);
	HorigentalShader->Create(L"Horizonal.hlsl");
#pragma endregion

#pragma region VerticalBlur
	shared_ptr<ComputeShader> VerticalBlurShadr = std::make_shared<ComputeShader>();
	Resources::Insert<ComputeShader>(L"VerticalBlurShader", VerticalBlurShadr);
	VerticalBlurShadr->Create(L"VerticalBlur.hlsl");
#pragma endregion

#pragma region BloomCombline
	{
		shared_ptr<Shader> BloomCombline = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"BloomCombline", BloomCombline);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::Default;
		_info.dst = DSType::None;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		BloomCombline->CreateShader(_info, _entry, L"Combine.hlsl");
	}
#pragma endregion

#pragma region ACESMap
	{
		shared_ptr<Shader> ACESMap =  std::make_shared<Shader>();
		Resources::Insert<Shader>(L"ACESMap", ACESMap);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::Default;
		_info.dst = DSType::None;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		ACESMap->CreateShader(_info, _entry, L"ACESMap.hlsl");
	}
#pragma endregion

#pragma region SmoothErase
	{
		shared_ptr<Shader> serase = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"SmoothErase", serase);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::AlphaBlend;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		serase->CreateShader(_info, _entry, L"SpriteSmoothErase.hlsl");
	}
#pragma endregion

#pragma region Thunder
	{
		shared_ptr<Shader> thunder = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"Thunder", thunder);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::AlphaBlend;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		thunder->CreateShader(_info, _entry, L"ThunderShader.hlsl");
	}
#pragma endregion
#pragma region Laser
	{
		shared_ptr<Shader> laser = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"Laser", laser);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::AlphaBlend;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		laser->CreateShader(_info, _entry, L"LaserShader.hlsl", true);
	}
#pragma endregion

#pragma region ChainDissolve
	{
		shared_ptr<Shader> chainDissolve = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"ChainDissolve", chainDissolve);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::AlphaBlend;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		chainDissolve->CreateShader(_info, _entry, L"ChainDissolve.hlsl");
	}
#pragma endregion

#pragma region DarkLady
	{
		shared_ptr<Shader> darkLady = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"DarkLady", darkLady);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::Default;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		darkLady->CreateShader(_info, _entry, L"DarkLady.hlsl");
	}
#pragma endregion

#pragma region DarkLady2
	{
		shared_ptr<Shader> darkLady2 = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"DarkLady2", darkLady2);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::Default;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		darkLady2->CreateShader(_info, _entry, L"DarkLady2.hlsl");
	}
#pragma endregion

#pragma region BloomBuffers
	{
		shared_ptr<Texture> downScaledEmission = std::make_shared<Texture>();
		downScaledEmission->Create(480, 270, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE);
		Resources::Insert<Texture>(L"DownScaledEmission", downScaledEmission);
		shared_ptr<Texture> VerticalBlur = std::make_shared<Texture>();
		VerticalBlur->Create(480, 270, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE);
		Resources::Insert<Texture>(L"VerticalBlur", VerticalBlur);
		shared_ptr<Texture> HorizontalBlur = std::make_shared<Texture>();
		HorizontalBlur->Create(480, 270, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE);
		Resources::Insert<Texture>(L"HorizontalBlur", HorizontalBlur);
	}
#pragma endregion
}

vector<shared_ptr<Shader>> Resources::GetShaders()
{
	vector<shared_ptr<Shader>> vec;

	for (auto& res : _resources)
	{
		if (res.second->_type == RESOURCE_TYPE::GRAPHIC_SHADER)
		{
			vec.push_back(static_pointer_cast<Shader>(res.second));
		}
	}
	return vec;
}
