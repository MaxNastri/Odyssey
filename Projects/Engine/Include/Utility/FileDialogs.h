#pragma once

namespace Odyssey
{
	class FileDialogs
	{
	public:
		static Path OpenFile(std::string_view fileType, const Path& extension);
		static Path SaveFile(std::string_view fileType, const Path& extension);
	};
}