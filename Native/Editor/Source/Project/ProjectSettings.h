#pragma once

namespace Odyssey
{
	class ProjectSettings
	{
	public:
		ProjectSettings() = default;
		ProjectSettings(const std::string& projectName, const std::filesystem::path& projectDirectory);
		ProjectSettings(const std::filesystem::path& settingsPath);

	public:
		void Save();
		void Load();

	public:
		const std::filesystem::path& GetPath() { return m_Path; }

	public:
		std::string ProjectName;

	private:
		std::filesystem::path m_Path;
	};
}