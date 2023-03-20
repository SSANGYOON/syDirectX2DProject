#include "pch.h"
#include "Grid.h"
#include "Resources.h"
#include "Shader.h"
#include "Mesh.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "ConstantBuffer.h"
#include "Engine.h"

Grid::Grid() : Component(Component_Type::GRID)
{
}

Grid::~Grid()
{
}

void Grid::Start()
{
	_camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
	_cameraTrans = _camera->GetOwner()->GetTransform();
	_shader = GET_SINGLE(Resources)->Find<Shader>(L"GridShader");
	_mesh = GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh");
}

void Grid::Render()
{
	GridCB grCB = {};
	grCB.cameraPosition = Vector4(_cameraTrans->GetPosition(),1.f);
	grCB.cameraScale = Vector2(_camera->GetScale());
	grCB.resolution = Vector2(_camera->_width, _camera->_height);

	shared_ptr<ConstantBuffer> cb = GEngine->GetConstantBuffer(Constantbuffer_Type::GRID);
	cb->SetData(&grCB);
	cb->SetPipline(ShaderStage::VS);
	cb->SetPipline(ShaderStage::PS);
	_shader->BindShader();
	_mesh->Render();
}
