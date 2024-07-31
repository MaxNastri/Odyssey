#pragma once
#include "efsw.hpp"

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
		std::filesystem::path Direrctory;
		std::vector<std::filesystem::path> Extensions;
		bool Recursive = false;
		bool IncludeDirectoryChanges = false;
		std::function<void(const std::filesystem::path, FileActionType)> Callback = nullptr;
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

	private:
		inline static efsw::FileWatcher s_FileWatcher;

	private:
		TrackingOptions m_Options;
		uint32_t m_ID = (uint32_t)(-1);
	};
}