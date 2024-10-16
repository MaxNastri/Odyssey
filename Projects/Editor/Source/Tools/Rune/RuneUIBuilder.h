#pragma once
#include "Node.h"
#include "Pin.h"
#include "Resource.h"
#include "GUID.h"

namespace Odyssey
{
	class Texture2D;

	namespace Rune
	{
		class Blueprint;

		class RuneUIBuilder
		{
		public:
			RuneUIBuilder();

		public:
			void DrawBlueprint(Blueprint* blueprint);
			void DrawLabel(const char* label, float4 color);

		public:
			static float3 GetIconColor(PinType pinType);

		private:
			enum class Stage { Invalid, Begin, Header, Content, Input, Output, Middle, End };

		private:
			void BeginNode(NodeId id);
			void EndNode();

		private:
			void BeginHeader(float4 color = float4(1.0f));
			void EndHeader();

		private:
			void BeginInput(PinId id);
			void EndInput();

		private:
			void BeginOutput(PinId id);
			void EndOutput();

		private:
			void BeginPin(PinId id, PinIO pinIO);
			void EndPin();

		private:
			void DrawNode(Node* node);
			void DrawSimpleNode(Node* node);
			void DrawTreeNode(Node* node);

		private:
			void Middle();
			bool SetStage(Stage stage);

		private:
			bool IsPinLinked(const Pin& pin);
			void DrawPinIcon(const Pin& pin, bool connected, float alpha);


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
			};

		private:
			Header m_Header;
			DrawingState m_DrawingState;

		private:
			inline static const GUID& Header_Texture_GUID = 980123767453938719;
			inline static constexpr float2 Pin_Icon_Size = float2(24.0f);
			inline static constexpr float3 Pin_Inner_Color = float3(0.12f);
		};
	}
}