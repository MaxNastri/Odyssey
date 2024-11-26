#pragma once
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

	struct TrackingOptions
	{
	public:
		Path TrackingPath;
		std::vector<Path> Extensions;
		bool Recursive = false;
		bool IncludeDirectoryChanges = false;
		std::function<void(const Path, const Path, FileActionType)> Callback = nullptr;
	};

	class FileTracker : efsw::FileWatchListener
	{
	public:
		FileTracker(TrackingOptions options);
		~FileTracker();

	protected:
		void handleFileAction(efsw::FileAction& fileAction) override;		

	private:
		friend class Editor;
		static void Init();
		static void Dispatch();

	private:
		inline static efsw::FileWatcher s_FileWatcher;

	private:
		enum class TrackingMode
		{
			None = 0,
			Directory = 1,
			File = 2,
		};
		TrackingOptions m_Options;
		TrackingMode m_TrackingMode;
		uint32_t m_ID = (uint32_t)(-1);
		inline static ReadWriteLock m_Lock;
		inline static std::vector<std::function<void()>> m_WaitingCallbacks;
	};
}