#pragma once

namespace Odyssey
{
	class FileDialogs
	{
	public:
		static Path OpenFile(const char* filter);
		static Path SaveFile(const char* filter);
	};
}