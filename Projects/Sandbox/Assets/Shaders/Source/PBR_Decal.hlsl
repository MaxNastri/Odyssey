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
    float4 EmissiveColor;
    float alphaClip;
}

Texture2D diffuseTex2D : register(t6);
SamplerState diffuseSampler : register(s6);
Texture2D normalTex2D : register(t7);
SamplerState normalSampler : register(s7);
Texture2D noiseTex2D : register(t8);
SamplerState noiseSampler : register(s8);
Texture2D shadowmapTex2D : register(t9);
SamplerState shadowmapSampler : register(s9);
Texture2D depthTex2D : register(t10);
SamplerState depthSampler : register(s10);

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

// Forward declarations
LightingOutput CalculateLighting(float3 worldPosition, float3 worldNormal, float3 shadowCoord);
float3 CalculateDiffuse(Light light, float3 worldNormal);
float3 CalculateDirectionalLight(Light light, float3 worldNormal, float3 shadowCoord);
float3 CalculatePointLight(Light light, float3 worldPosition, float3 worldNormal);
float CalculateShadowFactor(float3 shadowCoord, float bias);
float FilterPCF(float3 shadowCoord, float bias);

float4 main(PixelInput input) : SV_Target
{
    float3 worldNormal = normalize(input.Normal);
    
    float4 albedo = diffuseTex2D.Sample(diffuseSampler, input.TexCoord0);
    
    if (albedo.a < alphaClip)
        discard;
    
    float3 texNormal = normalTex2D.Sample(normalSampler, input.TexCoord0);
    bool blankNormalMap = texNormal.x == 0.0f && texNormal.y == 0.0f && texNormal.z == 0.0f;
    
    const float3 normalColor = normalize(float3(0.5f, 0.5f, 1.0f));
    float4 emissiveColor = float4(0.0f);
    
    if (dot(texNormal, normalColor) < 0.95f)
        emissiveColor = EmissiveColor;
    
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
    
    float3 shadowCoord = input.ShadowCoord.xyz;
    shadowCoord.x = 0.5f + (shadowCoord.x * 0.5f);
    shadowCoord.y = 0.5f - (shadowCoord.y * 0.5f);
    
    LightingOutput lighting = CalculateLighting(input.WorldPosition, worldNormal, shadowCoord);
    
    float3 finalLighting = lighting.Diffuse + AmbientColor.rgb + (emissiveColor.rgb * emissiveColor.a);
    return float4(albedo.rgb, 1.0f) * float4(finalLighting, 1.0f);
}

LightingOutput CalculateLighting(float3 worldPosition, float3 worldNormal, float3 shadowCoord)
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
    float3 diffuseLight = light.Color.rgb * light.Intensity * contribution;
    return diffuseLight;
}

float3 CalculateDirectionalLight(Light light, float3 worldNormal, float3 shadowCoord)
{
    float3 lightVector = -normalize(light.Direction.xyz);
    
    float shadowFactor = FilterPCF(shadowCoord, 0.0f);
    shadowFactor = max(0.0f, shadowFactor * dot(worldNormal, lightVector));
    
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

float CalculateShadowFactor(float3 shadowCoord, float2 offset, float bias)
{
    float depth = shadowmapTex2D.Sample(shadowmapSampler, shadowCoord.xy + offset).r;
    return step(depth + bias, shadowCoord.z);
}

float FilterPCF(float3 shadowCoord, float bias)
{
    int2 texDimensions;
    shadowmapTex2D.GetDimensions(texDimensions.x, texDimensions.y);
    float scale = 1.5f;
    float dx = (1.0f / float(texDimensions.x));
    float dy = (1.0f / float(texDimensions.y));
    
    float shadowfactor = 0.0f;
    int count = 0;
    int range = 3;
    
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