#pragma Shared
cbuffer SceneData : register(b0)
{
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

#pragma Vertex
struct VertexInput
{
    float3 Position : POSITION;
    float4 BoneIndices : BLENDINDICES0;
    float4 BoneWeights : BLENDWEIGHT0;
};

struct VertexOutput
{
    float4 Position : SV_Position;
};

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
    output.Position = mul(Model, skinning.Position);
    output.Position = mul(ViewProjection, output.Position);
    
    return output;
}

SkinningOutput SkinVertex(VertexInput input)
{
    SkinningOutput output;
    output.Position = float4(0, 0, 0, 1);
    output.Normal = float4(0, 0, 0, 1);
    float4 vertexPosition = float4(input.Position, 1.0f);
    
    for (int i = 0; i < 4; i++)
    {
        output.Position += mul(Bones[input.BoneIndices[i]], vertexPosition) * input.BoneWeights[i];
    }
    
    return output;
}