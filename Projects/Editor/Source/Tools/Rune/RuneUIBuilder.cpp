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
        auto& nodes = blueprint->GetNodes();
        for (Node& node : nodes)
        {
            DrawNode(&node);
        }

        auto& links = blueprint->GetLinks();
        for (Link& link : links)
        {
            ImColor color = ImColor(link.Color.r, link.Color.g, link.Color.b, 1.0f);
            ImguiExt::Link(link.ID, link.StartPinID, link.EndPinID, color, 2.0f);
        }
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

                float2 fMin = m_Header.Min - float2(8 - halfBorderWidth, 4 - halfBorderWidth);
                float2 fMax = m_Header.Max + float2(8 - halfBorderWidth, 0);
                ImVec2 min = ImVec2(fMin.x, fMin.y);
                ImVec2 max = ImVec2(fMax.x, fMax.y);
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

    void RuneUIBuilder::DrawNode(Node* node)
    {
        switch (node->Type)
        {
            case NodeType::None:
                return;
            case NodeType::Blueprint:
            case NodeType::Simple:
                DrawSimpleNode(node);
            case NodeType::Comment:
                return;
            case NodeType::Tree:
                DrawTreeNode(node);
            default:
                break;
        }
    }

    void RuneUIBuilder::DrawSimpleNode(Node* node)
    {
        const bool isSimple = node->Type == NodeType::Simple;
        const bool isBlueprint = node->Type == NodeType::Blueprint;

        bool hasOutputDelegates = false;

        for (auto& output : node->Outputs)
        {
            if (output.Type == PinType::Delegate)
            {
                hasOutputDelegates = true;
                break;
            }
        }

        BeginNode(node->ID);

        // Only blueprints draw a header
        if (isBlueprint)
        {
            BeginHeader(node->Color);
            ImGui::Spring(0);
            ImGui::TextUnformatted(node->Name.c_str());
            ImGui::Spring(1);
            ImGui::Dummy(ImVec2(0, 28));
            if (hasOutputDelegates)
            {
                ImGui::BeginVertical("delegates", ImVec2(0, 28));
                ImGui::Spring(1, 0);
                for (auto& output : node->Outputs)
                {
                    if (output.Type != PinType::Delegate)
                        continue;

                    auto alpha = ImGui::GetStyle().Alpha;
                    //if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
                    //    alpha = alpha * (48.0f / 255.0f);

                    ImguiExt::BeginPin(output.ID, ImguiExt::PinKind::Output);
                    ImguiExt::PinPivotAlignment(ImVec2(1.0f, 0.5f));
                    ImguiExt::PinPivotSize(ImVec2(0, 0));
                    ImGui::BeginHorizontal((int32_t)output.ID);
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                    if (!output.Name.empty())
                    {
                        ImGui::TextUnformatted(output.Name.c_str());
                        ImGui::Spring(0);
                    }
                    //DrawPinIcon(output, IsPinLinked(output.ID), (int)(alpha * 255));
                    ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
                    ImGui::EndHorizontal();
                    ImGui::PopStyleVar();
                    ImguiExt::EndPin();

                    //DrawItemRect(ImColor(255, 0, 0));
                }
                ImGui::Spring(1, 0);
                ImGui::EndVertical();
                ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
            }
            else
                ImGui::Spring(0);
            EndHeader();
        }

        for (auto& input : node->Inputs)
        {
            float alpha = ImGui::GetStyle().Alpha;
            //if (newLinkPin && !CanCreateLink(newLinkPin, &input) && &input != newLinkPin)
            //    alpha = alpha * (48.0f / 255.0f);

            BeginInput(input.ID);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
            DrawPinIcon(input, IsPinLinked(input), alpha);
            ImGui::Spring(0);
            if (!input.Name.empty())
            {
                ImGui::TextUnformatted(input.Name.c_str());
                ImGui::Spring(0);
            }
            if (input.Type == PinType::Bool)
            {
                ImGui::Button("Hello");
                ImGui::Spring(0);
            }
            ImGui::PopStyleVar();
            EndInput();
        }

        for (auto& output : node->Outputs)
        {
            if (!isSimple && output.Type == PinType::Delegate)
                continue;

            auto alpha = ImGui::GetStyle().Alpha;
            //if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
            //    alpha = alpha * (48.0f / 255.0f);

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
            BeginOutput(output.ID);
            if (output.Type == PinType::String)
            {
                static char buffer[128] = "Edit Me\nMultiline!";
                static bool wasActive = false;

                ImGui::PushItemWidth(100.0f);
                ImGui::InputText("##edit", buffer, 127);
                ImGui::PopItemWidth();
                if (ImGui::IsItemActive() && !wasActive)
                {
                    ImguiExt::EnableShortcuts(false);
                    wasActive = true;
                }
                else if (!ImGui::IsItemActive() && wasActive)
                {
                    ImguiExt::EnableShortcuts(true);
                    wasActive = false;
                }
                ImGui::Spring(0);
            }

            if (!output.Name.empty())
            {
                ImGui::Spring(0);
                ImGui::TextUnformatted(output.Name.c_str());
            }

            ImGui::Spring(0);
            DrawPinIcon(output, IsPinLinked(output), alpha);
            ImGui::PopStyleVar();
            EndOutput();
        }

        EndNode();
    }

    void RuneUIBuilder::DrawTreeNode(Node* node)
    {
        if (node->Type != NodeType::Tree)
            return;

        const float rounding = 5.0f;
        const float padding = 12.0f;

        const auto pinBackground = ImguiExt::GetStyle().Colors[ImguiExt::StyleColor_NodeBg];

        ImguiExt::PushStyleColor(ImguiExt::StyleColor_NodeBg, ImColor(128, 128, 128, 200));
        ImguiExt::PushStyleColor(ImguiExt::StyleColor_NodeBorder, ImColor(32, 32, 32, 200));
        ImguiExt::PushStyleColor(ImguiExt::StyleColor_PinRect, ImColor(60, 180, 255, 150));
        ImguiExt::PushStyleColor(ImguiExt::StyleColor_PinRectBorder, ImColor(60, 180, 255, 150));

        ImguiExt::PushStyleVar(ImguiExt::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
        ImguiExt::PushStyleVar(ImguiExt::StyleVar_NodeRounding, rounding);
        ImguiExt::PushStyleVar(ImguiExt::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
        ImguiExt::PushStyleVar(ImguiExt::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
        ImguiExt::PushStyleVar(ImguiExt::StyleVar_LinkStrength, 0.0f);
        ImguiExt::PushStyleVar(ImguiExt::StyleVar_PinBorderWidth, 1.0f);
        ImguiExt::PushStyleVar(ImguiExt::StyleVar_PinRadius, 5.0f);
        ImguiExt::BeginNode(node->ID);

        ImGui::BeginVertical((int32_t)node->ID);
        ImGui::BeginHorizontal("inputs");
        ImGui::Spring(0, padding * 2);

        ImRect inputsRect;
        int inputAlpha = 200;
        if (!node->Inputs.empty())
        {
            auto& pin = node->Inputs[0];
            ImGui::Dummy(ImVec2(0, padding));
            ImGui::Spring(1, 0);
            inputsRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

            ImguiExt::PushStyleVar(ImguiExt::StyleVar_PinArrowSize, 10.0f);
            ImguiExt::PushStyleVar(ImguiExt::StyleVar_PinArrowWidth, 10.0f);
#if IMGUI_VERSION_NUM > 18101
            ImguiExt::PushStyleVar(ImguiExt::StyleVar_PinCorners, ImDrawFlags_RoundCornersBottom);
#else
            ImguiExt::PushStyleVar(ImguiExt::StyleVar_PinCorners, 12);
#endif
            ImguiExt::BeginPin(pin.ID, ImguiExt::PinKind::Input);
            ImguiExt::PinPivotRect(inputsRect.GetTL(), inputsRect.GetBR());
            ImguiExt::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
            ImguiExt::EndPin();
            ImguiExt::PopStyleVar(3);

            //if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
            //    inputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
        }
        else
            ImGui::Dummy(ImVec2(0, padding));

        ImGui::Spring(0, padding * 2);
        ImGui::EndHorizontal();

        ImGui::BeginHorizontal("content_frame");
        ImGui::Spring(1, padding);

        ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
        ImGui::Dummy(ImVec2(160, 0));
        ImGui::Spring(1);
        ImGui::TextUnformatted(node->Name.c_str());
        ImGui::Spring(1);
        ImGui::EndVertical();
        auto contentRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

        ImGui::Spring(1, padding);
        ImGui::EndHorizontal();

        ImGui::BeginHorizontal("outputs");
        ImGui::Spring(0, padding * 2);

        ImRect outputsRect;
        int outputAlpha = 200;
        if (!node->Outputs.empty())
        {
            auto& pin = node->Outputs[0];
            ImGui::Dummy(ImVec2(0, padding));
            ImGui::Spring(1, 0);
            outputsRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

#if IMGUI_VERSION_NUM > 18101
            ImguiExt::PushStyleVar(ImguiExt::StyleVar_PinCorners, ImDrawFlags_RoundCornersTop);
#else
            ImguiExt::PushStyleVar(ImguiExt::StyleVar_PinCorners, 3);
#endif
            ImguiExt::BeginPin(pin.ID, ImguiExt::PinKind::Output);
            ImguiExt::PinPivotRect(outputsRect.GetTL(), outputsRect.GetBR());
            ImguiExt::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
            ImguiExt::EndPin();
            ImguiExt::PopStyleVar();

            //if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
            //    outputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
        }
        else
            ImGui::Dummy(ImVec2(0, padding));

        ImGui::Spring(0, padding * 2);
        ImGui::EndHorizontal();

        ImGui::EndVertical();

        ImguiExt::EndNode();
        ImguiExt::PopStyleVar(7);
        ImguiExt::PopStyleColor(4);

        auto drawList = ImguiExt::GetNodeBackgroundDrawList(node->ID);
        const ImDrawFlags topRoundCornersFlags = ImDrawFlags_RoundCornersTop;
        const ImDrawFlags bottomRoundCornersFlags = ImDrawFlags_RoundCornersBottom;

        {
            float2 inTopLeft = float2(inputsRect.GetTL().x, inputsRect.GetTL().y);

            float2 outTopLeft = float2(outputsRect.GetTL().x, outputsRect.GetTL().y);
            float2 outBotRight = float2(outputsRect.GetBR().x, outputsRect.GetBR().y);

            drawList->AddRectFilled(ImVec2(inTopLeft.x, inTopLeft.y + 1), inputsRect.GetBR(),
                IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, bottomRoundCornersFlags);
            drawList->AddRect(ImVec2(inTopLeft.x, inTopLeft.y + 1), inputsRect.GetBR(),
                IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, bottomRoundCornersFlags);
            drawList->AddRectFilled(outputsRect.GetTL(), ImVec2(outBotRight.x, outBotRight.y - 1),
                IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, topRoundCornersFlags);
            drawList->AddRect(outputsRect.GetTL(), ImVec2(outBotRight.x, outBotRight.y - 1),
                IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, topRoundCornersFlags);
            
            drawList->AddRectFilled(contentRect.GetTL(), contentRect.GetBR(), IM_COL32(24, 64, 128, 200), 0.0f);
            drawList->AddRect(
                contentRect.GetTL(),
                contentRect.GetBR(),
                IM_COL32(48, 128, 255, 100), 0.0f);
        }
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

    bool RuneUIBuilder::IsPinLinked(const Pin& pin)
    {
        if (!pin.ID)
            return false;

        // TODO: IMPLEMENT SOMEWHERE

        return false;
    }

    void RuneUIBuilder::DrawPinIcon(const Pin& pin, bool connected, float alpha)
    {
        using namespace ImGui::Widgets;

        IconType iconType;
        float4  color = float4(GetIconColor(pin.Type), alpha);
        
        switch (pin.Type)
        {
            case PinType::Flow:
                iconType = IconType::Flow;
                break;
            case PinType::Bool:
                iconType = IconType::Circle;
                break;
            case PinType::Int:
                iconType = IconType::Circle;
                break;
            case PinType::Float:
                iconType = IconType::Circle;
                break;
            case PinType::String:
                iconType = IconType::Circle;
                break;
            case PinType::Object:
                iconType = IconType::Circle;
                break;
            case PinType::Function:
                iconType = IconType::Circle;
                break;
            case PinType::Delegate:
                iconType = IconType::Square;
                break;
            default:
                return;
        }

        ImColor pinColor = ImColor(color.r, color.g, color.b, color.a);
        ImColor pinInnerColor = ImColor(Pin_Inner_Color.r, Pin_Inner_Color.g, Pin_Inner_Color.b, alpha);

        ImGui::Widgets::Icon(ImVec2(Pin_Icon_Size.x, Pin_Icon_Size.y), iconType, connected, pinColor, pinInnerColor);
    }

    float3 RuneUIBuilder::GetIconColor(PinType pinType)
    {
        switch (pinType)
        {
            default:
            case PinType::Flow:     return float3(1.0f, 1.0f, 1.0f);
            case PinType::Bool:     return float3(0.86f, 0.18f, 0.18f);
            case PinType::Int:      return float3(0.26f, 0.79f, 0.61f);
            case PinType::Float:    return float3(0.57f, 0.88f, 0.29f);
            case PinType::String:   return float3(0.48f, 0.08f, 0.6f);
            case PinType::Object:   return float3(0.2f, 0.59f, 0.84f);
            case PinType::Function: return float3(0.86f, 0.0f, 0.72f);
            case PinType::Delegate: return float3(1.0f, 0.18f, 0.18f);
        }
    }
}