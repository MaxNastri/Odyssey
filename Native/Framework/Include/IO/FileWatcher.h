#pragma once
#include <functional>
#include <map>
#include <mutex>
#include "FileWatcherHandle.h"

namespace Odyssey::Framework
{
	class WatchInfo;

	class FileWatcher
	{
	public:
		FileWatcher();
		~FileWatcher();

	private:
		void Update();
		void Stop();
		void ProcessEvent(DWORD numberOfBytes, OVERLAPPED* overlapped);

	public:
		bool AddDirectory(int64_t id, const std::wstring& path);
		void RemoveDirectory(int64_t id);

	public:
		std::function<void(int64_t, const NotificationSet&)> ChangeEvent;
		std::function<void(int64_t)> ErrorEvent;

	private:
		HandlePtr ioCompletionPort;
		std::map<OVERLAPPED*, WatchInfo> watchInfos;

		std::mutex watchInfoMutex;
		std::atomic<bool> running;
		std::thread ioThread;
	};
}