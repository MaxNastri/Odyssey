#pragma once
#include "Scene.h"

namespace Odyssey
{
	struct BuildCompleteEvent;
	struct OnAssembliesReloaded;
	class Scene;

	class SceneManager
	{
	public:
		static void LoadScene(const Path& assetPath);
		static void SaveActiveScene();
		static void SaveActiveScene(const Path& path);
		static Scene* GetActiveScene();

	public:
		static void Awake();
		static void OnEditorUpdate();
		static void Update();

	private:
		static void BuildFinished(BuildCompleteEvent* onBuildFinished);
		static void AssembliesReloaded(OnAssembliesReloaded* reloadedEvent);

	private:
		inline static std::vector<std::shared_ptr<Scene>> scenes;
		inline static int activeScene = -1;
		inline static std::filesystem::path m_TempDirectory;
		inline static std::filesystem::path s_TempFilename = "tmps.scene";
		inline static const std::string tempSaveFilename = "Assets/Scenes/tmps.yaml";

	};
}