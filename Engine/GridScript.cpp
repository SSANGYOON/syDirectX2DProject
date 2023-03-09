#include "pch.h"
#include "GridScript.h"
#include "ConstantBuffer.h"
#include "GameObject.h"
#include "Engine.h"

GridScript::GridScript()
{
}

GridScript::~GridScript()
{
}

void GridScript::FinalUpdate()
{
	//	CBUFFER(GridCB, CBSLOT_GRID)
		//{
		//	Vector4 cameraPosition;
		//	Vector2 cameraScale;
		//	Vector2 resolution;
		//};

	if (_camera == nullptr)
		return;

	GameObject* gameObj = _camera->GetOwner().get();
	Transform* tr = gameObj->GetTransform().get();

	Vector3 cameraPos = tr->GetPosition();
	Vector4 position = Vector4(cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);

	float scale = _camera->GetScale();

	WindowInfo info = GEngine->GetWindow();
	float width = info.width;
	float height = info.height;
	Vector2 resolution(width, height);

	// Constant buffer
	GridCB grCB = {};
	grCB.cameraPosition = position;
	grCB.cameraScale = Vector2(scale, scale);
	grCB.resolution = resolution;

	shared_ptr<ConstantBuffer> cb = GEngine->GetConstantBuffer(Constantbuffer_Type::GRID);
	cb->SetData(&grCB);
	cb->SetPipline(ShaderStage::VS);
}
