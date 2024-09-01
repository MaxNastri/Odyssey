#pragma Vertex
struct VertexInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float4 Color : COLOR;
    float2 TexCoord0 : TEXCOORD0;
    float2 TexCoord1 : TEXCOORD1;
    float2 TexCoord2 : TEXCOORD2;
    float2 TexCoord3 : TEXCOORD3;
    float2 TexCoord4 : TEXCOORD4;
    float2 TexCoord5 : TEXCOORD5;
    float2 TexCoord6 : TEXCOORD6;
    float2 TexCoord7 : TEXCOORD7;
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
    float2 TexCoord1 : TEXCOORD1;
    float2 TexCoord2 : TEXCOORD2;
    float2 TexCoord3 : TEXCOORD3;
    float2 TexCoord4 : TEXCOORD4;
    float2 TexCoord5 : TEXCOORD5;
    float2 TexCoord6 : TEXCOORD6;
    float2 TexCoord7 : TEXCOORD7;
    float4 BoneIndices : BLENDINDICES0;
    float4 BoneWeights : BLENDWEIGHT0;
};

cbuffer SceneData : register(b0)
{
    float4x4 View;
    float4x4 ViewProjection;
}

cbuffer ModelData : register(b1)
{
    float4x4 Model;
}

cbuffer SkinningData : register(b2)
{
    float4x4 Bones[128];
}

// Forward declarations
float4 SkinVertex(VertexInput input);

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    
    float4 position = SkinVertex(input);
    //float4 position = float4(input.Position, 1.0f);
    output.Position = mul(Model, position);
    output.Position = mul(ViewProjection, output.Position);
    output.Normal = input.Normal;
    output.Tangent = input.Tangent;
    output.Color = input.Color;
    output.TexCoord0 = input.TexCoord0;
    output.TexCoord1 = input.TexCoord1;
    output.TexCoord2 = input.TexCoord2;
    output.TexCoord3 = input.TexCoord3;
    output.TexCoord4 = input.TexCoord4;
    output.TexCoord5 = input.TexCoord5;
    output.TexCoord6 = input.TexCoord6;
    output.TexCoord7 = input.TexCoord7;
    output.BoneIndices = input.BoneIndices;
    output.BoneWeights = input.BoneWeights;
    return output;
}

float4 SkinVertex(VertexInput input)
{
    float4 position = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 vertexPosition = float4(input.Position, 1.0f);
    
    for (int i = 0; i < 4; i++)
    {
        position += mul(Bones[input.BoneIndices[0]], vertexPosition) * input.BoneWeights[i];
    }
    
    return position;
}

#pragma Fragment
struct PixelInput
{
    float4 Position : SV_Position;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float4 Color : COLOR0;
    float2 TexCoord0 : TEXCOORD0;
    float2 TexCoord1 : TEXCOORD1;
    float2 TexCoord2 : TEXCOORD2;
    float2 TexCoord3 : TEXCOORD3;
    float2 TexCoord4 : TEXCOORD4;
    float2 TexCoord5 : TEXCOORD5;
    float2 TexCoord6 : TEXCOORD6;
    float2 TexCoord7 : TEXCOORD7;
    float4 BoneIndices : BLENDINDICES0;
    float4 BoneWeights : BLENDWEIGHT0;
};

Texture2D diffuseTex2D : register(t3);
SamplerState diffuseSampler : register(s3);

float4 main(PixelInput input) : SV_Target
{
    return diffuseTex2D.Sample(diffuseSampler, input.TexCoord0);
}