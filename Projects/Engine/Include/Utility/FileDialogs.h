#pragma once

namespace Odyssey
{
	class FileDialogs
	{
	public:
		static std::string OpenFile(void* mainWindow, const char* filter);
		static std::string SaveFile(void* mainWindow, const char* filter);
	};
}