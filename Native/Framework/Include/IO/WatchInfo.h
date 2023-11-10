#pragma once
#include "FileWatcherHandle.h"

namespace Odyssey::Framework
{
	class WatchInfo
	{
	public:
		WatchInfo(const int64_t& ID, std::unique_ptr<OVERLAPPED>&& overlapped, const std::wstring& path, HandlePtr&& directoryHandle);
		WatchInfo(const WatchInfo&) = delete;
		WatchInfo& operator=(const WatchInfo&) = delete;
		~WatchInfo();

	public:
		bool Watch();
		void Stop();
		NotificationSet ProcessNotifications() const;
		bool CanRun() const { return state != State::PendingClose; }

	private:
		void ProcessNotification(const FILE_NOTIFY_INFORMATION& notification, NotificationSet& notifications) const;
		std::wstring TryGetLongName(const std::wstring& pathName) const;

	public:
		enum class State : uint8_t
		{
			Initialized,
			Listening,
			PendingClose,
		};
	public:
		int64_t ID;
		std::atomic<State> state;

	private:
		std::unique_ptr<OVERLAPPED> overlapped;
		std::aligned_storage_t<64 * 1024, sizeof(DWORD)> notificationBuffer;
		std::wstring path;
		HandlePtr directory;
	};
}