#pragma once
#include "GUIElement.h"

namespace Odyssey
{
	class EditorActionsBar : public GUIElement
	{
	public:
		EditorActionsBar();

	public:
		virtual bool Draw() override;

	private:
		int32_t m_WindowFlags = 0;
		bool m_WindowOpen = true;
	};
}