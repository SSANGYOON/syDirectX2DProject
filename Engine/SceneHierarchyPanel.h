#pragma once

#include "Scene.h"
#include "Entity.h"

namespace SY {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const shared_ptr<Scene>& scene);

		void SetContext(const shared_ptr<Scene>& scene);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity);
	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		shared_ptr<Scene> m_Context;
		Entity m_SelectionContext;
	};
}