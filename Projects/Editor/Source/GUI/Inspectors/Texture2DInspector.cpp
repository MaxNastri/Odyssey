#include "Texture2DInspector.h"
#include "AssetManager.h"
#include "Texture2D.h"
#include "SourceTexture.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Renderer.h"

namespace Odyssey
{
	TextureInspector::TextureInspector(GUID guid)
	{
		if (m_Texture = AssetManager::LoadAsset<Texture2D>(guid))
		{
			m_GUIDDrawer = StringDrawer("GUID", m_Texture->GetGUID().String(), true);
			m_NameDrawer = StringDrawer("Name", m_Texture->GetName(), false,
				[this](std::string_view name) { OnNameChanged(name); });
			m_TypeDrawer = StringDrawer("Type", m_Texture->GetType(), true);
			m_SourceAssetDrawer = AssetFieldDrawer("Source Asset", m_Texture->GetSourceAsset(), SourceTexture::Type,
				[this](GUID sourceGUID) { OnSourceAssetchanged(sourceGUID); });

			m_PreviewTexture = Renderer::AddImguiTexture(m_Texture);
		}
	}

	static bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
	{
		using namespace ImGui;
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		ImGuiID id = window->GetID("##Splitter");
		ImRect bb;
		bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
		bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);

		ImGui::SetCursorScreenPos(float2(bb.Min.x, bb.Max.y) + g.Style.WindowPadding);
		return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
	}

	bool TextureInspector::Draw()
	{
		bool modified = false;

		modified |= m_GUIDDrawer.Draw();
		modified |= m_TypeDrawer.Draw();
		modified |= m_NameDrawer.Draw();
		modified |= m_SourceAssetDrawer.Draw();

		static float bottomPaneHeight = 400.0f;
		static float topPaneHeight = 800.0f;
		Splitter(false, 4.0f, &bottomPaneHeight, &topPaneHeight, 50.0f, 50.0f);

		ImGui::BeginChild("TexturePreview");
		float2 windowSize = ImGui::GetContentRegionAvail();
		float2 textureSize = float2(m_Texture->GetWidth(), m_Texture->GetHeight());
		float xScale = windowSize.x / textureSize.x;
		float yScale = windowSize.y / textureSize.y;
		float scale = std::min(xScale, yScale);

		textureSize *= scale;
		float2 upperLeft = (windowSize / 2.0f) - (textureSize / 2.0f);
		ImGui::SetCursorPos(upperLeft);
		ImGui::Image((void*)m_PreviewTexture, textureSize);
		ImGui::EndChild();

		return modified;
	}

	void TextureInspector::OnNameChanged(std::string_view name)
	{
		if (m_Texture)
		{
			m_Texture->SetName(name);
			m_Texture->Save();
		}
	}
	void TextureInspector::OnSourceAssetchanged(GUID sourceGUID)
	{
		if (m_Texture)
		{
			m_Texture->SetSourceAsset(sourceGUID);
			m_Texture->Save();
		}
	}
}