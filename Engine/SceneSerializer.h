#pragma once
#include "Entity.h"

namespace SY {
	class Scene;
	class SceneSerializer
	{
	public:
		SceneSerializer(const shared_ptr<Scene>& scene);

		void Serialize(const std::string& filepath);
		static void SerializeEntity(YAML::Emitter& out, Entity entity);
		void SerializeRuntime(const std::string& filepath);

		bool Deserialize(const std::string& filepath);

		static Entity DeserializeEntity(Scene* scene, YAML::Node& entity);
	private:
		shared_ptr<Scene> m_Scene;
	};

	
}