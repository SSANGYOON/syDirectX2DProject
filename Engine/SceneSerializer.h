#pragma once

namespace SY {
	class Scene;
	class SceneSerializer
	{
	public:
		SceneSerializer(const shared_ptr<Scene>& scene);

		void Serialize(const std::string& filepath);
		void SerializeRuntime(const std::string& filepath);

		bool Deserialize(const std::string& filepath);
		bool DeserializeRuntime(const std::string& filepath);
	private:
		shared_ptr<Scene> m_Scene;
	};

}