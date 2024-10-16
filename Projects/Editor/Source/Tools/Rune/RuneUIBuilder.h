#pragma once
#include "RuneNodes.h"
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
			enum class Stage { Invalid, Begin, Header, Content, Input, Output, Middle, End };

		public:
			void BeginNode(NodeId id);
			void EndNode();

		public:
			void BeginHeader(float4 color = float4(1.0f));
			void EndHeader();

		public:
			void BeginInput(PinId id);
			void EndInput();

		public:
			void BeginOutput(PinId id);
			void EndOutput();

		public:
			void BeginPin(PinId id, PinIO pinIO);
			void EndPin();

		public:
			//void DrawNode(Node* node);
			//void DrawSimpleNode(Node* node);
			//void DrawTreeNode(Node* node);

		public:
			void Middle();
			bool SetStage(Stage stage);

		public:
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

		public:
			Header m_Header;
			DrawingState m_DrawingState;

		public:
			inline static const GUID& Header_Texture_GUID = 980123767453938719;
		};
	}
}