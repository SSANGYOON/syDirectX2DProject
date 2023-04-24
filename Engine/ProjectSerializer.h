#pragma once

#include "Project.h"

namespace SY {

	class ProjectSerializer
	{
	public:
		ProjectSerializer(shared_ptr<Project> project);

		bool Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);
	private:
		shared_ptr<Project> m_Project;
	};

}