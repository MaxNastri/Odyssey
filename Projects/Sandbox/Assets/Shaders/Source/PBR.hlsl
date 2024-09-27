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
    output.Position = mul(ViewProjection, worldPosition);
    output.Normal = skinning.Normal.xyz;
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
    float4 vertexNormal = float4(input.Normal, 1.0f);
    
    for (int i = 0; i < 4; i++)
    {
        output.Position += mul(Bones[input.BoneIndices[i]], vertexPosition) * input.BoneWeights[i];
        output.Normal += mul(Bones[input.BoneIndices[i]], vertexNormal) * input.BoneWeights[i];
    }
    
    output.Normal = normalize(output.Normal);
    return output;
}

#pragma Fragment
#define DIRECTIONAL_LIGHT 1
#define POINT_LIGHT 2
#define SPOT_LIGHT 3

struct PixelInput
{
    float4 Position : SV_Position;
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

cbuffer LightData : register(b3)
{
    Light SceneLights[16];
    uint LightCount;
}

Texture2D diffuseTex2D : register(t4);
SamplerState diffuseSampler : register(s4);

LightingOutput CalculateLighting(float3 surfaceNormal);
float3 CalculateDiffuse(Light light, float3 surfaceNormal);

float4 main(PixelInput input) : SV_Target
{
    LightingOutput lighting = CalculateLighting(input.Normal);
    
    float3 ambient = (0.2f, 0.2f, 0.2f);
    float4 finalLighting = float4(lighting.Diffuse + ambient, 1.0f);
    
    return diffuseTex2D.Sample(diffuseSampler, input.TexCoord0) * finalLighting;
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
    return light.Color.rgb * light.Intensity * contribution;
}