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
		std::filesystem::copy(TEMPLATE_DIRECTORY, projectDirectory);
		auto premakePath = projectDirectory / "premake5.lua";
		auto projectSettingsPath = projectDirectory / "ProjectSettings.asset";
		
		// Replace the placeholder project name in the premake file
		ReplaceProjectName(projectName, premakePath);

		// Replace the placeholder project name in the project settings file
		ReplaceProjectName(projectName, projectSettingsPath);

		// Load the project settings
		ProjectSettings settings = ProjectSettings(projectSettingsPath);

		// Copy the sample scripts into the project's scripts folder
		auto templateScriptsPath = Path(TEMPLATE_DIRECTORY) / "Scripts";
		std::filesystem::copy(templateScriptsPath, settings.GetCodeDirectory());

		// Replace the placeholder project name in the sample script
		auto sampleScript = settings.GetCodeDirectory() / TEMPLATE_SCRIPT_FILE;
		ReplaceProjectName(projectName, sampleScript);

		// Run the batch file to generate the VS project
		auto batchFile = projectDirectory / TEMPLATE_PROJ_GEN;
		system(batchFile.string().c_str());

		// Set the new project as our active
		SetActive(settings);
	}

	void Project::LoadProject(const Path& projectDirectory)
	{
		Path projectSettingsPath = projectDirectory / "ProjectSettings.osettings";

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

	void Project::ReplaceProjectName(std::string_view projectName, const Path& path)
	{
		std::ifstream stream(path);
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
			std::ofstream ostream(path);
			ostream << str;
			ostream.close();
		}
	}
}