#pragma once
#include "AssetHandle.h"

namespace Odyssey
{
	struct OnBuildFinished;
	struct OnAssembliesReloaded;
	class Scene;

	class SceneManager
	{
	public:
		static void ListenForEvents();

		static void LoadScene(const std::string& assetPath);
		static void SaveActiveScene();
		static void SaveActiveSceneTo(const std::string& assetPath);

		static Scene* GetActiveScene();

	public:
		static void Awake();
		static void Update();

	private:
		static void BuildFinished(OnBuildFinished* onBuildFinished);
		static void AssembliesReloaded(OnAssembliesReloaded* reloadedEvent);

	private:
		inline static std::vector<AssetHandle<Scene>> scenes;
		inline static int activeScene = -1;
		inline static const std::string tempSaveFilename = "Assets/Scenes/tmps.yaml";

	};
}