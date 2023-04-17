#pragma once
#include "Component.h"

class UIChild;
class UI :public Component
{
public:
	UI(GameObject* obj);
	virtual ~UI();
	virtual void Render();

	void AddChild(shared_ptr<UIChild> panel);

private:
	vector<shared_ptr<UIChild>> _children;
};

