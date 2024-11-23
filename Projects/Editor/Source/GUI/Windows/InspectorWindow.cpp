#include "EditorEvents.h"
#include "Events.h"
#include "EventSystem.h"
#include "GameObjectInspector.h"
#include "GUIManager.h"
#include "imgui.h"
#include "InspectorWindow.h"
#include "MaterialInspector.h"
#include "MeshInspector.h"
#include "MeshRendererInspector.h"
#include "ShaderInspector.h"
#include "SourceModelInspector.h"
#include "SourceShaderInspector.h"
#include "SourceTextureInspector.h"
#include "Texture2DInspector.h"

namespace Odyssey
{
	std::map<std::string, std::function<Ref<Inspector>(GUID)>> s_CreateInspectorFuncs;

	template<typename T>
	static void RegisterInspectorType(const std::string& assetType)
	{
		s_CreateInspectorFuncs[assetType] = [](GUID guid) { return new T(guid); };
	}

	InspectorWindow::InspectorWindow(Ref<Inspector> inspector)
	{
		m_Inspector = inspector;
		m_selectionChangedListener = EventSystem::Listen<GUISelectionChangedEvent>
			([this](GUISelectionChangedEvent* event) { OnGUISelectionChanged(event); });

		m_SceneLoadedListener = EventSystem::Listen<SceneLoadedEvent>
			([this](SceneLoadedEvent* event) { OnSceneLoaded(event); });

		RegisterInspectorType<GameObjectInspector>(GameObject::Type);
		RegisterInspectorType<MaterialInspector>(Material::Type);
		RegisterInspectorType<TextureInspector>(Texture2D::Type);
		RegisterInspectorType<ShaderInspector>(Shader::Type);
		RegisterInspectorType<MeshInspector>(Mesh::Type);

		RegisterInspectorType<SourceShaderInspector>(SourceShader::Type);
		RegisterInspectorType<SourceModelInspector>(SourceModel::Type);
		RegisterInspectorType<SourceTextureInspector>(SourceTexture::Type);
	}

	bool InspectorWindow::Draw()
	{
		bool modified = false;

		ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Inspector", &open))
		{
			ImGui::End();
			return modified;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

		if (m_Inspector)
			m_Inspector->Draw();

		ImGui::PopStyleVar();
		ImGui::End();
		return modified;
	}

	void InspectorWindow::OnWindowClose()
	{
		GUIManager::DestroyDockableWindow(this);
	}

	void InspectorWindow::OnGUISelectionChanged(GUISelectionChangedEvent* event)
	{
		GUISelection& selection = event->Selection;
		if (s_CreateInspectorFuncs.contains(selection.Type))
		{
			auto& createFunc = s_CreateInspectorFuncs[selection.Type];
			m_Inspector = createFunc(selection.GUID);
		}
	}

	void InspectorWindow::OnSceneLoaded(SceneLoadedEvent* event)
	{
		// todo: clear selection
	}
}