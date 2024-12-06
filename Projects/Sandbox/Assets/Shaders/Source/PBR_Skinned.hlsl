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
    float3 WorldPosition : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float4 Color : COLOR0;
    float2 TexCoord0 : TEXCOORD0;
    float4 ViewPosition : POSITION1;
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

struct SkinningOutput
{
    float4 Position;
    float4 Normal;
};

// Forward declarations
SkinningOutput SkinVertex(VertexInput input);

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    
    SkinningOutput skinning = SkinVertex(input);
    float4 worldPosition = mul(Model, skinning.Position);
    float4 normal = float4(normalize(skinning.Normal.xyz), 0.0f);
    
    output.Position = mul(ViewProjection, worldPosition);
    output.WorldPosition = worldPosition.xyz;
    output.Normal = normalize(mul(Model, normal).xyz);
    output.Tangent = input.Tangent;
    output.Color = input.Color;
    output.TexCoord0 = input.TexCoord0;
    output.ViewPosition = ViewPos - worldPosition;
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
    float3 WorldPosition : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float4 Color : COLOR0;
    float2 TexCoord0 : TEXCOORD0;
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
LightingOutput CalculateLighting(float3 worldPosition, float3 worldNormal);
float3 CalculateDiffuse(Light light, float3 worldNormal);
float3 CalculateDirectionalLight(Light light, float3 worldNormal);
float3 CalculatePointLight(Light light, float3 worldPosition, float3 worldNormal);

float4 main(PixelInput input) : SV_Target
{
    input.Normal = normalize(input.Normal);
    
    float4 albedo = diffuseTex2D.Sample(diffuseSampler, input.TexCoord0);
    
    // Alpha cutout for discarding completely transparent pixels
    // Todo: Make this an option in the material inspector
    if (albedo.a < 1.0f)
        discard;
    
    LightingOutput lighting = CalculateLighting(input.WorldPosition, input.Normal);
    float4 finalLighting = float4(lighting.Diffuse * AmbientColor.rgb, 1.0f);
    return albedo * finalLighting;
}

LightingOutput CalculateLighting(float3 worldPosition, float3 worldNormal)
{
    LightingOutput output;
    output.Diffuse = float4(0, 0, 0, 1);
    
    for (uint i = 0; i < LightCount; i++)
    {
        switch (SceneLights[i].Type)
        {
            case DIRECTIONAL_LIGHT:
                output.Diffuse += CalculateDirectionalLight(SceneLights[i], worldNormal);
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

float3 CalculateDirectionalLight(Light light, float3 worldNormal)
{
    float3 lightVector = -normalize(light.Direction.xyz);
    return CalculateDiffuse(light, lightVector, worldNormal);
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