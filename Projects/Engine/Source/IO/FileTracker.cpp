#include "FileTracker.h"

namespace Odyssey
{
	FileTracker::FileTracker(TrackingOptions options)
	{
		m_Options = options;
		m_TrackingMode = std::filesystem::is_directory(m_Options.TrackingPath) ? TrackingMode::Directory : TrackingMode::File;
		std::string directory = m_TrackingMode == TrackingMode::Directory ? m_Options.TrackingPath.string() : m_Options.TrackingPath.parent_path().string();
		m_ID = s_FileWatcher.addWatch(directory, this, options.Recursive);
	}

	FileTracker::~FileTracker()
	{
		s_FileWatcher.removeWatch(m_ID);
	}

	void FileTracker::Init()
	{
		s_FileWatcher.watch();
	}

	void FileTracker::Dispatch()
	{
		for (auto& callback : m_WaitingCallbacks)
			callback();

		m_WaitingCallbacks.clear();
	}

	void FileTracker::handleFileAction(efsw::FileAction& fileAction)
	{
		if (m_TrackingMode == TrackingMode::File)
		{
			const Path& fullPath = fileAction.Directory / fileAction.Filename;
			if (m_Options.TrackingPath == fullPath)
			{
				if (fileAction.Action == efsw::Action::Add && std::filesystem::file_size(fileAction.Directory / fileAction.Filename) == 0)
					return;

				if (m_Options.Callback)
					m_WaitingCallbacks.push_back([this, fileAction]() { m_Options.Callback(fileAction.Filename, (FileActionType)fileAction.Action); });
			}
		}
		else if (m_TrackingMode == TrackingMode::Directory)
		{
			// Check if the file extension is contained in the filter list
			// Note: Initial value is set to true when no extension filter is set
			bool extensionMatch = m_Options.Extensions.size() == 0;
			bool isDirectory = std::filesystem::is_directory(fileAction.Filename);

			// Early out for directories if we don't set the specific flag
			if (isDirectory && !m_Options.IncludeDirectoryChanges)
				return;

			if (!isDirectory)
			{
				auto fileExt = fileAction.Filename.extension();
				for (size_t i = 0; i < m_Options.Extensions.size(); i++)
				{
					if (fileExt == m_Options.Extensions[i])
					{
						extensionMatch = true;
						break;
					}
				}
			}

			if (m_Options.Callback && (isDirectory || extensionMatch))
			{
				Path fullpath = fileAction.Directory / fileAction.Filename;
				if (!isDirectory && fileAction.Action == efsw::Action::Add && std::filesystem::file_size(fullpath) == 0)
					return;

				if (m_Options.Callback)
					m_WaitingCallbacks.push_back([this, fileAction]() { m_Options.Callback(fileAction.Filename, (FileActionType)fileAction.Action); });
			}
		}
	}
}