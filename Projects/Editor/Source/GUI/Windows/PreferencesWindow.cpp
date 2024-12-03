#include "PreferencesWindow.h"
#include "GUIManager.h"

namespace Odyssey
{
    PreferencesWindow::PreferencesWindow(size_t windowID)
    {
    }
    void PreferencesWindow::Destroy()
    {
    }
    bool PreferencesWindow::Draw()
    {
        return false;
    }
    void PreferencesWindow::OnWindowClose()
    {
        GUIManager::DestroyDockableWindow(this);
    }
}