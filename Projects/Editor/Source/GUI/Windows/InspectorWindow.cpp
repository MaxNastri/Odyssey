#include "InspectorWindow.h"
#include "imgui.h"
#include "EditorEvents.h"
#include "EventSystem.h"
#include "GameObjectInspector.h"
#include "MaterialInspector.h"
#include "MeshRendererInspector.h"
#include "SourceShaderInspector.h"
#include "SourceModelInspector.h"
#include "ShaderInspector.h"
#include "MeshInspector.h"
#include "Texture2DInspector.h"
#include "Events.h"

namespace Odyssey
{
	InspectorWindow::InspectorWindow(std::shared_ptr<Inspector> inspector)
	{
		m_Inspector = inspector;
		m_selectionChangedListener = EventSystem::Listen<GUISelectionChangedEvent>
			([this](GUISelectionChangedEvent* event) { OnGUISelectionChanged(event); });

		m_SceneLoadedListener = EventSystem::Listen<SceneLoadedEvent>
			([this](SceneLoadedEvent* event) { OnSceneLoaded(event); });
	}

	void InspectorWindow::Draw()
	{
		ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Inspector", &open))
		{
			ImGui::End();
			return;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

		if (m_Inspector)
			m_Inspector->Draw();
		
		ImGui::PopStyleVar();
		ImGui::End();
	}

	void InspectorWindow::OnGUISelectionChanged(GUISelectionChangedEvent* event)
	{
		// TODO: Convert this to use ClassName::Type
		// Components/GameObject are setup like this but not assets
		GUISelection& selection = event->Selection;

		if (selection.Type == GameObject::Type)
			// TODO: Pass game object GUID here
			m_Inspector = std::make_shared<GameObjectInspector>(selection.GUID);
		else if (selection.Type == "Material")
			m_Inspector = std::make_shared<MaterialInspector>(selection.GUID);
		else if (selection.Type == "SourceShader")
			m_Inspector = std::make_shared<SourceShaderInspector>(selection.GUID);
		else if (selection.Type == "SourceModel")
			m_Inspector = std::make_shared<SourceModelInspector>(selection.GUID);
		else if (selection.Type == "Shader")
			m_Inspector = std::make_shared<ShaderInspector>(selection.GUID);
		else if (selection.Type == "Mesh")
			m_Inspector = std::make_shared<MeshInspector>(selection.GUID);
		else if (selection.Type == "Texture2D")
			m_Inspector = std::make_shared<TextureInspector>(selection.GUID);
	}

	void InspectorWindow::OnSceneLoaded(SceneLoadedEvent* event)
	{
		// todo: clear selection
	}
}