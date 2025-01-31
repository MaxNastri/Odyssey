#pragma Shared
struct Light
{
    float4 Position;
    float4 Direction;
    float4 Color;
    uint Type;
    float Intensity;
    float Range;
};

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

cbuffer LightData : register(b2)
{
    float4 AmbientColor;
    Light SceneLights[16];
    uint LightCount;
    float Exposure;
    float GammaCorrection;
}

TextureCube skyboxTex3D : register(t3);
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

// From http://filmicworlds.com/blog/filmic-tonemapping-operators/
float3 Uncharted2Tonemap(float3 color)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;
    return ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
}

void ApplyTonemapping(inout float3 color)
{
    color = Uncharted2Tonemap(color * Exposure);
    color = color * (1.0f / Uncharted2Tonemap((11.2f).xxx));
}

float4 main(PixelInput input) : SV_Target
{
    float3 color = skyboxTex3D.SampleLevel(skyboxSampler, input.TexCoord0.xyz, 0);
    ApplyTonemapping(color);
    color = pow(color, (1.0 / GammaCorrection).xxx);
    return float4(color, 1.0);

}