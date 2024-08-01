#pragma once
#include "AssetHandle.h"

namespace Odyssey
{
	struct BuildCompleteEvent;
	struct OnAssembliesReloaded;
	class Scene;

	class SceneManager
	{
	public:
		static void Initialize();
		static void LoadScene(const std::string& assetPath);
		static void SaveActiveScene();
		static Scene* GetActiveScene();

	public:
		static void Awake();
		static void Update();

	private:
		static void BuildFinished(BuildCompleteEvent* onBuildFinished);
		static void AssembliesReloaded(OnAssembliesReloaded* reloadedEvent);

	private:
		inline static std::vector<AssetHandle<Scene>> scenes;
		inline static int activeScene = -1;
		inline static std::filesystem::path m_TempDirectory;
		inline static std::filesystem::path s_TempFilename = "tmps.scene";
		inline static const std::string tempSaveFilename = "Assets/Scenes/tmps.yaml";

	};
}