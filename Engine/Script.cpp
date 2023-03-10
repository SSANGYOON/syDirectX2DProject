#include "pch.h"
#include "Script.h"
#include "Collider.h"

Script::Script()
	:Component(Component_Type::Script)
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

void Script::OntriggerEnter(Collider* collider)
{
}

void Script::OntriggerStay(Collider* collider)
{
}

void Script::OntriggerExit(Collider* collider)
{
}

void Script::OnCollisionEnter(Collider* collider)
{
}

void Script::OnCollisionStay(Collider* collider)
{
}

void Script::OnCollisionExit(Collider* collider)
{
}
