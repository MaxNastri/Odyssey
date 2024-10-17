#pragma once
#include "DockableWindow.h"
#include "AnimationBlueprint.h"

namespace Odyssey
{
	class AnimationWindow : public DockableWindow
	{
	public:
		AnimationWindow(size_t windowID);
		void Destroy();

	public:
		virtual void Draw() override;
		virtual void OnWindowClose() override;

	private:
		AnimationBlueprint bp;
	};
}