#pragma once
#include "Component.h"

class Camera;
class Shader;
class Mesh;
class Grid : public Component
{
public:
	Grid();
	virtual ~Grid();

	virtual void Start() override;
	virtual void Render() override;
private:
	Camera* _camera;
	Transform* _cameraTrans;
	shared_ptr<Shader> _shader;
	shared_ptr<Mesh> _mesh;
};