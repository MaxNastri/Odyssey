#pragma once
#include <VulkanRenderer.h>
#include <Stopwatch.h>
#include "EditorEvents.h"
#include "ScriptCompiler.h"

namespace Odyssey
{
	class Editor
	{
	public:
		Editor();
		void Run();
		void Exit();

	public:
		static std::shared_ptr<VulkanRenderer> GetRenderer() { return renderer; }
		
	private:
		void SetupEditorGUI();
		void CreateRenderPasses();
		void OnPlaymodeStateChanged(PlaymodeStateChangedEvent* event);

	private:
		bool running;
		bool allowRecompile = true;
		bool m_UpdateScripts = false;
		float m_TimeSinceLastUpdate = 0.0f;
		inline static std::shared_ptr<VulkanRenderer> renderer = nullptr;
		std::unique_ptr<ScriptCompiler> m_ScriptCompiler;
		const float MaxFPS = 1.0f / 144.0f;
		inline static constexpr std::string_view TEMP_SCENE_FILE = "tmps.scene";
	};
}