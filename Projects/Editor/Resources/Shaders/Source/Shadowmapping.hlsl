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
    float4x4 ViewProjection;
}

cbuffer ModelData : register(b1)
{
    float4x4 Model;
}

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    float4 position = float4(input.Position, 1.0f);
    position.xyz = position.xyz;
    
    output.Position = mul(Model, position);
    output.Position = mul(ViewProjection, output.Position);
    return output;
}