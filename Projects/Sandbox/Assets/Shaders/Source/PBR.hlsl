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
    float3x3 InverseModel;
}

cbuffer SkinningData : register(b2)
{
    float4x4 Bones[128];
}

cbuffer GlobalData : register(b3)
{
    // Values used to linearize the Z buffer (http://www.humus.name/temp/Linearize%20depth.txt)
    // x = 1-far/near
    // y = far/near
    // z = x/far
    // w = y/far
    // or in case of a reversed depth buffer (UNITY_REVERSED_Z is 1)
    // x = -1+far/near
    // y = 1
    // z = x/far
    // w = 1/far
    float4 ZBufferParams;
	//x is 1.0 (or –1.0 if currently rendering with a flipped projection matrix), y is the camera’s near plane, z is the camera’s far plane and w is 1/FarPlane.
    float4 ProjectionParams;
    //x is the width of the camera’s target texture in pixels, y is the height of the camera’s target texture in pixels,
	// z is 1.0 + 1.0 / width and w is 1.0 + 1.0 / height.
    float4 ScreenParams;
    float4 Time;
}

// Bindings
cbuffer LightData : register(b4)
{
    float4 AmbientColor;
    Light SceneLights[16];
    uint LightCount;
}

cbuffer MaterialData : register(b5)
{
    // RGB = Emissive Color, A = Emissive Power
    float3 BaseColor;
    float Roughness;
    float4 EmissiveColor;
    float alphaClip;
}

Texture2D AlbedoMapTexture : register(t6);
SamplerState AlbedoMapSampler : register(s6);
Texture2D NormalMapTexture : register(t7);
SamplerState NormalMapSampler : register(s7);
Texture2D MetallicMapTexture : register(t8);
SamplerState MetallicMapSampler : register(s8);
Texture2D AOMapTexture : register(t9);
SamplerState AOMapSampler : register(s9);
Texture2D shadowmapTex2D : register(t10);
SamplerState shadowmapSampler : register(s10);

#pragma Vertex
struct VertexInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 TexCoord0 : TEXCOORD0;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 TexCoord0 : TEXCOORD0;
    float3 WorldPosition : POSITION1;
    float4 ShadowCoord : POSITION2;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    
    float4 worldPosition = mul(Model, float4(input.Position, 1.0f));
    float4 normal = float4(normalize(input.Normal.xyz), 0.0f);
    float4 tangent = float4(input.Tangent.xyz, 0.0f);
    
    output.Position = mul(ViewProjection, worldPosition);
    output.WorldPosition = worldPosition.xyz;
    output.ShadowCoord = mul(LightViewProj, worldPosition);
    output.ShadowCoord.xyz /= output.ShadowCoord.w;
    output.Normal = normalize(mul(Model, normal).xyz);
    output.Tangent = float4(mul(Model, tangent).xyz, input.Tangent.w);
    output.TexCoord0 = abs(input.TexCoord0);
    
    return output;
}

#pragma Fragment
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2
#define HALF_MIN_SQRT 0.0078125
static const float PI = 3.14159265359;
static const float Inverse_PI = 0.318309886f;
static const float Epsilon = 0.00001;

struct PixelInput
{
    float4 Position : SV_Position;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 TexCoord0 : TEXCOORD0;
    float3 WorldPosition : POSITION1;
    float4 ShadowCoord : POSITION2;
};

struct LightingOutput
{
    float3 Diffuse;
};

struct BRDFData
{
    float3 albedo;
    float3 diffuse;
    float3 specular;
    float reflectivity;
    float perceptualRoughness;
    float roughness;
    float roughness2;
    float grazingTerm;
    
    float normalizationTerm;
    float roughness2MinusOne;
};
float3 CalculateNormal(float3 vNormal, float4 vTangent, float2 uv)
{
    float3 tangentNormal = NormalMapTexture.Sample(NormalMapSampler, uv).xyz * 2.0 - 1.0;
    
    float3 T = normalize(vTangent.xyz - dot(vTangent.xyz, vNormal) * vNormal);
    float3 N = normalize(vNormal);
    float3 B = vTangent.w * normalize(cross(N, T));
    float3x3 TBN = float3x3(T, B, N);
    
    return normalize(mul(tangentNormal, TBN));
}

static float4 kDielectricSpec = float4(0.04, 0.04, 0.04, 1.0 - 0.04);

half OneMinusReflectivityMetallic(half metallic)
{
    // We'll need oneMinusReflectivity, so
    //   1-reflectivity = 1-lerp(dielectricSpec, 1, metallic) = lerp(1-dielectricSpec, 0, metallic)
    // store (1-dielectricSpec) in kDielectricSpec.a, then
    //   1-reflectivity = lerp(alpha, 0, metallic) = alpha + metallic*(0 - alpha) =
    //                  = alpha - metallic * alpha
    half oneMinusDielectricSpec = kDielectricSpec.a;
    return oneMinusDielectricSpec - metallic * oneMinusDielectricSpec;
}

float PerceptualSmoothnessToPerceptualRoughness(float perceptualSmoothness)
{
    return 1.0 - perceptualSmoothness;
}

float PerceptualRoughnessToRoughness(float perceptualRoughness)
{
    return perceptualRoughness * perceptualRoughness;
}

float DirectBRDFSpecular(BRDFData brdfData, float3 normalWS, float3 lightDirectionWS, float3 viewDirectionWS)
{
    float3 halfDir = normalize(lightDirectionWS + viewDirectionWS);
    
    float NoH = saturate(dot(normalWS, halfDir));
    float LoH = saturate(dot(lightDirectionWS, halfDir));
    
    float d = NoH * NoH * brdfData.roughness2MinusOne + 1.00001f;
    
    float LoH2 = LoH * LoH;
    float specularTerm = brdfData.roughness2 / ((d * d) * max(0.1, LoH2) * brdfData.normalizationTerm);
    
    return specularTerm;
}

static const float2 poissonDisk[] =
{
    float2(-0.94201624, -0.39906216),
  float2(0.94558609, -0.76890725),
  float2(-0.094184101, -0.92938870),
  float2(0.34495938, 0.29387760)
};
float CalculateShadowFactor(float3 shadowCoord, float2 offset, float bias)
{
    float depth = shadowmapTex2D.Sample(shadowmapSampler, shadowCoord.xy + offset).r;
    return step(depth + bias, shadowCoord.z);
}


float FilterPCF(float3 shadowCoord, float bias)
{
    int2 texDimensions;
    shadowmapTex2D.GetDimensions(texDimensions.x, texDimensions.y);
    float dx = (1.0f / float(texDimensions.x));
    float dy = (1.0f / float(texDimensions.y));
    
    float shadowfactor = 0.0f;
    int count = 0;
    float range = 0.5;
    
    for (float x = -range; x <= range; x+= 0.25f)
    {
        for (float y = -range; y <= range; y+= 0.25f)
        {
            shadowfactor += CalculateShadowFactor(shadowCoord, float2(dx * x, dy * y) + (poissonDisk[count % 4] / 10000.0f), bias);
            count++;
        }
    }
    
    return shadowfactor / float(count);
}

float3 LightingPBR(BRDFData brdfData, float3 lightColor, float3 lightDirectionWS, float attenuation, float3 normalWS, float3 viewDirectionWS, float3 shadowCoord)
{
    float NdotL = saturate(dot(normalWS, lightDirectionWS));
    float3 radiance = lightColor * (attenuation * NdotL);

    float3 brdf = brdfData.diffuse;
    
    // Calculate the specular factor
    brdf += brdfData.specular * DirectBRDFSpecular(brdfData, normalWS, lightDirectionWS, viewDirectionWS);
    
    // Multiply in the radiance
    brdf *= radiance;
    
    brdf += (brdfData.diffuse * AmbientColor.rgb * (1.0f - NdotL));
    
    return brdf;
}


float4 main(PixelInput input) : SV_Target
{
    float3 albedo = AlbedoMapTexture.Sample(AlbedoMapSampler, input.TexCoord0).rgb;
    float4 specGlossMap = MetallicMapTexture.Sample(MetallicMapSampler, input.TexCoord0);
    float3 worldNormal = CalculateNormal(input.Normal, input.Tangent, input.TexCoord0);
    float3 viewDirectionWS = normalize(ViewPos.xyz - input.WorldPosition);
    
    BRDFData brdfData;
    
    float metallic = specGlossMap.r;
    float smoothness = specGlossMap.a;
    float3 specular = float3(0.0, 0.0, 0.0);
    float occlusion = 1.0;
    float oneMinusReflectivity = OneMinusReflectivityMetallic(metallic);
    
    brdfData.albedo = albedo;
    brdfData.reflectivity =  1.0 - oneMinusReflectivity;
    brdfData.diffuse = albedo * oneMinusReflectivity;
    brdfData.specular = lerp(kDielectricSpec.rgb, albedo, metallic);
    
    brdfData.perceptualRoughness = PerceptualSmoothnessToPerceptualRoughness(smoothness);
    brdfData.roughness = max(PerceptualRoughnessToRoughness(brdfData.perceptualRoughness), HALF_MIN_SQRT);
    brdfData.roughness2 = max(brdfData.roughness * brdfData.roughness, HALF_MIN_SQRT);
    brdfData.grazingTerm = saturate(smoothness + brdfData.reflectivity);
    brdfData.normalizationTerm = brdfData.roughness * 4.0 + 2.0;
    brdfData.roughness2MinusOne = brdfData.roughness2 - 1.0;
    
    float3 shadowCoord = input.ShadowCoord.xyz;
    shadowCoord.x = 0.5f + (shadowCoord.x * 0.5f);
    shadowCoord.y = 0.5f - (shadowCoord.y * 0.5f);
    
    float3 directLighting = float3(0.0, 0.0, 0.0);
    for (int i = 0; i < LightCount; i++)
    {
        if (SceneLights[i].Type == DIRECTIONAL_LIGHT)
        {
            directLighting += LightingPBR(brdfData, SceneLights[i].Color.rgb, -normalize(SceneLights[i].Direction.xyz),
            1.0, worldNormal, viewDirectionWS, shadowCoord);
        }
    }

    return float4(directLighting, 1.0);
}
