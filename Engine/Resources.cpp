#include "pch.h"
#include "Resources.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"
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
	vertexes[0].color = Vector4(0.f, 1.f, 0.f, 1.f);
	vertexes[0].uv = Vector2(0.f, 0.f);

	vertexes[1].pos = Vector4(0.5f, 0.5f, 0.f, 1.0f);
	vertexes[1].color = Vector4(1.f, 1.f, 1.f, 1.f);
	vertexes[1].uv = Vector2(1.0f, 0.0f);

	vertexes[2].pos = Vector4(0.5f, -0.5f, 0.f, 1.0f);
	vertexes[2].color = Vector4(1.f, 0.f, 0.f, 1.f);
	vertexes[2].uv = Vector2(1.0f, 1.0f);

	vertexes[3].pos = Vector4(-0.5f, -0.5f, 0.f, 1.0f);
	vertexes[3].color = Vector4(0.f, 0.f, 1.f, 1.f);
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

#pragma region DebugRect

	shared_ptr<Mesh> debugmesh = std::make_shared<Mesh>();
	Resources::Insert<Mesh>(L"DebugMesh", debugmesh);
	debugmesh->CreateVertexBuffer(vertexes, 4);

	debugmesh->CreateIndexBuffer(indexes.data(), (UINT)indexes.size());
#pragma endregion

#pragma region DebugCircle
	{
		vector<Vertex> circleVertexes;
		Vertex center = {};
		center.pos = Vector4(0.0f, 0.0f, 0.f, 1.0f);
		center.color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
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
			vtx.color = center.color;

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
		cirlceMesh->CreateVertexBuffer(circleVertexes.data(), (UINT)circleVertexes.size());
		cirlceMesh->CreateIndexBuffer(circleIndexes.data(), circleIndexes.size());
	}
#pragma endregion

#pragma region 2DCircle
	{
		vector<Vertex> circleVertexes;
		Vertex center = {};
		center.pos = Vector4(0.0f, 0.0f, 0.1f, 1.0f);
		center.color = Vector4(0.0f, 1.0f, 0.f, 1.0f);
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
				fRadius * cosf(fTheta * (float)i)
				, fRadius * sinf(fTheta * (float)i)
				, 0.1f, 1.0f
			);
			vtx.color = center.color;

			circleVertexes.push_back(vtx);
		}

		vector<UINT> circleIndexes;
		for (size_t i = 0; i < iSlice; i++)
		{
			circleIndexes.push_back(0);
			circleIndexes.push_back(1 + i);
			circleIndexes.push_back(1 + (i + 1) % iSlice);
		}

		std::shared_ptr<Mesh> cirlceMesh = std::make_shared<Mesh>();
		Resources::Insert<Mesh>(L"Circle2D", cirlceMesh);
		cirlceMesh->CreateVertexBuffer(circleVertexes.data(), (UINT)circleVertexes.size());
		cirlceMesh->CreateIndexBuffer(circleIndexes.data(), circleIndexes.size());
	}
#pragma endregion

#pragma region DefaultShader
	shared_ptr<Shader> shader = std::make_shared<Shader>();
	Resources::Insert<Shader>(L"DefaultShader", shader);
	ShaderInfo _info;
	ShaderEntry _entry;
	_info.bst = BSType::AlphaBlend;
	_info.dst = DSType::Less;
	_info.rst = RSType::SolidNone;
	_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	_entry = {};
	_entry.VS = true;
	_entry.PS = true;
	shader->CreateShader(_info, _entry, L"Default.hlsl");
	shared_ptr<Material> material = make_shared<Material>();
	material->SetShader(0,shader);
#pragma endregion
{
#pragma region TrailShader
		shared_ptr<Shader> TrailShader = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"TrailShader", TrailShader);
		ShaderInfo _info;
		_info = {};
		ShaderEntry _entry;
		_info.bst = BSType::AlphaBlend;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		TrailShader->CreateShader(_info, _entry, L"TrailShader.hlsl");
#pragma endregion
}
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
	shared_ptr<Material> debugMaterial = make_shared<Material>();
	debugMaterial->SetShader(0,debugShader);
	Resources::Insert<Material>(L"DebugMaterial", debugMaterial);
#pragma endregion

#pragma region GridMaterial
	shared_ptr<Shader> gridShader = std::make_shared<Shader>();
	_info.bst = BSType::AlphaBlend;
	_info.dst = DSType::NoWrite;
	_info.rst = RSType::SolidNone;
	_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	_entry = {};
	_entry.VS = true;
	_entry.PS = true;
	gridShader->CreateShader(_info, _entry, L"Grid.hlsl");
	Resources::Insert<Shader>(L"GridShader", gridShader);
#pragma endregion

#pragma region ParticleCompute
	shared_ptr<ComputeShader> ParticleCompute = std::make_shared<ComputeShader>();
	Resources::Insert<ComputeShader>(L"ParticleShader", ParticleCompute);
	ParticleCompute->Create(L"ParticleCS.hlsl");
#pragma endregion

#pragma region SpriteShader
	{
		shared_ptr<Shader> spriteShader = std::make_shared<Shader>();
		Resources::Insert<Shader>(L"SpriteShader", spriteShader);
		ShaderInfo _info = {};
		ShaderEntry _entry;
		_info.bst = BSType::AlphaBlend;
		_info.dst = DSType::Less;
		_info.rst = RSType::SolidNone;
		_info.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_entry = {};
		_entry.VS = true;
		_entry.PS = true;
		spriteShader->CreateShader(_info, _entry, L"Sprite.hlsl");
	}
#pragma endregion
}
