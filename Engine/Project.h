#pragma once

#include <string>
#include <filesystem>

namespace SY {

	struct ProjectConfig
	{
		std::string Name = "Untitled";

		std::filesystem::path StartScene;

		std::filesystem::path AssetDirectory;
		std::filesystem::path ScriptModulePath;
	};

	class Project
	{
	public:
		static const std::filesystem::path& GetProjectDirectory()
		{
			assert(s_ActiveProject);
			return s_ActiveProject->m_ProjectDirectory;
		}

		static std::filesystem::path GetAssetDirectory()
		{
			assert(s_ActiveProject);
			return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory;
		}

		static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path)
		{
			assert(s_ActiveProject);
			return GetAssetDirectory() / path;
		}

		ProjectConfig& GetConfig() { return m_Config; }

		static shared_ptr<Project> GetActive() { return s_ActiveProject; }
		static shared_ptr<Project> New();
		static shared_ptr<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);
	private:
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;

		inline static shared_ptr<Project> s_ActiveProject;
	};

}