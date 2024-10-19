#pragma once
#include "RuneNodes.h"
#include "GUID.h"

namespace ax::NodeEditor
{
	struct EditorContext;
}
namespace ImguiExt = ax::NodeEditor;

namespace Odyssey
{
	class Texture2D;

	namespace Rune
	{
		class Blueprint;

		class BlueprintBuilder
		{
		public:
			BlueprintBuilder() = default;
			BlueprintBuilder(Blueprint* blueprint);
			~BlueprintBuilder();

		public:
			void SetEditor();
			void Begin();
			void DrawBlueprint();
			void DrawLabel(const char* label, float4 color);
			void End();

		public:
			void ConnectNewNode(Node* node);
			void NavigateToContent(bool zoomIn = false);

		public:
			void BeginNode(NodeId id);
			void BeginHeader(float4 color = float4(1.0f));
			void BeginInput(PinId id);
			void BeginOutput(PinId id);
			void BeginPin(PinId id, PinIO pinIO);
			void Middle();
			void EndNode();
			void EndHeader();
			void EndInput();
			void EndOutput();
			void EndPin();

		public:
			void OverrideNodeMenu(std::string_view menuName, uint32_t menuID);
			void OverridePinMenu(std::string_view menuName, uint32_t menuID);
			void OverrideLinkMenu(std::string_view menuName, uint32_t menuID);
			void OverrideCreateNodeMenu(std::string_view menuName, uint32_t menuID);

		private:
			void CheckNewLinks();
			void CheckNewNodes();
			void CheckDeletions();
			void CheckContextMenus();

		private:
			enum class Stage { Invalid, Begin, Header, Content, Input, Output, Middle, End };
			bool SetStage(Stage stage);

		private:
			struct Header
			{
				std::shared_ptr<Texture2D> Texture;
				uint64_t TextureID;
				float4 Color;
				float2 Min;
				float2 Max;
				bool HasHeader = false;
			};

			struct DrawingState
			{
				bool HasHeader = false;
				Stage CurrentStage;
				NodeId CurrentNodeID;
				float2 NodeMin;
				float2 NodeMax;
				float2 ContentMin;
				float2 ContentMax;
				float2 MousePos;
				bool CreatingNewNode = false;
				Pin* NewNodeLinkPin = nullptr;
				Pin* ActiveLinkPin = nullptr;
			};

			struct UIOverrides
			{
				std::string NodeMenu = "Default Node Menu";
				uint32_t NodeMenuID = 0;
				bool NodeMenuSet = false;

				std::string PinMenu = "Default Pin Menu";
				uint32_t PinMenuID = 0;
				bool PinMenuSet = false;

				std::string LinkMenu = "Default Link Menu";
				uint32_t LinkMenuID = 0;
				bool LinkMenuSet = false;

				std::string CreateNodeMenu = "Default Create Node Menu";
				uint32_t CreateNodeMenuID = 0;
				bool CreateNodeMenuSet = false;
			};

		private:
			ImguiExt::EditorContext* m_Context = nullptr;
			Blueprint* m_Blueprint = nullptr;
			Header m_Header;
			DrawingState m_DrawingState;
			UIOverrides m_UIOverrides;

		private:
			inline static const GUID& Header_Texture_GUID = 980123767453938719;

		private:
			inline static constexpr float4 Reject_Link_Color = float4(1.0f, 0.0f, 0.0f, 1.0f);
			inline static constexpr float4 Incompatible_Link_Color = float4(0.17f, 0.12f, 0.12f, 0.70f);
			inline static constexpr float4 New_Node_Text_Color = float4(0.12f, 0.17f, 0.12f, 0.70f);
		};
	}
}