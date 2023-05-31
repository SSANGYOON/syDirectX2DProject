#pragma once
#include "Texture.h"

enum class RENDER_TARGET_GROUP_TYPE : UINT8
{
	SWAP_CHAIN,
	HDR,
	DEFFERED,
	EDITOR, 
	LIGHTING,
	END,
};

enum
{
	SWAP_CHAIN_BUFFER_COUNT = 2
};

enum
{
	RENDER_TARGET_EDITER_GROUP_MEMBER_COUNT = 1,
	RENDER_TARGET_HDR_BUFFER_MEMBER_COUNT = 2,
	RENDER_TARGET_DEFFERED_GROUP_MEMBER_COUNT = 4,

	RENDER_TARGET_GROUP_COUNT = static_cast<UINT8>(RENDER_TARGET_GROUP_TYPE::END)
};

struct RenderTarget
{
	shared_ptr<Texture> target;
	float clearColor[4];
	RenderTarget()
		:clearColor{ 0.0f,0.0f,0.0f,1.f } {}
};

class RenderTargetGroup
{
public:
	void Create(RENDER_TARGET_GROUP_TYPE groupType, vector<RenderTarget>& rtVec, shared_ptr<Texture> dsTexture);

	void OMSetRenderTargets();
	void ClearRenderTargets(float* color = nullptr);
	void ClearRenderTargets(float* color, UINT ind);

	shared_ptr<Texture> GetRTTexture(UINT index) { return _rtVec[index].target; }
	shared_ptr<Texture> GetDSTexture() { return _dsTexture; }

private:
	RENDER_TARGET_GROUP_TYPE		_groupType;
	vector<RenderTarget>			_rtVec;
	UINT							_rtCount;
	shared_ptr<Texture>				_dsTexture;
};
