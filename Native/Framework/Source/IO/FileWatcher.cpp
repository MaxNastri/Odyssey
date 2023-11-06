#include "FileWatcher.h"
#include "Log.h"
#include "WatchInfo.h"

namespace Odyssey::Framework
{
	FileWatcher::FileWatcher()
	{
		running = true;
		ioCompletionPort.reset(CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 1));
		if (!ioCompletionPort)
		{
			Log::Error("Error while creating io completion port handle.");
			return;
		}

		ioThread = std::thread([this]() { Update(); });
	}

	FileWatcher::~FileWatcher()
	{
		Stop();
		ioThread.join();
	}

	void FileWatcher::Update()
	{
		DWORD numOfBytes = 0;
		OVERLAPPED* overlapped = nullptr;
		ULONG_PTR compKey = 0;
		BOOL result = FALSE;

		while ((result = GetQueuedCompletionStatus(this->ioCompletionPort.get(), &numOfBytes, &compKey, &overlapped, INFINITE)) != FALSE)
		{
			if (compKey != 0 && compKey == reinterpret_cast<ULONG_PTR>(this))
			{
				// stop "magic packet" was sent, so we shut down:
				break;
			}
			else
			{
				ProcessEvent(numOfBytes, overlapped);
			}
		}

		running.store(false);

		if (result != FALSE)
		{
			// IOCP is intact, so we clean up outstanding calls:
			std::lock_guard<std::mutex> lock(watchInfoMutex);

			const auto hasPending = [&lock, this]()
				{
					for (const auto& watchInfo : watchInfos)
					{
						if (watchInfo.second.state == WatchInfo::State::PendingClose)
						{
							return true;
						}
					}
					return false;
				};


			while (hasPending() &&
				(result = GetQueuedCompletionStatus(ioCompletionPort.get(), &numOfBytes, &compKey, &overlapped, INFINITE)) != FALSE)
			{
				const auto watchInfoIter = watchInfos.find(overlapped);
				if (watchInfoIter != watchInfos.end())
				{
					watchInfoIter->second.state = WatchInfo::State::PendingClose;
					watchInfos.erase(watchInfoIter);
				}
			}
		}
		else
		{
			const uint32_t errorCode = GetLastError();
			Log::Error("There is something wrong with the IOCP: " + errorCode);

			// alert all subscribers that they will not receive events from now on:
			std::lock_guard<std::mutex> lock(watchInfoMutex);
			for (auto& watchInfo : this->watchInfos)
			{
				ErrorEvent(watchInfo.second.ID);
			}
		}
	}

	void FileWatcher::Stop()
	{
		std::lock_guard<std::mutex> lock(watchInfoMutex);

		for (auto& watchInfo : watchInfos)
		{
			watchInfo.second.Stop();
		}

		if (ioCompletionPort.get() != INVALID_HANDLE_VALUE)
		{
			// send stop "magic packet"
			PostQueuedCompletionStatus(ioCompletionPort.get(), 0, reinterpret_cast<ULONG_PTR>(this), nullptr);
		}
	}

	void FileWatcher::ProcessEvent(DWORD numberOfBytes, OVERLAPPED* overlapped)
	{
		std::lock_guard<std::mutex> lock(watchInfoMutex);

		// initialization:
		const auto watchInfoIt = watchInfos.find(overlapped);
		if (watchInfoIt == watchInfos.end())
		{
			Log::Error("WatchInfo was not found for filesystem event.");
			return;
		}

		if (watchInfoIt->second.state == WatchInfo::State::Listening)
		{
			watchInfoIt->second.state = WatchInfo::State::Initialized;
		}

		if (numberOfBytes == 0)
		{
			if (watchInfoIt->second.state == WatchInfo::State::PendingClose)
			{
				// this is the "closing" notification, se we clean up:
				watchInfos.erase(watchInfoIt);
			}
			else
			{
				ErrorEvent(watchInfoIt->second.ID);
			}
			return;
		}

		WatchInfo& watchInfo = watchInfoIt->second;

		// If we're already in PendingClose state, and receive a legitimate notification, then
		// we don't emit a change notification, however, we delete the WatchInfo, just like when
		// we receive a "closing" notification.

		if (watchInfo.CanRun())
		{
			auto notificationResult = watchInfo.ProcessNotifications();

			if (!notificationResult.empty())
			{
				ChangeEvent(watchInfo.ID, std::move(notificationResult));
			}

			if (!watchInfo.Watch())
			{
				this->ErrorEvent(watchInfo.ID);
				this->watchInfos.erase(watchInfoIt);
			}
		}
		else
		{
			this->watchInfos.erase(watchInfoIt);
		}
	}

	bool FileWatcher::AddDirectory(int64_t id, const std::wstring& path)
	{
		if (!this->running)
		{
			Log::Error("Watcher thread is not running.");
			return false;
		}

		HandlePtr dirHandle(CreateFileW(path.c_str(), FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL));

		if (dirHandle.get() == INVALID_HANDLE_VALUE)
		{
			Log::Error("Cannot create directory handle: " + GetLastError());
			return false;
		}

		BY_HANDLE_FILE_INFORMATION fileInfo{};

		const BOOL result = GetFileInformationByHandle(dirHandle.get(), &fileInfo);
		if (result == FALSE)
		{
			return false;
		}
		else if (!(fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			Log::Error("Not a directory.");
			return false;
		}

		if (!CreateIoCompletionPort(dirHandle.get(), ioCompletionPort.get(), NULL, 1))
		{
			Log::Error("Cannot create IOCP: " + GetLastError());
			return false;
		}

		auto overlapped = std::make_unique<OVERLAPPED>();
		OVERLAPPED* const ovPtr = overlapped.get();

		std::lock_guard<std::mutex> lock(watchInfoMutex);
		const auto info = watchInfos.emplace(std::piecewise_construct, std::forward_as_tuple(ovPtr),
			std::forward_as_tuple(id, std::move(overlapped), path, std::move(dirHandle)));

		if (!info.second)
		{
			return false;
		}

		if (!running)
		{
			watchInfos.erase(info.first);
			return false;
		}
		else
		{
			if (!info.first->second.Watch())
			{
				watchInfos.erase(info.first);
				return false;
			}
		}

		return false;
	}

	void FileWatcher::RemoveDirectory(int64_t id)
	{
		std::lock_guard<std::mutex> lock(watchInfoMutex);

		for (auto& watchInfo : watchInfos)
		{
			if (watchInfo.second.ID == id)
			{
				watchInfo.second.Stop();
			}
		}
	}
}