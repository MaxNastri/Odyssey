#pragma once
#include <atomic>
#include <thread>
#include <mutex>
#include <map>
#include <set>
#include <functional>
#include <windows.h>

namespace Odyssey::Framework
{
	enum class FileNotifcations
	{
		Added = 0x00000001,
		Removed = 0x00000002,
		Modified = 0x00000003,
		RenamedOld = 0x00000004,
		RenamedNew = 0x00000005,
	};

	class Utils
	{
	public:
		static std::string FileNotifcationsToString(FileNotifcations notifications)
		{
			switch (notifications)
			{
			case Odyssey::Framework::FileNotifcations::Added:
				return "File Added";
			case Odyssey::Framework::FileNotifcations::Removed:
				return "File Removed";
			case Odyssey::Framework::FileNotifcations::Modified:
				return "Modified";
			case Odyssey::Framework::FileNotifcations::RenamedOld:
				return "Renamed Old";
			case Odyssey::Framework::FileNotifcations::RenamedNew:
				return "Renamed New";
			}

			return "";
		}
	};

	struct HandleDeleter
	{
		void operator()(HANDLE handle)
		{
			if (handle != INVALID_HANDLE_VALUE && handle != NULL) {
				CloseHandle(handle);
			}
		}
	};

	using HandlePtr = std::unique_ptr<std::remove_pointer<HANDLE>::type, HandleDeleter>;
	using NotificationSet = std::set<std::pair<std::wstring, FileNotifcations>>;

}