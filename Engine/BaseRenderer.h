#pragma once
#include "Component.h"
class Mesh;
class Material;
class BaseRenderer : public Component
{
public:
	BaseRenderer(class GameObject* owner);
	virtual ~BaseRenderer();


	virtual void Render() override;

	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }
	void SetMaterial(shared_ptr <Material> shader) { _material = shader; }
	void SetRender(bool render) { _render = render; }
	
	std::shared_ptr<Mesh> GetMesh() { return _mesh; }
	std::shared_ptr <Material> GetMaterial() { return _material; }

protected:
	std::shared_ptr <Mesh> _mesh;
	std::shared_ptr <Material> _material;
	bool _render;
};

