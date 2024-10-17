#include "RuneUIBuilder.h"
#include "Blueprint.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_node_editor.h"
#include "widgets.h"
#include "Renderer.h"
#include "AssetManager.h"
#include "ResourceManager.h"
#include "Texture2D.h"

namespace Odyssey::Rune
{
    namespace ImguiExt = ax::NodeEditor;

	RuneUIBuilder::RuneUIBuilder()
	{
        m_DrawingState.CurrentNodeID = 0;
        m_DrawingState.CurrentStage = Stage::Invalid;

        m_Header.Texture = AssetManager::LoadAsset<Texture2D>(Header_Texture_GUID);
        m_Header.TextureID = Renderer::AddImguiTexture(m_Header.Texture);
	}

    void RuneUIBuilder::DrawBlueprint(Blueprint* blueprint)
    {
    }

    void RuneUIBuilder::DrawLabel(const char* label, float4 color)
    {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
        float2 size = ImGui::CalcTextSize(label);
        
        float2 padding = ImGui::GetStyle().FramePadding;
        float2 spacing = ImGui::GetStyle().ItemSpacing;
        float2 cursorPos = ImGui::GetCursorPos();
        float2 cursorScreenPos = ImGui::GetCursorScreenPos();
        float2 offset = float2(spacing.x, -spacing.y);

        ImGui::SetCursorPos(cursorPos + offset);
        
        float2 rectMin = cursorScreenPos - padding + float2(spacing.x, 0.0f);
        float2 rectMax = cursorScreenPos + size + float2(spacing.x + padding.x, 0.0f);
        
        auto drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(rectMin, rectMax, ImColor(color.r, color.g, color.b, color.a), size.y * 0.15f);
        ImGui::TextUnformatted(label);
    }


	void RuneUIBuilder::BeginNode(NodeId id)
	{
        m_DrawingState.HasHeader = false;
        m_Header.Min = m_Header.Max = float2(0.0f);

		ImguiExt::PushStyleVar(ImguiExt::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));

		ImguiExt::BeginNode(id);

		ImGui::PushID((int32_t)id);
        m_DrawingState.CurrentNodeID = id;

		SetStage(Stage::Begin);
	}

	void RuneUIBuilder::EndNode()
	{
        SetStage(Stage::End);

        ImguiExt::EndNode();

        if (ImGui::IsItemVisible())
        {
            const float halfBorderWidth = ImguiExt::GetStyle().NodeBorderWidth * 0.5f;
            int32_t alpha = (int32_t)(255 * ImGui::GetStyle().Alpha);
            ImColor headerColor = ImColor(m_Header.Color.r, m_Header.Color.g, m_Header.Color.b, m_Header.Color.a);

            ImDrawList* drawList = ImguiExt::GetNodeBackgroundDrawList(m_DrawingState.CurrentNodeID);

            if ((m_Header.Max.x > m_Header.Min.x) && (m_Header.Max.y > m_Header.Min.y) && m_Header.Texture)
            {
                const ImVec2 uv = ImVec2(
                    (m_Header.Max.x - m_Header.Min.x) / (float)(4.0f * m_Header.Texture->GetWidth()),
                    (m_Header.Max.y - m_Header.Min.y) / (float)(4.0f * m_Header.Texture->GetHeight()));

                float2 min = m_Header.Min - float2(8.0f - halfBorderWidth, 4.0f - halfBorderWidth);
                float2 max = m_Header.Max + float2(8.0f - halfBorderWidth, 0.0f);
                ImVec2 uvMin = ImVec2(0.0f, 0.0f);
#if IMGUI_VERSION_NUM > 18101
                ImDrawFlags drawFlags = ImDrawFlags_RoundCornersTop;
#else
                ImDrawFlags drawFlags = 1 | 2;
#endif
                drawList->AddImageRounded((ImTextureID)m_Header.TextureID,
                    min, max,
                    uvMin, uv,
                    headerColor, ImguiExt::GetStyle().NodeRounding, drawFlags);

                if (m_DrawingState.ContentMin.y > m_Header.Max.y)
                {
                    drawList->AddLine(
                        ImVec2(m_Header.Min.x - (8 - halfBorderWidth), m_Header.Max.y - 0.5f),
                        ImVec2(m_Header.Max.x + (8 - halfBorderWidth), m_Header.Max.y - 0.5f),
                        ImColor(255, 255, 255, 96 * alpha / (3 * 255)), 1.0f);
                }
            }
        }

        m_DrawingState.CurrentNodeID = 0;

        ImGui::PopID();

        ImguiExt::PopStyleVar();

        SetStage(Stage::Invalid);
	}

    void RuneUIBuilder::BeginHeader(float4 color)
    {
        m_Header.Color = color;
        SetStage(Stage::Header);
    }

    void RuneUIBuilder::EndHeader()
    {
        SetStage(Stage::Content);
    }

    void RuneUIBuilder::BeginInput(PinId id)
    {
        if (m_DrawingState.CurrentStage == Stage::Begin)
            SetStage(Stage::Content);

        const auto applyPadding = (m_DrawingState.CurrentStage == Stage::Input);

        SetStage(Stage::Input);

        if (applyPadding)
            ImGui::Spring(0);

        BeginPin(id, PinIO::Input);

        ImGui::BeginHorizontal((int32_t)id);
    }

    void RuneUIBuilder::EndInput()
    {
        ImGui::EndHorizontal();
        EndPin();
    }

    void RuneUIBuilder::BeginOutput(PinId id)
    {
        if (m_DrawingState.CurrentStage == Stage::Begin)
            SetStage(Stage::Content);

        bool applyPadding = m_DrawingState.CurrentStage == Stage::Output;

        SetStage(Stage::Output);

        if (applyPadding)
            ImGui::Spring(0);

        BeginPin(id, PinIO::Output);

        ImGui::BeginHorizontal((int32_t)id);
    }

    void RuneUIBuilder::EndOutput()
    {
        ImGui::EndHorizontal();
        EndPin();
    }

    void RuneUIBuilder::BeginPin(PinId id, PinIO pinIO)
    {
        ImguiExt::BeginPin(id, pinIO == PinIO::Input ? ImguiExt::PinKind::Input : ImguiExt::PinKind::Output);
    }

    void RuneUIBuilder::EndPin()
    {
        ImguiExt::EndPin();

        // #debug
        // ImGui::GetWindowDrawList()->AddRectFilled(
        //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 64));
    }

    void RuneUIBuilder::Middle()
    {
        if (m_DrawingState.CurrentStage == Stage::Begin)
            SetStage(Stage::Content);

        SetStage(Stage::Middle);
    }

    bool RuneUIBuilder::SetStage(Stage stage)
    {
        if (stage == m_DrawingState.CurrentStage)
            return false;

        Stage oldStage = m_DrawingState.CurrentStage;
        m_DrawingState.CurrentStage = stage;

        ImVec2 cursor;
        switch (oldStage)
        {
            case Stage::Begin:
                break;

            case Stage::Header:
                ImGui::EndHorizontal();
                m_Header.Min = float2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);
                m_Header.Max = float2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y);

                // spacing between header and content
                ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.y * 2.0f);

                break;

            case Stage::Content:
                break;

            case Stage::Input:
                ImguiExt::PopStyleVar(2);

                ImGui::Spring(1, 0);
                ImGui::EndVertical();

                // #debug
                // ImGui::GetWindowDrawList()->AddRect(
                //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

                break;

            case Stage::Middle:
                ImGui::EndVertical();

                // #debug
                // ImGui::GetWindowDrawList()->AddRect(
                //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

                break;

            case Stage::Output:
                ImguiExt::PopStyleVar(2);

                ImGui::Spring(1, 0);
                ImGui::EndVertical();

                // #debug
                // ImGui::GetWindowDrawList()->AddRect(
                //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

                break;

            case Stage::End:
                break;

            case Stage::Invalid:
                break;
        }

        switch (stage)
        {
            case Stage::Begin:
                ImGui::BeginVertical("node");
                break;

            case Stage::Header:
                m_DrawingState.HasHeader = true;

                ImGui::BeginHorizontal("header");
                break;

            case Stage::Content:
                if (oldStage == Stage::Begin)
                    ImGui::Spring(0);

                ImGui::BeginHorizontal("content");
                ImGui::Spring(0, 0);
                break;

            case Stage::Input:
                ImGui::BeginVertical("inputs", ImVec2(0, 0), 0.0f);

                ImguiExt::PushStyleVar(ImguiExt::StyleVar_PivotAlignment, ImVec2(0, 0.5f));
                ImguiExt::PushStyleVar(ImguiExt::StyleVar_PivotSize, ImVec2(0, 0));

                if (!m_DrawingState.HasHeader)
                    ImGui::Spring(1, 0);
                break;

            case Stage::Middle:
                ImGui::Spring(1);
                ImGui::BeginVertical("middle", ImVec2(0, 0), 1.0f);
                break;

            case Stage::Output:
                if (oldStage == Stage::Middle || oldStage == Stage::Input)
                    ImGui::Spring(1);
                else
                    ImGui::Spring(1, 0);
                ImGui::BeginVertical("outputs", ImVec2(0, 0), 1.0f);

                ImguiExt::PushStyleVar(ImguiExt::StyleVar_PivotAlignment, ImVec2(1.0f, 0.5f));
                ImguiExt::PushStyleVar(ImguiExt::StyleVar_PivotSize, ImVec2(0, 0));

                if (!m_DrawingState.HasHeader)
                    ImGui::Spring(1, 0);
                break;

            case Stage::End:
                if (oldStage == Stage::Input)
                    ImGui::Spring(1, 0);
                if (oldStage != Stage::Begin)
                    ImGui::EndHorizontal();
                m_DrawingState.ContentMin = float2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);
                m_DrawingState.ContentMax = float2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y);

                //ImGui::Spring(0);
                ImGui::EndVertical();
                m_DrawingState.NodeMin = float2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);
                m_DrawingState.NodeMax = float2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y);
                break;

            case Stage::Invalid:
                break;
        }

        return true;
    }
}