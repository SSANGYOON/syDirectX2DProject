#pragma once
enum class Resource_Type
{
	MATERIAL,
	MESH,
	SHADER,
	TEXTURE
};
class Resource
{
public:
	Resource(Resource_Type type);
	virtual ~Resource();

	virtual HRESULT Load(const std::wstring& path) = 0;

	const wstring& GetKey() { return _key;}
	const wstring& GetPath() { return _path; }

private:
	Resource_Type _type;
	wstring _key;
	wstring _path;
};

