#pragma once
#include "Ref.h"
#include "efsw.hpp"
#include "ReadWriteLock.h"

namespace Odyssey
{
	enum class FileActionType
	{
		None = 0,
		Add = 1,
		Delete = 2,
		Modified = 3,
		Moved = 4,
	};

	typedef std::function<void(const Path, const Path, FileActionType)> FileActionCallback;
	typedef size_t TrackingID;

	struct FileTracker
	{
	public:
		TrackingID ID;
		Path FilePath;
		FileActionCallback Callback;
	};

	struct FolderTracker
	{
	public:
		struct Options
		{
			std::vector<Path> Extensions;
			FileActionCallback Callback;
			bool IncludeDirectoryChanges = false;
			bool Recursive = false;
		};

	public:
		TrackingID ID;
		Path DirectoryPath;
		Options TrackingOptions;
	};

	class DirectoryListener : public efsw::FileWatchListener
	{
	public:
		DirectoryListener(const Path& directory, bool recursive);

	public:
		void AddFileTracker(TrackingID id, const Path& filePath, FileActionCallback callback);
		void RemoveFileTracker(TrackingID id);
		void AddFolderTracker(TrackingID id, const Path& folderPath, const FolderTracker::Options& options);
		void RemoveFolderTracker(TrackingID id);

	public:
		void Dispatch();

	protected:
		virtual void handleFileAction(efsw::FileAction& fileAction) override;

	public:
		Path Directory;
		bool Recursive = false;
		std::vector<FileTracker> FileTrackers;
		std::vector<FolderTracker> FolderTrackers;
		std::vector<std::function<void()>> PendingCallbacks;
		ReadWriteLock Lock;
	};

	class FileManager
	{
	public:
		FileManager();

	public:
		static void Init();
		static void Destroy();
		static FileManager& Get();

	public:
		TrackingID TrackFile(const Path& path, FileActionCallback callback);
		bool UntrackFile(TrackingID id);
		TrackingID TrackFolder(const Path& folderPath, const FolderTracker::Options& options);
		bool UntrackFolder(TrackingID id);
		void Dispatch();

	private:
		inline static FileManager* s_Instance;

	private:
		std::unordered_map<Path, Ref<DirectoryListener>> m_DirectoryListeners;
		std::unordered_map<TrackingID, Path> m_TrackedIDs;
		efsw::FileWatcher m_Watcher;
		TrackingID m_NextID = 0;
	};

}