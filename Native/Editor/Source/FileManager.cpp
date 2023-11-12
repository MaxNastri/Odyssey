#include "FileManager.h"
#include <io.h>
#include <cstdio>
#include <fcntl.h>
#include <Logger.h>

namespace Odyssey::Editor
{
	Framework::FileWatcher FileManager::fileWatcher;
	std::map<int64_t, std::wstring> FileManager::folderIDs;
	std::unordered_set<std::wstring> FileManager::trackedFolders;
	int64_t FileManager::nextID;

	void FileManager::Initialize()
	{
		// use _setmode() to make sure that filenames containing Unicode characters are displayed correctly
		//int ret = _setmode(_fileno(stdout), _O_WTEXT);
		fileWatcher.ChangeEvent = FileChangedEvent;
		fileWatcher.ErrorEvent = FileWatcherError;

		nextID = 0;
	}

	void FileManager::TrackFolder(const std::string& path)
	{
		std::wstring wide = ConvertUtf8ToWide(path);

		if (!trackedFolders.contains(wide))
		{
			folderIDs[nextID++] = wide;
			trackedFolders.insert(wide);
			fileWatcher.AddDirectory(nextID, wide);
		}
	}

	void FileManager::FileChangedEvent(int64_t id, const Framework::NotificationSet& notificationSet)
	{
		for (auto& notification : notificationSet)
		{
			std::string path = ConvertWideToUtf8(notification.first.c_str());
			std::string action = Framework::Utils::FileNotifcationsToString(notification.second);

			Framework::Logger::LogInfo("[FileManager] File changed: " + path + ", action = " + action);
		}
	}

	void FileManager::FileWatcherError(int64_t id)
	{

	}

	std::string FileManager::ConvertWideToUtf8(const std::wstring& wstr)
	{
		std::string str;
		size_t size;

		str.resize(wstr.length());
		wcstombs_s(&size, &str[0], str.size() + 1, wstr.c_str(), wstr.size());
		return str;
	}

	std::wstring FileManager::ConvertUtf8ToWide(const std::string& str)
	{
		std::wstring wstr;
		size_t size;

		wstr.resize(str.length());
		mbstowcs_s(&size, &wstr[0], wstr.size() + 1, str.c_str(), str.size());
		return wstr;
	}
}