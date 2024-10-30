#pragma once
#include "DockableWindow.h"
#include "AnimationBlueprintUI.h"

namespace Odyssey
{
	namespace Rune
	{
		class BlueprintBuilder;
	}

	class AnimationWindow : public DockableWindow
	{
	public:
		AnimationWindow(size_t windowID);
		void Destroy();

	public:
		virtual void Update() override;
		virtual void Draw() override;
		virtual void OnWindowClose() override;

	private:
		std::shared_ptr<AnimationBlueprint> m_Blueprint;
		AnimationBlueprintUI m_UI;
		std::shared_ptr<BlueprintBuilder> m_Builder;
		uint32_t m_DockspaceID = 0;
	};
}