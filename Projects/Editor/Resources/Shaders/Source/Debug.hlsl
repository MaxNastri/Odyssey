#pragma Vertex
struct VertexInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float4 Color : COLOR;
    float2 TexCoord0 : TEXCOORD0;
    float4 BoneIndices : BLENDINDICES0;
    float4 BoneWeights : BLENDWEIGHT0;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float4 Color : COLOR0;
    float2 TexCoord0 : TEXCOORD0;
    float4 BoneIndices : BLENDINDICES0;
    float4 BoneWeights : BLENDWEIGHT0;
};

cbuffer SceneData : register(b0)
{
    float4x4 View;
    float4x4 ViewProjection;
}

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    
    float4 position = float4(input.Position, 1.0f);
    output.Position = mul(ViewProjection, position);
    output.Normal = input.Normal;
    output.Tangent = input.Tangent;
    output.Color = input.Color;
    output.TexCoord0 = input.TexCoord0;
    output.BoneIndices = input.BoneIndices;
    output.BoneWeights = input.BoneWeights;
    return output;
}

#pragma Fragment
struct PixelInput
{
    float4 Position : SV_Position;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float4 Color : COLOR0;
    float2 TexCoord0 : TEXCOORD0;
    float4 BoneIndices : BLENDINDICES0;
    float4 BoneWeights : BLENDWEIGHT0;
};

float4 main(PixelInput input) : SV_Target
{
    return input.Color;
}