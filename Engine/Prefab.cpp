#include "pch.h"
#include "Prefab.h"
#include "SceneSerializer.h"
#include "Scene.h"
#include "UUID.h"


map<uint64_t, uint64_t> Prefab::uuidMap;
Prefab::Prefab()
    :Resource(RESOURCE_TYPE::PREFAB)
{
}

Prefab::~Prefab()
{
}

HRESULT Prefab::Load(const std::wstring& path, bool stockObject)
{
	try
	{
		data = YAML::LoadFile(wtos(path));
	}
	catch (YAML::ParserException e)
	{
		assert(false);
		return S_FALSE;
	}

	if (!data["Entities"])
		return S_FALSE;
	else
		return S_OK;
}

 SY::Entity Prefab::Instantiate(SY::Scene* scene)
{
	 uuidMap.clear();
	 for (const auto& entity : data["Entities"]) {
		 uint64_t uuid = entity["Entity"].as<uint64_t>();
		 uuidMap[uuid] = SY::GetRandomNumber();
	 }

	 for (auto&& entity : data["Entities"]) {
		 uint64_t uuid = entity["Entity"].as<uint64_t>();
		 entity.force_insert("Entity", uuidMap[uuid]);
		 if (entity["Parent"]) {
			 uint64_t parentId = entity["Parent"].as<uint64_t>();
			 entity.force_insert("Parent", uuidMap[parentId]);
		 }
		 
		 return SY::SceneSerializer::DeserializeEntity(scene, entity);
	 }

	 
}
