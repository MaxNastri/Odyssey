#pragma once
#include "DockableWindow.h"
#include "Preferences.h"

namespace Odyssey
{
	class PreferencesWindow : public DockableWindow
	{
	public:
		PreferencesWindow(size_t windowID);
		void Destroy();

	public:
		virtual bool Draw() override;
		virtual void OnWindowClose() override;

	private:

	};
}