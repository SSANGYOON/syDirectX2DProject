#include "Component.h"



class Halo : public Component
{
public:
	Halo(class GameObject* obj);
	~Halo();

	void SetRadius(float radius);
	void SetCenterAlpha(float alpha);
	void SetColor(Vector4 color);

	virtual void Render() override;

private:

	shared_ptr<class Material> _haloMaterial;
	shared_ptr<class Mesh> _circleMesh;
};
