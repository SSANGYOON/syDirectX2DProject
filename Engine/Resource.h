#pragma once
enum class Resource_Type
{
	MATERIAL,
	MESH,
	SHADER,
	TEXTURE,
	Animation
};
class Resource
{
public:
	Resource(Resource_Type type);
	virtual ~Resource();

	virtual HRESULT Load(const std::wstring& path) = 0;

	void SetKey(const wstring& key) { _key = key; }
	void SetPath(const wstring& path) { _path = path; }

	const wstring& GetKey() { return _key;}
	const wstring& GetPath() { return _path; }

private:
	Resource_Type _type;
	wstring _key;
	wstring _path;
};

