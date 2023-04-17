#include "pch.h"
#include "UIImage.h"
#include "Material.h"
#include "Resources.h"
#include "Mesh.h"

UIImage::UIImage()
	:UIChild(UI_TYPE::IMAGE)
{
}

UIImage::~UIImage()
{
}

void UIImage::Render()
{
	_material->Bind();
	auto rect = GET_SINGLE(Resources)->Find<Mesh>(L"RectMesh");
	rect->BindBuffer();
	rect->Render();
}
