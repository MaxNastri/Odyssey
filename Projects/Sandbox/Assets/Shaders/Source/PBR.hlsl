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
    float4 ViewPosition : POSITION1;
    float4 BoneIndices : BLENDINDICES0;
    float4 BoneWeights : BLENDWEIGHT0;
};

cbuffer SceneData : register(b0)
{
    float4x4 View;
    float4x4 ViewProjection;
    float4 ViewPos;
}

cbuffer ModelData : register(b1)
{
    float4x4 Model;
    float3x3 InverseModel;
}

cbuffer SkinningData : register(b2)
{
    float4x4 Bones[128];
}

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    
    float4 worldPosition = mul(Model, float4(input.Position, 1.0f));
    float4 normal = float4(normalize(input.Normal.xyz), 0.0f);
    
    output.Position = mul(ViewProjection, worldPosition);
    output.Normal = normalize(mul(Model, normal).xyz);
    output.Tangent = input.Tangent;
    output.Color = input.Color;
    output.TexCoord0 = input.TexCoord0;
    output.ViewPosition = ViewPos - worldPosition;
    output.BoneWeights = input.BoneWeights;
    output.BoneIndices = input.BoneIndices;
    
    return output;
}

#pragma Fragment
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

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

struct Light
{
    float4 Position;
    float4 Direction;
    float4 Color;
    uint Type;
    float Intensity;
    float Range;
};

struct LightingOutput
{
    float3 Diffuse;
};

// Bindings
cbuffer LightData : register(b3)
{
    float4 AmbientColor;
    Light SceneLights[16];
    uint LightCount;
}

Texture2D diffuseTex2D : register(t4);
SamplerState diffuseSampler : register(s4);

// Forward declarations
LightingOutput CalculateLighting(float3 surfaceNormal);
float3 CalculateDiffuse(Light light, float3 surfaceNormal);

float4 main(PixelInput input) : SV_Target
{
    input.Normal = normalize(input.Normal);
    
    float4 albedo = diffuseTex2D.Sample(diffuseSampler, input.TexCoord0);
    
    // Alpha cutout for discarding completely transparent pixels
    // Todo: Make this an option in the material inspector
    if (albedo.a < 1.0f)
        discard;
    
    LightingOutput lighting = CalculateLighting(input.Normal);
    float4 finalLighting = float4(lighting.Diffuse + AmbientColor.rgb, 1.0f);
    return albedo * finalLighting;
}

LightingOutput CalculateLighting(float3 surfaceNormal)
{
    LightingOutput output;
    output.Diffuse = float4(0, 0, 0, 1);
    
    for (uint i = 0; i < LightCount; i++)
    {
        switch (SceneLights[i].Type)
        {
            case DIRECTIONAL_LIGHT:
                output.Diffuse += CalculateDiffuse(SceneLights[i], surfaceNormal);
                break;
            case POINT_LIGHT:
                //output.Diffuse += CalculateDiffuse(SceneLights[i], surfaceNormal);
                break;
        }

    }
    return output;
}

float3 CalculateDiffuse(Light light, float3 surfaceNormal)
{
    float3 lightVector = -normalize(light.Direction.xyz);
    float contribution = max(0.0f, dot(surfaceNormal, lightVector));
    // Half-lambert equation modified to be cubed instead of squared
    contribution = pow((contribution * 0.5f) + 0.5f, 3);
    return light.Color.rgb * light.Intensity * contribution;
}