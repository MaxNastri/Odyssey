#include "Project.h"
#include "Globals.h"

namespace Odyssey
{
	Project::Project(const ProjectSettings& projectSettings)
		: m_ProjectSettings(projectSettings)
	{

	}

	void Project::CreateNewProject(const std::string& projectName, const Path& projectDirectory)
	{
		// Copy the template files into the project directory
		std::filesystem::copy(TEMPLATE_DIRECTORY, projectDirectory, std::filesystem::copy_options::recursive);

		auto premakePath = projectDirectory / "premake5.lua";
		auto projectSettingsPath = projectDirectory / "ProjectSettings.asset";

		// Premake
		{
			std::ifstream stream(premakePath);
			if (stream.is_open())
			{
				// Read the premake file into a buffer
				std::stringstream ss;
				ss << stream.rdbuf();
				stream.close();

				// Replace the template placeholder text with the project name
				std::string premakeString = ss.str();
				Globals::ReplaceString(premakeString, "%ProjectName%", projectName);

				std::ofstream ostream(projectDirectory / "premake5.lua");
				ostream << premakeString;
				ostream.close();
			}
		}

		// Project file
		{
			std::ifstream stream(projectSettingsPath);
			if (stream.is_open())
			{
				// Read the project file into a buffer
				std::stringstream ss;
				ss << stream.rdbuf();
				stream.close();

				// Replace the template placeholder text with the project name
				std::string str = ss.str();
				Globals::ReplaceString(str, "%ProjectName%", projectName);

				// Write it back
				std::ofstream ostream(projectSettingsPath);
				ostream << str;
				ostream.close();
			}
		}

		// Run the batch file to generate the VS project
		auto batchFile = projectDirectory / TEMPLATE_PROJ_GEN;
		system(batchFile.string().c_str());

		// Set the new project as our active
		SetActive(ProjectSettings(projectSettingsPath));
	}

	void Project::LoadProject(const Path& projectDirectory)
	{
		Path projectSettingsPath = projectDirectory / "ProjectSettings.asset";

		if (!std::filesystem::exists(projectSettingsPath))
		{
			Logger::LogError("[ProjectManager] Could not find project settings at " + projectSettingsPath.string());
			return;
		}

		SetActive(ProjectSettings(projectSettingsPath));
	}

	void Project::SetActive(const ProjectSettings& projectSettings)
	{
		s_ActiveProject = std::make_unique<Project>(projectSettings);
	}
	std::shared_ptr<Project> Project::GetActive()
	{
		return s_ActiveProject;
	}
}