#pragma once
#include <string>
#include <unordered_set>
#include <FileWatcher.h>

namespace Odyssey::Editor
{
	class FileManager
	{
	public:
		static void Initialize();
		static void TrackFolder(const std::string& path);

	private:
		static void FileChangedEvent(int64_t id, const Framework::NotificationSet& notificationSet);
		static void FileWatcherError(int64_t id);

		static std::string ConvertWideToUtf8(const std::wstring& wstr);
		static std::wstring ConvertUtf8ToWide(const std::string& str);
	private:
		static Framework::FileWatcher fileWatcher;
		static std::map<int64_t, std::wstring> folderIDs;
		static std::unordered_set<std::wstring> trackedFolders;
		static int64_t nextID;
	};
}