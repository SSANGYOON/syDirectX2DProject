#include "pch.h"
#include "RectScript.h"
#include "Input.h"
#include "Timer.h"

#include "Transform.h"
#include "GameObject.h"
void RectScript::Update()
{
	shared_ptr<Transform> tr = GetOwner()->GetComponent <Transform>();
	if (INPUT->GetKeyState(KEY_TYPE::RIGHT) == KEY_STATE::PRESS)
	{
		Vector3 pos = tr->GetPosition();
		pos.x += 200.f * TIME->DeltaTime();
		tr->SetPosition(pos);
	}
	if (INPUT->GetKeyState(KEY_TYPE::LEFT) == KEY_STATE::PRESS)
	{
		Vector3 pos = tr->GetPosition();
		pos.x -= 200.f * TIME->DeltaTime();
		tr->SetPosition(pos);
	}
	if (INPUT->GetKeyState(KEY_TYPE::UP) == KEY_STATE::PRESS)
	{
		Vector3 pos = tr->GetPosition();
		pos.y += 200.f * TIME->DeltaTime();
		tr->SetPosition(pos);
	}
	if (INPUT->GetKeyState(KEY_TYPE::DOWN) == KEY_STATE::PRESS)
	{
		Vector3 pos = tr->GetPosition();
		pos.y -= 200.f * TIME->DeltaTime();
		tr->SetPosition(pos);
	}
	
}
