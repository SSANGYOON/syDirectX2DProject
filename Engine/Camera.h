#pragma once
#include "Component.h"

class Camera : public Component
{
public:
	enum ProjectionType
	{
		Perspective,
		Orthographic,
	};
	Camera(class GameObject* owner);
	virtual ~Camera();

	virtual void FinalUpdate() override;

	void CreateView();
	void CreateProjection();

	void TurnLayerMask(UINT layerIndex, bool enable = true);
	void EnableLayerMasks() {_layerMasks.set(); }
	void DisableLayerMasks() { _layerMasks.reset(); }
	
	void SetFov(float fov) { _fov = fov; }
	void SetAspectRation(float AspectRatio) { _aspectRatio = AspectRatio; }
	void SetNear(float n) { _near = n; }
	void SetFar(float f) { _far = f; }
	void SetScale(float scale) { _scale = scale; }

	void SetProjectionType(ProjectionType type) { _type = type; }

	float GetScale() { return _scale; }

	void SetView() { s_view = _view; }
	void SetProjection() { s_projection = _projection; }

	void Render();

private:
	friend class Grid;
	Matrix _view;
	Matrix _projection;

	float _fov;
	float _aspectRatio;
	float _near;
	float _far;
	float _scale;

	float _width;
	float _height;
	ProjectionType _type;
	bitset<32> _layerMasks;
	vector<shared_ptr<GameObject>> _vecSorted;

public:
	static Matrix s_view;
	static Matrix s_projection;
};

