#include "FileManager.h"
#include "EventSystem.h"
#include "Events.h"
#include "Utils.h"

namespace Odyssey
{
	FileWatcher FileManager::fileWatcher;
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

	void FileManager::AddFilesChangedCallback(std::function<void(const NotificationSet& notificationSet)> callback)
	{
		s_OnFilesChanged.push_back(callback);
	}

	void FileManager::FileChangedEvent(int64_t id, const NotificationSet& notificationSet)
	{
		for (auto& notification : notificationSet)
		{
			std::string path = ConvertWideToUtf8(notification.first.c_str());
			std::string action = Utils::FileNotifcationsToString(notification.second);

			Logger::LogInfo("[FileManager] File changed: " + path + ", action = " + action);
		}

		for (const auto& callback : s_OnFilesChanged)
		{
			callback(notificationSet);
		}
	}

	void FileManager::FileWatcherError(int64_t id)
	{

	}
}