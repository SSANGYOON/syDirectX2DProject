
#include "pch.h"
#include "Project.h"

#include "ProjectSerializer.h"
#include "PrefabManager.h"

namespace SY {

	shared_ptr<Project> Project::New()
	{
		s_ActiveProject = shared_ptr<Project>();
		return s_ActiveProject;
	}

	shared_ptr<Project> Project::Load(const std::filesystem::path& path)
	{
		shared_ptr<Project> project = make_shared<Project>();
		ProjectSerializer serializer(project);
		if (serializer.Deserialize(path))
		{
			project->m_ProjectDirectory = path.parent_path();
			s_ActiveProject = project;
			
			return s_ActiveProject;
		}
		

		return nullptr;
	}

	bool Project::SaveActive(const std::filesystem::path& path)
	{
		ProjectSerializer serializer(s_ActiveProject);
		if (serializer.Serialize(path))
		{
			s_ActiveProject->m_ProjectDirectory = path.parent_path();
			return true;
		}

		return false;
	}

}