#include "SceneViewWindow.h"
#include <imgui.h>

namespace Odyssey
{
	void SceneViewWindow::Draw()
	{
		ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Scene View Window", &open))
		{
			ImGui::End();
			return;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

		// Draw

		ImGui::PopStyleVar();
		ImGui::End();
	}
}