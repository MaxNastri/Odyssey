#pragma Shared
#define PI 3.1415926535897932384626433832795

cbuffer IrradianceData : register(b0)
{
    float4x4 MVP;
    float DeltaPhi;
    float DeltaTheta;
}

TextureCube skyboxTex3D : register(t1);
SamplerState skyboxSampler : register(s1);

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
    output.Position = mul(MVP, position);
    output.TexCoord0 = input.Position.xyz;
    return output;
}

#pragma Fragment
struct FragInput
{
    float4 Position : SV_Position;
    float3 TexCoord0 : TEXCOORD0;
};

// From http://filmicgames.com/archives/75
float3 Uncharted2Tonemap(float3 x)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

void ApplyTonemapping(inout float3 color)
{
    color = Uncharted2Tonemap(color * 1.1);
    color = color * (1.0f / Uncharted2Tonemap((11.2f).xxx));
}

float4 main(FragInput input) : SV_Target
{
    float3 N = normalize(input.TexCoord0);
    float3 up = float3(0.0, 1.0, 0.0);
    float3 right = normalize(cross(up, N));
    up = cross(N, right);
    
    const float two_pi = PI * 2.0;
    const float half_pi = PI * 0.5;
    
    float3 color = float3(0.0, 0.0, 0.0);
    
    uint sampleCount = 0;
    
    for (float phi = 0.0; phi < two_pi; phi += DeltaPhi)
    {
        for (float theta = 0.0; theta < half_pi; theta += DeltaTheta)
        {
            float3 vec = cos(phi) * right + sin(phi) * up;
            float3 sampleVec = cos(theta) * N + sin(theta) * vec;
            float3 envColor = skyboxTex3D.Sample(skyboxSampler, sampleVec).rgb * cos(theta) * sin(theta);
            color += envColor;
            sampleCount++;

        }
    }
    
    float3 finalColor = PI * color / float(sampleCount);
    ApplyTonemapping(finalColor);
    return float4(finalColor, 1.0);

}