#pragma once
class Material;

enum class UI_TYPE
{
	IMAGE,
	TEXTURE,
};

class UIChild
{
public:
	UIChild(UI_TYPE type);
	virtual ~UIChild();
	
	void SetMaterial(shared_ptr<Material> material) { _material = material; };
	virtual void Render() abstract;
protected:
	shared_ptr<Material> _material;
	Vector2 _offset;

private:
	UI_TYPE _type;
};

