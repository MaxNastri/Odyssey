#include "AnimationWindow.h"
#include "GUIManager.h"
#include "Input.h"

namespace Odyssey
{
	AnimationWindow::AnimationWindow(size_t windowID)
		: DockableWindow("Animation Window", windowID,
			glm::vec2(0, 0), glm::vec2(500, 500), glm::vec2(2, 2))
	{
	}

	void AnimationWindow::Destroy()
	{
	}

	void AnimationWindow::Draw()
	{
		if (!Begin())
			return;

		bp.Update();

		End();
	}

	void AnimationWindow::OnWindowClose()
	{
		GUIManager::DestroyDockableWindow(this);
	}
}