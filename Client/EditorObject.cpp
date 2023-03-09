#include "pch.h"
#include "EditorObject.h"

EditorObject::EditorObject()
	: GameObject()
{
}
EditorObject::~EditorObject()
{

}
void EditorObject::Start()
{
	GameObject::Start();
}
void EditorObject::Update()
{
	GameObject::Update();
}
void EditorObject::FinalUpdate()
{
	GameObject::FinalUpdate();
}
void EditorObject::Render()
{
	GameObject::Render();
}