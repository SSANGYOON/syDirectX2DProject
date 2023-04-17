#pragma once

enum class RESOURCE_TYPE
{
	MESH,
	TEXTURE,
	GRAPHIC_SHADER,
	COMPUTE_SHADER,
	MATERIAL,
};

class Resource
{
public:
	Resource(RESOURCE_TYPE type);
	virtual ~Resource();

	virtual HRESULT Load(const std::wstring& path) = 0;

	void SetKey(const wstring& key) { _key = key; }
	void SetPath(const wstring& path) { _path = path; }

	const wstring& GetKey() { return _key;}
	const wstring& GetPath() { return _path; }

private:
	RESOURCE_TYPE _type;

	wstring _key;
	wstring _path;
};

