#include "pch.h"
#include "Script.h"
#include "Collider.h"
#include "GameObject.h"

Script::Script(GameObject* owner)
	:Component(Component_Type::Script, owner)
{

}

Script::~Script()
{
}

void Script::Start()
{
}

void Script::Update()
{
}

void Script::LateUpdate()
{
}

void Script::FinalUpdate()
{
}
