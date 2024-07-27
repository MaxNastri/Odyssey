#pragma once
#include <VulkanRenderer.h>
#include <Stopwatch.h>

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

	private:
		bool running;
		bool allowRecompile = true;
		float m_TimeSinceLastUpdate = 0.0f;
		inline static std::shared_ptr<VulkanRenderer> renderer = nullptr;

		const float MaxFPS = 1.0f / 144.0f;
	};
}