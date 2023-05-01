#pragma once
#include "Resource.h"
#include "Entity.h"

class SY::Scene;
class Prefab : public Resource
{
public:
	Prefab();
	virtual ~Prefab();

	virtual HRESULT Load(const std::wstring& path, bool stockObject = false) override;
	SY::Entity Instantiate(SY::Scene* scene);

private:
	YAML::Node data;
	static map<uint64_t, uint64_t> uuidMap;
};

