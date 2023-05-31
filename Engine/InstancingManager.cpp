#include "pch.h"
#include "InstancingManager.h"
#include "Scene.h"
#include "Entity.h"
#include "Component.h"
#include "Renderer.h"

namespace SY {
	void InstancingManager::Render(vector<Entity>& entities)
	{
		map<UINT64, vector<Entity>> cache;

		for (Entity& entity : entities)
		{
			UINT64 id = entity.GetComponent<SpriteRendererComponent>().material->GetInstanceID();
			cache[id].push_back(entity);
		}

		for (auto& pair : cache)
		{
			vector<Entity>& vec = pair.second;

			const UINT64 instanceId = pair.first;

			for (Entity& entity : vec)
			{
				InstancingParams params;

				auto& tr = entity.GetComponent<TransformComponent>();
				auto& sr = entity.GetComponent<SpriteRendererComponent>();

				params.matW = tr.localToWorld;
				params.matWVP = tr.localToWorld * Renderer::trCB.view * Renderer::trCB.projection;

				if (sr.Diffuse) {
					params.tile = sr.tile / sr.Diffuse->GetSize();
					params.offset = sr.Offset / sr.Diffuse->GetSize();
				}

				params.Color = sr.Color;
				params.Emission = sr.Emission;

				AddParam(instanceId, params);
			}

			shared_ptr<InstancingBuffer>& buffer = _buffers[instanceId];
			Renderer::DrawBatchSprite(buffer, vec[0].GetComponent<SpriteRendererComponent>().material);
		}
	}

	void InstancingManager::ClearBuffer()
	{
		_buffers.clear();
	}

	void InstancingManager::AddParam(UINT64 instanceId, InstancingParams& data)
	{
		if (_buffers.find(instanceId) == _buffers.end())
			_buffers[instanceId] = make_shared<InstancingBuffer>();
		_buffers[instanceId]->AddData(data);
	}
}
