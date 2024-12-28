#pragma Vertex

struct VertexInput
{
    float3 Position : POSITION;
    float2 TexCoord0 : TEXCOORD0;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 TexCoord0 : TEXCOORD0;
};

cbuffer SceneData : register(b0)
{
    float4 ViewPos;
    float4x4 View;
    float4x4 Projection;
    float4x4 ViewProjection;
    float4x4 LightViewProj;
}

cbuffer SpriteData : register(b1)
{
    float2 WorldPosition;
    float2 Scale;
}

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    
    float2 position = WorldPosition + (input.Position.xy * Scale);
    output.Position = float4(position, 0.0f, 1.0f);
    output.Position = mul(Projection, output.Position);
    output.Position.zw = float2(0.0f, 1.0f);
    output.TexCoord0 = input.TexCoord0;
    
    return output;
}

#pragma Fragment
Texture2D diffuseTex2D : register(t2);
SamplerState diffuseSampler : register(s2);

struct FragmentInput
{
    float4 Position : SV_Position;
    float2 TexCoord0 : TEXCOORD0;
};

float4 main(FragmentInput input)
{
    float4 diffuse = diffuseTex2D.Sample(diffuseSampler, input.TexCoord0);
    
    if (diffuse.a < 1.0f)
        discard;
    
    return diffuse;
}