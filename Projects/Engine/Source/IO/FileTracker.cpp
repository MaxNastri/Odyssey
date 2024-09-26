#include "FileTracker.h"

namespace Odyssey
{
	FileTracker::FileTracker(TrackingOptions options)
	{
		m_Options = options;
		m_ID = s_FileWatcher.addWatch(m_Options.Directory.string(), this, options.Recursive);
	}

	FileTracker::~FileTracker()
	{
		s_FileWatcher.removeWatch(m_ID);
	}

	void FileTracker::Init()
	{
		s_FileWatcher.watch();
	}

	void FileTracker::handleFileAction(efsw::FileAction& fileAction)
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
			if (!isDirectory && fileAction.Action == efsw::Action::Add && std::filesystem::file_size(fileAction.Directory / fileAction.Filename) == 0)
				return;

			m_Options.Callback(fileAction.Filename, (FileActionType)fileAction.Action);
		}
	}
}