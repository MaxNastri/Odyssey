#pragma Vertex
struct VertexInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float4 Color : COLOR;
    float2 TexCoord0 : TEXCOORD0;
    float4 BoneIndices : BLENDINDICES0;
    float4 BoneWeights : BLENDWEIGHT0;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 WorldPosition : POSITION1;
    float4 ShadowCoord : POSITION2;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float4 Color : COLOR0;
    float2 TexCoord0 : TEXCOORD0;
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
    float3x3 InverseModel;
}

cbuffer SkinningData : register(b2)
{
    float4x4 Bones[128];
}

struct SkinningOutput
{
    float4 Position;
    float4 Normal;
};

static const float4x4 ShadowBias = float4x4
(
	0.5, 0.0, 0.0, 0.5,
	0.0, 0.5, 0.0, 0.5,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0
);

// Forward declarations
SkinningOutput SkinVertex(VertexInput input);

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    
    SkinningOutput skinning = SkinVertex(input);
    float4 worldPosition = mul(Model, skinning.Position);
    float4 normal = float4(normalize(skinning.Normal.xyz), 0.0f);
    float4 tangent = float4(input.Tangent.xyz, 0.0f);
    
    output.Position = mul(ViewProjection, worldPosition);
    output.WorldPosition = worldPosition.xyz;
    output.ShadowCoord = mul(LightViewProj, worldPosition);
    output.ShadowCoord = mul(ShadowBias, output.ShadowCoord);
    output.ShadowCoord.xyz /= output.ShadowCoord.w;
    
    output.Normal = normalize(mul(Model, normal).xyz);
    output.Tangent = float4(mul(Model, tangent).xyz, input.Tangent.w);
    output.Color = input.Color;
    output.TexCoord0 = input.TexCoord0;
    return output;
}

SkinningOutput SkinVertex(VertexInput input)
{
    SkinningOutput output;
    output.Position = float4(0, 0, 0, 1);
    output.Normal = float4(0, 0, 0, 1);
    float4 vertexPosition = float4(input.Position, 1.0f);
    float4 vertexNormal = float4(input.Normal, 0.0f);
    
    for (int i = 0; i < 4; i++)
    {
        output.Position += mul(Bones[input.BoneIndices[i]], vertexPosition) * input.BoneWeights[i];
        output.Normal += mul(Bones[input.BoneIndices[i]], vertexNormal) * input.BoneWeights[i];
    }
    
    return output;
}

#pragma Fragment
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

struct PixelInput
{
    float4 Position : SV_Position;
    float3 WorldPosition : POSITION1;
    float4 ShadowCoord : POSITION2;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
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
Texture2D normalTex2D : register(t5);
SamplerState normalSampler : register(s5);
Texture2D shadowmapTex2D : register(t6);
SamplerState shadowmapSampler : register(s6);

// Forward declarations
LightingOutput CalculateLighting(float3 worldPosition, float3 worldNormal, float4 shadowCoord);
float3 CalculateDiffuse(Light light, float3 worldNormal);
float3 CalculateDirectionalLight(Light light, float3 worldNormal, float4 shadowCoord);
float3 CalculatePointLight(Light light, float3 worldPosition, float3 worldNormal);
float CalculateShadowFactor(float4 shadowCoord, float bias);
float FilterPCF(float4 shadowCoord, float bias);

float4 main(PixelInput input) : SV_Target
{
    float3 worldNormal = normalize(input.Normal);
    
    float4 albedo = diffuseTex2D.Sample(diffuseSampler, input.TexCoord0);
    
    // Alpha cutout for discarding completely transparent pixels
    // Todo: Make this an option in the material inspector
    if (albedo.a < 1.0f)
        discard;
    
    float3 texNormal = normalTex2D.Sample(normalSampler, input.TexCoord0);
    bool blankNormalMap = texNormal.x == 0.0f && texNormal.y == 0.0f && texNormal.z == 0.0f;
    
    if (!blankNormalMap)
    {
        // Move the texture normal from 0.0 - 1.0 space to -1.0 to 1.0
        texNormal = (2.0f * texNormal) - 1.0f;
        
        // "Orthogonalize" the tangent
        float3 tangent = normalize(input.Tangent.xyz - dot(input.Tangent.xyz, worldNormal) * worldNormal);
        
        // Calculate the binormal
        float3 binormal = input.Tangent.w * cross(worldNormal, tangent);
        
        // Create the tex-space matrix and generate the final surface normal
        float3x3 texSpace = float3x3(tangent, binormal, worldNormal);
        worldNormal = mul(texNormal.xyz, texSpace);
    }
    
    LightingOutput lighting = CalculateLighting(input.WorldPosition, worldNormal, input.ShadowCoord);
    
    float3 finalLighting = lighting.Diffuse + AmbientColor.rgb;
    return albedo * float4(finalLighting, 1.0f);
}

LightingOutput CalculateLighting(float3 worldPosition, float3 worldNormal, float4 shadowCoord)
{
    LightingOutput output;
    output.Diffuse = float4(0, 0, 0, 1);
    
    for (uint i = 0; i < LightCount; i++)
    {
        switch (SceneLights[i].Type)
        {
            case DIRECTIONAL_LIGHT:
                output.Diffuse += CalculateDirectionalLight(SceneLights[i], worldNormal, shadowCoord);
                break;
            case POINT_LIGHT:
                output.Diffuse += CalculatePointLight(SceneLights[i], worldPosition, worldNormal);
                break;
        }

    }
    return output;
}

float3 CalculateDiffuse(Light light, float3 lightVector, float3 worldNormal)
{
    float contribution = max(0.0f, dot(worldNormal, lightVector));
    
    // Half-lambert equation modified to be squared instead of cubed
    contribution = pow((contribution * 0.5f) + 0.5f, 3);
    return light.Color.rgb * light.Intensity * contribution;
}

float3 CalculateDirectionalLight(Light light, float3 worldNormal, float4 shadowCoord)
{
    float3 lightVector = -normalize(light.Direction.xyz);
    
    const float MINIMUM_SHADOW_BIAS = 0.00;
    float bias = max(MINIMUM_SHADOW_BIAS * (1.0 - dot(worldNormal, lightVector)), MINIMUM_SHADOW_BIAS);
    float shadowFactor = FilterPCF(shadowCoord, bias);
    
    return CalculateDiffuse(light, lightVector, worldNormal) * shadowFactor;
}

float3 CalculatePointLight(Light light, float3 worldPosition, float3 worldNormal)
{
    float3 lightVector = light.Position.xyz - worldPosition;
    
    // Manually normalize the light vector so
    // we can use the distance in our attenuation calculation
    float distance = length(lightVector);
    lightVector /= distance;
    
    // Squared exponential falloff
    float attenuation = pow(1.0f - saturate(distance / light.Range), 2);
    
    return CalculateDiffuse(light, lightVector, worldNormal) * attenuation;
}

float CalculateShadowFactor(float4 shadowCoord, float2 offset, float bias)
{
    float depth = shadowmapTex2D.Sample(shadowmapSampler, shadowCoord.xy + offset).r;
    return step(shadowCoord.z, depth + bias);
}

float FilterPCF(float4 shadowCoord, float bias)
{
    int2 texDimensions;
    shadowmapTex2D.GetDimensions(texDimensions.x, texDimensions.y);
    float scale = 1.5f;
    float dx = scale * (1.0f / float(texDimensions.x));
    float dy = scale * (1.0f / float(texDimensions.y));
    
    float shadowfactor = 0.0f;
    int count = 0;
    int range = 16;
    
    for (int x = -range; x <= range; x++)
    {
        for (int y = -range; y <= range; y++)
        {
            shadowfactor += CalculateShadowFactor(shadowCoord, float2(dx * x, dy * y), bias);
            count++;
        }
    }
    
    return shadowfactor / float(count);
}