#pragma once
#include <VulkanRenderer.h>
#include <Stopwatch.h>
#include "EditorEvents.h"

using namespace Odyssey;
using namespace Odyssey;

namespace Odyssey
{
	class GUIElement;
}

namespace Odyssey
{
	class Application
	{
	public:
		Application();
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

		const float MaxFPS = 1.0f / 144.0f;
		inline static constexpr std::string_view TEMP_SCENE_FILE = "tmps.scene";
	};
}