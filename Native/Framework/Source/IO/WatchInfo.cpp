#include "WatchInfo.h"
#include "Log.h"

namespace Odyssey::Framework
{
	WatchInfo::WatchInfo(const int64_t& ID, std::unique_ptr<OVERLAPPED>&& overlapped, const std::wstring& path, HandlePtr&& directoryHandle)
		: overlapped(std::move(overlapped)),
		notificationBuffer(),
		path(path),
		directory(std::move(directoryHandle)),
		ID(ID),
		state(State::Initialized)
	{

	}

	WatchInfo::~WatchInfo()
	{
		if (state == State::Listening)
		{
			Log::Info("Destroying watcher.");
		}
	}

	bool WatchInfo::Watch()
	{
		if (state != State::Initialized)
		{
			Log::Error("Invalid file watcher state.");
			return false;
		}

		constexpr DWORD flags = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME
			| FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION
			| FILE_NOTIFY_CHANGE_SECURITY;

		const BOOL result = ReadDirectoryChangesW(this->directory.get(), &notificationBuffer,
			static_cast<DWORD>(sizeof(notificationBuffer)), true, flags,
			nullptr, this->overlapped.get(), nullptr);

		if (result == FALSE)
		{
			state = State::Initialized;
			Log::Error("An error occured: " + GetLastError());
			return false;
		}

		state = State::Listening;
		return true;
	}

	void WatchInfo::Stop()
	{
		if (state == State::Listening)
		{
			state = State::PendingClose;
		}
		directory.reset(INVALID_HANDLE_VALUE);
	}

	NotificationSet WatchInfo::ProcessNotifications() const
	{
		NotificationSet notifications;

		auto notificationInfo = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(&this->notificationBuffer);
		
		bool moreNotifications = notificationInfo->NextEntryOffset > 0;
		while (moreNotifications)
		{
			// Get the next notification
			auto nextNotification = reinterpret_cast<const char*>(notificationInfo) + notificationInfo->NextEntryOffset;
			notificationInfo = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(nextNotification);

			// Process the notification
			ProcessNotification(*notificationInfo, notifications);

			// Check if there are more notifications
			moreNotifications = notificationInfo->NextEntryOffset > 0;
		}
		
		return std::move(notifications);
	}

	void WatchInfo::ProcessNotification(const FILE_NOTIFY_INFORMATION& notification, NotificationSet& notifications) const
	{
		std::wstring pathName(notification.FileName, notification.FileName + (notification.FileNameLength / sizeof(notification.FileName)));

		if (notification.Action != FILE_ACTION_REMOVED && notification.Action != FILE_ACTION_RENAMED_OLD_NAME)
		{
			std::wstring longName = TryGetLongName(pathName);
			if (longName != pathName)
			{
				pathName = longName;
			}
		}
		notifications.emplace(pathName, (FileNotifcations)notification.Action);
	}

	std::wstring WatchInfo::TryGetLongName(const std::wstring& pathName) const
	{
		const std::wstring fullPath = path + L"\\" + pathName;
		const DWORD longSize = GetLongPathNameW(fullPath.c_str(), NULL, 0);

		std::wstring longPathName;
		longPathName.resize(longSize);

		const DWORD retVal = GetLongPathNameW(fullPath.c_str(), longPathName.data(), static_cast<DWORD>(longPathName.size()));
		
		if (retVal == 0)
		{
			return pathName;
		}

		while (!longPathName.empty() && longPathName.back() == L'\0')
		{
			longPathName.pop_back();
		}

		if (longPathName.find(this->path) == 0 && this->path.size() < longPathName.size())
		{
			std::wstring longName = longPathName.substr(this->path.size() + 1);

			if (longName.empty())
			{
				return pathName;
			}
			else
			{
				return longName;
			}
		}
		else
		{
			return pathName;
		}
	}
}