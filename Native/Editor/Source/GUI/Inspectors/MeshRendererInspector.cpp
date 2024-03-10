#include "MeshRendererInspector.h"
#include "imgui.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "AssetManager.h"
#include "GameObject.h"

namespace Odyssey
{
    MeshRendererInspector::MeshRendererInspector(GameObject* gameObject)
    {
        m_GameObject = gameObject;
        m_MeshRenderer = gameObject->GetComponent<MeshRenderer>();

        m_MeshDrawer = AssetFieldDrawer("Mesh", m_MeshRenderer->GetMesh().Get()->GetGUID(), "Mesh",
            [gameObject](const std::string& guid) { OnMeshModified(gameObject, guid); });

        m_MaterialDrawer = AssetFieldDrawer("Material", m_MeshRenderer->GetMaterial().Get()->GetGUID(), "Material",
            [gameObject](const std::string& guid) { OnMaterialModified(gameObject, guid); });
    }

    void MeshRendererInspector::Draw()
    {
        if (ImGui::CollapsingHeader("MeshRenderer", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::BeginTable("MeshRendererInspector", 2, ImGuiTableFlags_::ImGuiTableFlags_SizingMask_))
            {
                ImGui::TableSetupColumn("##A", 0, 0.4f);
                m_MeshDrawer.Draw();
                ImGui::TableNextRow();
                m_MaterialDrawer.Draw();
                ImGui::TableNextRow();
                ImGui::EndTable();
            }
        }
    }
    void MeshRendererInspector::OnMeshModified(GameObject* gameObject, const std::string& guid)
    {
        if (MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>())
        {
            meshRenderer->SetMesh(AssetManager::LoadMeshByGUID(guid));
        }
    }

    void MeshRendererInspector::OnMaterialModified(GameObject* gameObject, const std::string& guid)
    {
        if (MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>())
        {
            meshRenderer->SetMaterial(AssetManager::LoadMaterialByGUID(guid));
        }
    }
}