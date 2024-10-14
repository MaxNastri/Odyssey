#pragma once
#include "Node.h"
#include "Pin.h"

namespace Odyssey::Rune
{
    typedef uintptr_t PinId;

	class RuneUIBuilder
	{
    public:
        RuneUIBuilder(uint64_t texture = 0, int32_t textureWidth = 0, uint64_t textureHeight = 0);

        void Begin(NodeId id);
        void DrawNode(Node* node);
        void End();

        void Header(float4 color = float4(1.0f));
        void EndHeader();

        void Input(PinId id);
        void EndInput();

        void Middle();

        void Output(PinId id);
        void EndOutput();

    private:
        void DrawSimpleNode(Node* node);

    private:
        enum class Stage
        {
            Invalid,
            Begin,
            Header,
            Content,
            Input,
            Output,
            Middle,
            End
        };

        bool SetStage(Stage stage);

        void Pin(PinId id, PinIO pinIO);
        void EndPin();

        uint64_t HeaderTextureId;
        int32_t         HeaderTextureWidth;
        int32_t         HeaderTextureHeight;
        NodeId      CurrentNodeId;
        Stage       CurrentStage;
        float4       HeaderColor;
        float2      NodeMin;
        float2      NodeMax;
        float2      HeaderMin;
        float2      HeaderMax;
        float2      ContentMin;
        float2      ContentMax;
        bool        HasHeader;
	};
}