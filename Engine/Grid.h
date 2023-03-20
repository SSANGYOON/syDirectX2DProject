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
	shared_ptr<Camera> _camera;
	shared_ptr<Transform> _cameraTrans;
	shared_ptr<Shader> _shader;
	shared_ptr<Mesh> _mesh;
};