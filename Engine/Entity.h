#pragma once
class Entity
{
public:
	Entity();
	virtual ~Entity();

	void SetName(const std::wstring& name) { _name = name; }
	const std::wstring& GetName() { return _name; }
	UINT32 GetID() { return _ID; }

private:
	std::wstring _name;
	UINT32 _ID;
};

