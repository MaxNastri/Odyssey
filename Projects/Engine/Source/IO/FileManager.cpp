#include "FileManager.h"

namespace Odyssey
{
	DirectoryListener::DirectoryListener(const Path& directory, bool recursive)
	{
		Directory = directory;
		Recursive = recursive;
	}

	void DirectoryListener::AddFileTracker(TrackingID id, const Path& filePath, FileActionCallback callback)
	{
		Lock.Lock(LockState::Write);
		FileTrackers.emplace_back(id, filePath, callback);
		Lock.Unlock(LockState::Write);
	}

	void DirectoryListener::RemoveFileTracker(TrackingID id)
	{
		Lock.Lock(LockState::Write);

		for (size_t i = 0; i < FileTrackers.size(); i++)
		{
			if (FileTrackers[i].ID == id)
			{
				FileTrackers.erase(FileTrackers.begin() + i);
				break;
			}
		}

		Lock.Unlock(LockState::Write);
	}

	void DirectoryListener::AddFolderTracker(TrackingID id, const Path& folderPath, const FolderTracker::Options& options)
	{
		Lock.Lock(LockState::Write);
		FolderTrackers.emplace_back(id, folderPath, options);
		Lock.Unlock(LockState::Write);
	}

	void DirectoryListener::RemoveFolderTracker(TrackingID id)
	{
		Lock.Lock(LockState::Write);

		for (size_t i = 0; i < FolderTrackers.size(); i++)
		{
			if (FolderTrackers[i].ID == id)
			{
				FolderTrackers.erase(FolderTrackers.begin() + i);
				break;
			}
		}

		Lock.Unlock(LockState::Write);
	}

	void DirectoryListener::Dispatch()
	{
		Lock.Lock(LockState::Read);
		for (auto& callback : PendingCallbacks)
			callback();
		PendingCallbacks.clear();
		Lock.Unlock(LockState::Read);
	}

	void DirectoryListener::handleFileAction(efsw::FileAction& fileAction)
	{
		const Path& fullPath = fileAction.Directory / fileAction.Filename;

		for (FileTracker& fileTracker : FileTrackers)
		{
			if (fileTracker.FilePath == fullPath)
			{
				if (fileAction.Action == efsw::Action::Add && std::filesystem::file_size(fileAction.Directory / fileAction.Filename) == 0)
					continue;

				if (fileTracker.Callback)
				{
					// We queue up the callbacks since handleFileAction operates on a separate thread
					// These callbacks are collected and executed on the main thread
					std::function<void()> pendingCallback = [this, fileAction, fileTracker]()
						{
							fileTracker.Callback(fileAction.OldFilename, fileAction.Filename, (FileActionType)fileAction.Action);
						};

					Lock.Lock(LockState::Write);
					PendingCallbacks.push_back(pendingCallback);
					Lock.Unlock(LockState::Write);
				}
			}
		}

		for (FolderTracker& folderTracker : FolderTrackers)
		{
			FolderTracker::Options& options = folderTracker.TrackingOptions;

			bool extensionMatch = options.Extensions.size() == 0;
			bool isDirectory = std::filesystem::is_directory(fileAction.Filename);

			// Skip directory changes
			if (isDirectory && !options.IncludeDirectoryChanges)
				continue;

			// Check against the list of tracked extensions
			if (!isDirectory && !extensionMatch)
			{
				const Path& extension = fileAction.Filename.extension();
				for (const Path& trackedExtension : options.Extensions)
				{
					if (extension == trackedExtension)
					{
						extensionMatch = true;
						break;
					}
				}
			}

			if (options.Callback && (isDirectory || extensionMatch))
			{
				// We queue up the callbacks since handleFileAction operates on a separate thread
				// These callbacks are collected and executed on the main thread
				std::function<void()> pendingCallback = [this, fileAction, folderTracker]()
					{
						folderTracker.TrackingOptions.Callback(fileAction.OldFilename, fileAction.Filename, (FileActionType)fileAction.Action);
					};

				Lock.Lock(LockState::Write);
				PendingCallbacks.push_back(pendingCallback);
				Lock.Unlock(LockState::Write);
			}
		}
	}

	FileManager::FileManager()
	{
		m_Watcher.watch();
	}

	void FileManager::Init()
	{
		s_Instance = new FileManager();
	}

	void FileManager::Destroy()
	{
		delete s_Instance;
		s_Instance = nullptr;
	}

	FileManager& FileManager::Get()
	{
		return *s_Instance;
	}

	size_t FileManager::TrackFile(const Path& path, FileActionCallback callback)
	{
		Path directory = path;

		if (path.has_filename())
			directory = path.parent_path();

		if (!m_DirectoryListeners.contains(directory))
		{
			m_DirectoryListeners[directory] = new DirectoryListener(directory, false);
			m_Watcher.addWatch(directory.string(), m_DirectoryListeners[directory].Get(), false);
		}

		TrackingID id = m_NextID++;
		m_DirectoryListeners[directory]->AddFileTracker(id, path, callback);
		m_TrackedIDs[id] = path;

		return id;
	}

	bool FileManager::UntrackFile(TrackingID id)
	{
		if (m_TrackedIDs.contains(id))
		{
			Path directory = m_TrackedIDs[id].parent_path();
			m_TrackedIDs.erase(id);

			if (m_DirectoryListeners.contains(directory))
			{
				m_DirectoryListeners[directory]->RemoveFileTracker(id);
				return true;
			}
		}

		return false;
	}

	TrackingID FileManager::TrackFolder(const Path& folderPath, const FolderTracker::Options& options)
	{
		if (!m_DirectoryListeners.contains(folderPath))
		{
			m_DirectoryListeners[folderPath] = new DirectoryListener(folderPath, false);
			m_Watcher.addWatch(folderPath.string(), m_DirectoryListeners[folderPath].Get(), false);
		}

		TrackingID id = m_NextID++;
		m_DirectoryListeners[folderPath]->AddFolderTracker(id, folderPath, options);
		m_TrackedIDs[id] = folderPath;

		return id;
	}

	bool FileManager::UntrackFolder(TrackingID id)
	{
		if (m_TrackedIDs.contains(id))
		{
			Path directory = m_TrackedIDs[id];
			m_TrackedIDs.erase(id);

			if (m_DirectoryListeners.contains(directory))
			{
				m_DirectoryListeners[directory]->RemoveFolderTracker(id);
				return true;
			}
		}

		return false;
	}

	void FileManager::Dispatch()
	{
		for (auto& [folder, directoryListener] : m_DirectoryListeners)
		{
			directoryListener->Dispatch();
		}
	}
}