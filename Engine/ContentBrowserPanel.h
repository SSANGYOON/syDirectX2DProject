#pragma once

class Texture;

#include <filesystem>

namespace SY {

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();
	private:
		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;

		shared_ptr<Texture> m_DirectoryIcon;
		shared_ptr<Texture> m_FileIcon;
	};

}