#pragma once
#include <VulkanRenderer.h>
#include <Stopwatch.h>
#include "EditorEvents.h"
#include "ScriptCompiler.h"
#include "EventSystem.h"

namespace Odyssey
{
	struct BuildCompleteEvent;

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

	private: // Events
		void OnPlaymodeStateChanged(PlaymodeStateChangedEvent* event);
		void OnBuildComplete(BuildCompleteEvent* event);
			
	private:
		bool running;
		bool m_UpdateScripts = false;
		float m_TimeSinceLastUpdate = 0.0f;

	private:
		inline static std::shared_ptr<VulkanRenderer> renderer = nullptr;
		std::unique_ptr<ScriptCompiler> m_ScriptCompiler;

	private:
		std::shared_ptr<IEventListener> m_BuildCompleteListener;
		std::shared_ptr<IEventListener> m_PlaymodeStateListener;

	private: // Constants
		const float MaxFPS = 1.0f / 144.0f;
		inline static constexpr std::string_view TEMP_SCENE_FILE = "tmps.scene";
	};
}