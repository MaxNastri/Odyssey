#pragma Vertex
struct VertexInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
};

struct VertexOutput
{
    float4 Position : SV_Position;
};

cbuffer SceneData : register(b0)
{
    float4 ViewPos;
    float4x4 View;
    float4x4 ViewProjection;
    float4x4 LightViewProj;
}

cbuffer ModelData : register(b1)
{
    float4x4 Model;
}

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    float4 position = float4(input.Position, 1.0f);
    position.xyz -= (input.Normal * 0.001f);
    output.Position = mul(Model, position);
    output.Position = mul(LightViewProj, output.Position);
    return output;
}