#pragma once
#include "Scene.h"

namespace Odyssey::Scripting
{
	struct OnAssembliesReloaded;
}

namespace Odyssey::Editor
{
	struct OnBuildFinished;

	class SceneManager
	{
	public:
		static void ListenForEvents();

		static void LoadScene(const std::string& filename);
		static void SaveActiveScene(const std::string& filename);

		static Scene& GetActiveScene();

	public:
		static void Update();

	private:
		static void BuildFinished(OnBuildFinished* onBuildFinished);
		static void AssembliesReloaded(Scripting::OnAssembliesReloaded* reloadedEvent);

	private:
		static std::vector<Scene> scenes;
		static int activeScene;
		inline static const std::string tempSaveFilename = "tmpSave.json";

	};
}