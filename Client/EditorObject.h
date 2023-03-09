#pragma once
#include "GameObject.h"

class EditorObject : public GameObject
{
public:
	EditorObject();
	virtual ~EditorObject();

	virtual void Start() override;
	virtual void Update() override;
	virtual void FinalUpdate() override;
	virtual void Render() override;

private:

};

