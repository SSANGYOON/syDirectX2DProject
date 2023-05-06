#include "pch.h"
#include "RenderTargetGroup.h"
#include "Texture.h"
#include "Engine.h"

void RenderTargetGroup::Create(RENDER_TARGET_GROUP_TYPE groupType, vector<RenderTarget>& rtVec, shared_ptr<Texture> dsTexture)
{
	_groupType = groupType;
	_rtVec = rtVec;
	_rtCount = static_cast<UINT>(rtVec.size());
	_dsTexture = dsTexture;
}
void RenderTargetGroup::OMSetRenderTargets()
{
	vector<ID3D11RenderTargetView*> _vec;

	for (auto renderTarget : _rtVec)
	{
		_vec.push_back(renderTarget.target->GetRTV());
	}

	CONTEXT->OMSetRenderTargets(_rtVec.size(), (ID3D11RenderTargetView**)_vec.data(), _dsTexture->GetDSV());
}

void RenderTargetGroup::ClearRenderTargets(float* color)
{
	for (auto renderTarget : _rtVec)
	{
		if(color == nullptr)
			CONTEXT->ClearRenderTargetView(renderTarget.target->GetRTV(), renderTarget.clearColor);
		else
			CONTEXT->ClearRenderTargetView(renderTarget.target->GetRTV(), color);
	}

	CONTEXT->ClearDepthStencilView(_dsTexture->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}
