#pragma once

#include "Buffer.h"

namespace SY {

	class FileSystem
	{
	public:
		static Buffer ReadFileBinary(const std::filesystem::path& filepath);
	};

}