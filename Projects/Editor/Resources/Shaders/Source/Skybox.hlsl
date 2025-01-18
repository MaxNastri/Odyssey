#pragma Shared
cbuffer SceneData : register(b0)
{
    float4 ViewPos;
    float4x4 View;
    float4x4 Projection;
    float4x4 ViewProjection;
    float4x4 LightViewProj;
}

cbuffer ModelData : register(b1)
{
    float4x4 Model;
}

TextureCube skyboxTex2D : register(t3);
SamplerState skyboxSampler : register(s3);

#pragma Vertex
struct VertexInput
{
    float3 Position : POSITION;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 TexCoord0 : TEXCOORD0;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    
    float4 position = float4(input.Position, 1.0f);
    output.Position = mul(Model, position);
    output.Position = mul(ViewProjection, output.Position);
    output.TexCoord0 = input.Position.xyz;
    return output;
}

#pragma Fragment
struct PixelInput
{
    float4 Position : SV_Position;
    float3 TexCoord0 : TEXCOORD0;
};

float4 main(PixelInput input) : SV_Target
{
    return skyboxTex2D.Sample(skyboxSampler, input.TexCoord0.xyz);
}