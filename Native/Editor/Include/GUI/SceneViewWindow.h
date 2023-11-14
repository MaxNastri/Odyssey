#pragma once
#include <GUIElement.h>

namespace Odyssey
{
	class SceneViewWindow : public GUIElement
	{
	public:
		virtual void Draw() override;

	private:
		bool open = true;
	};
}