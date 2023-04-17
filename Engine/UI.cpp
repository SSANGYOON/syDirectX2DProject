#include "pch.h"
#include "UI.h"
#include "UIChild.h"
#include "GameObject.h"

UI::UI(GameObject* obj)
	:Component(Component_Type::UI, obj)
{
}

UI::~UI()
{
}

void UI::Render()
{
	_owner->GetTransform()->SetTransformBuffer();
	for (auto& child : _children)
	{
		child->Render();
	}
}

void UI::AddChild(shared_ptr<UIChild> child)
{
	_children.push_back(child);
}
