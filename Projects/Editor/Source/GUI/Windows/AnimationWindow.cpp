#include "AnimationWindow.h"
#include "imgui.h"

namespace Odyssey
{
	AnimationWindow::AnimationWindow(size_t windowID)
		: DockableWindow("Animation Window", windowID,
			glm::vec2(0, 0), glm::vec2(500,500), glm::vec2(2, 2))
	{
	}

	void AnimationWindow::Destroy()
	{
	}

	void AnimationWindow::Draw()
	{
		bp.Update();
	}

	void AnimationWindow::OnWindowClose()
	{
	}
}