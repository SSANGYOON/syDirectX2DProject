#include "pch.h"
#include "Editor.h"
#include "Resources.h"
#include "Mesh.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "GridScript.h"
#include "SceneManager.h"
#include "Scene.h"

void Editor::Initalize()
{
	// 충돌체의 종류 갯수만큼만 있으면 된다.
	_debugObjects.resize((UINT)DebugMeshType::END);

	shared_ptr<Mesh> rectMesh = GET_SINGLE(Resources)->Find<Mesh>(L"DebugMesh");
	shared_ptr<Material> material = GET_SINGLE(Resources)->Find<Material>(L"DebugMaterial");

	_debugObjects[(UINT)DebugMeshType::RECT] = make_shared<DebugObject>();
	MeshRenderer* renderer
		= _debugObjects[(UINT)DebugMeshType::RECT]->AddComponent<MeshRenderer>().get();

	renderer->SetMaterial(material);
	renderer->SetMesh(rectMesh);

	std::shared_ptr<Mesh> circleMesh = GET_SINGLE(Resources)->Find<Mesh>(L"CircleMesh");

	_debugObjects[(UINT)DebugMeshType::CIRCLE] = make_shared<DebugObject>();
	renderer = _debugObjects[(UINT)DebugMeshType::CIRCLE]->AddComponent<MeshRenderer>().get();

	renderer->SetMaterial(material);
	renderer->SetMesh(circleMesh);

	//그리드 이쪽으로 옮겨줘야 한다.
	// Grid Object
	shared_ptr<EditorObject> gridObject = make_shared<EditorObject>();
	MeshRenderer* gridMr = gridObject->AddComponent<MeshRenderer>().get();
	gridMr->SetMesh(GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh"));
	gridMr->SetMaterial(GET_SINGLE(Resources)->Find<Material>(L"GridMaterial"));
	GridScript* gridScript = gridObject->AddComponent<GridScript>().get();
	gridScript->SetCamera(GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera());

	_editorObjects.push_back(gridObject);

}

void Editor::Run()
{
	Update();
	FinalUpdate();
	Render();
}

void Editor::Update()
{
	for (shared_ptr<EditorObject> obj : _editorObjects)
	{
		obj->Update();
	}
}

void Editor::FinalUpdate()
{
	for (shared_ptr<EditorObject> obj : _editorObjects)
	{
		obj->FinalUpdate();
	}
}

void Editor::Render()
{
	for (shared_ptr<EditorObject> obj : _editorObjects)
	{
		//obj->Render();
	}

	for (const DebugAttribute& debugAtt : GET_SINGLE(Resources)->GetDebugAtts())
	{
		DebugRender(debugAtt);
	}
	GET_SINGLE(Resources)->GetDebugAtts().clear();
}

void Editor::Release()
{

}

void Editor::DebugRender(const DebugAttribute& debugAtt)
{
	DebugObject* debugObj = _debugObjects[(UINT)debugAtt.dtype].get();

	Transform* tr = debugObj->GetTransform().get();
	tr->SetPosition(debugAtt.position);
	tr->SetRotation(debugAtt.rotation);
	tr->SetScale(debugAtt.scale);
	tr->FinalUpdate();

	auto cam = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
	cam->SetView();
	cam->SetProjection();

	debugObj->Render();
}