#pragma once
#include "InstancingBuffer.h"

namespace SY {
	class Entity;
	class Scene;
	class InstancingManager
	{
		DECLARE_SINGLE(InstancingManager)

	public:
		void Render(vector<Entity>& entities);
		void ClearBuffer();

	private:
		void AddParam(UINT64 instanceId, InstancingParams& data);

	private:
		map<UINT64, shared_ptr<InstancingBuffer>> _buffers;
	};
}
