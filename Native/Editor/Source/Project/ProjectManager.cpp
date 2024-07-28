#include "ProjectManager.h"
#include "Logger.h"
#include <fstream>
#include "Globals.h"

namespace Odyssey
{
	void ProjectManager::CreateNewProject(const std::string& projectName, const std::filesystem::path& projectDirectory)
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

		// Load the project settings
		m_Settings = ProjectSettings(projectSettingsPath);

		// Run the batch file to generate the VS project
		auto batchFile = projectDirectory / TEMPLATE_PROJ_GEN;
		system(batchFile.string().c_str());
	}
}