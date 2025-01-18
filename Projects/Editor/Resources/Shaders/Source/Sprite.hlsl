#pragma Shared
cbuffer SpriteData : register(b0)
{
    float4 PositionScale;
    float4 BaseColor;
    float4 Fill;
    float4x4 Projection;
}

Texture2D diffuseTex2D : register(t1);
SamplerState diffuseSampler : register(s1);

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

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    
    float2 position = PositionScale.xy + (input.Position.xy * PositionScale.zw);
    output.Position = float4(position, 0.0f, 1.0f);
    output.Position = mul(Projection, output.Position);
    output.Position.zw = float2(0.0f, 1.0f);
    output.TexCoord0 = input.TexCoord0;
    
    return output;
}

#pragma Fragment
struct FragmentInput
{
    float4 Position : SV_Position;
    float2 TexCoord0 : TEXCOORD0;
};

float4 main(FragmentInput input)
{
    float4 diffuse = diffuseTex2D.Sample(diffuseSampler, input.TexCoord0);
    float spriteMask = step(Fill.xy, input.TexCoord0);

    if (spriteMask)
        discard;
    
    return float4(diffuse.rgb * BaseColor.rgb, 1.0f);
}