#pragma once
#include "Widget.h"
#include "EditorObject.h"
#include "DebugObject.h"


class Editor
{
DECLARE_SINGLE(Editor);

public:
	void Initalize();
	void Run();

	void Update();
	void FinalUpdate();
	void Render();
	void Release();
	void DebugRender(const DebugAttribute& debugAtt);

private:
	std::vector<shared_ptr<Widget>> _widgets;
	std::vector<shared_ptr<EditorObject>> _editorObjects;
	std::vector<shared_ptr<DebugObject>> _debugObjects;
};
