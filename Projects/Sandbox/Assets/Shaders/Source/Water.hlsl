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

struct PnPatch
{
    float b210;
    float b120;
    float b021;
    float b012;
    float b102;
    float b201;
    float b111;
    float n110;
    float n011;
    float n101;
};

struct ConstantsHSOutput
{
    float TessLevelOuter[3] : SV_TessFactor;
    float TessLevelInner : SV_InsideTessFactor;
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

cbuffer LightData : register(b4)
{
    float4 AmbientColor;
    Light SceneLights[16];
    uint LightCount;
}

cbuffer MaterialData : register(b5)
{
    int TesselationLevel;
    float TessAlpha;
    float Tiling;
    float WaterDepth;
    float3 SurfaceColor;
    float3 DeepColor;
    float RefractionSpeed;
    float2 RefractionScale;
    float RefractionStrength;
}

Texture2D normalTex2D : register(t7);
SamplerState normalSampler : register(s7);
Texture2D noiseTex2D : register(t8);
SamplerState noiseSampler : register(s8);
Texture2D depthTex2D : register(t10);
SamplerState depthSampler : register(s10);
Texture2D refractionNormalTex2D : register(t11);
SamplerState refractionNormalSampler : register(s11);
Texture2D cameraColorTex2D : register(t9);
SamplerState cameraColorSampler : register(s9);

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
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.Position = float4(input.Position, 1.0f);
    output.Normal = normalize(input.Normal);
    output.Tangent.xyz = normalize(input.Tangent.xyz);
    output.TexCoord0 = input.TexCoord0;
    return output;
}

#pragma Hull
struct HullInput
{
    float4 Position : SV_Position;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 TexCoord0 : TEXCOORD0;
};

struct HullOutput
{
    float4 Position : SV_Position;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 TexCoord0 : TEXCOORD0;
    float pnPatch[10] : TEXCOORD6;
};

void SetPnPatch(out float output[10], PnPatch patch)
{
    output[0] = patch.b210;
    output[1] = patch.b120;
    output[2] = patch.b021;
    output[3] = patch.b012;
    output[4] = patch.b102;
    output[5] = patch.b201;
    output[6] = patch.b111;
    output[7] = patch.n110;
    output[8] = patch.n011;
    output[9] = patch.n101;
}

float wij(float4 iPos, float3 iNormal, float4 jPos)
{
    return dot(jPos.xyz - iPos.xyz, iNormal);
}

float vij(float4 iPos, float3 iNormal, float4 jPos, float3 jNormal)
{
    float3 Pj_minus_Pi = jPos.xyz
					- iPos.xyz;
    float3 Ni_plus_Nj = iNormal + jNormal;
    return 2.0 * dot(Pj_minus_Pi, Ni_plus_Nj) / dot(Pj_minus_Pi, Pj_minus_Pi);
}

ConstantsHSOutput ConstantsHS(InputPatch<HullInput, 3> patch, uint InvocationID : SV_PrimitiveID)
{
    ConstantsHSOutput output = (ConstantsHSOutput) 0;
    output.TessLevelOuter[0] = TesselationLevel;
    output.TessLevelOuter[1] = TesselationLevel;
    output.TessLevelOuter[2] = TesselationLevel;
    output.TessLevelInner = TesselationLevel;
    return output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantsHS")]
[maxtessfactor(20.0f)]
HullOutput main(InputPatch<HullInput, 3> patch, uint InvocationID : SV_OutputControlPointID)
{
    HullOutput output = (HullOutput) 0;
    
	// get data
    output.Position = patch[InvocationID].Position;
    output.Normal = patch[InvocationID].Normal;
    output.Tangent = patch[InvocationID].Tangent;
    output.TexCoord0 = patch[InvocationID].TexCoord0;

	// set base
    float P0 = patch[0].Position[InvocationID];
    float P1 = patch[1].Position[InvocationID];
    float P2 = patch[2].Position[InvocationID];
    float N0 = patch[0].Normal[InvocationID];
    float N1 = patch[1].Normal[InvocationID];
    float N2 = patch[2].Normal[InvocationID];

	// compute control points
    PnPatch pnPatch;
    pnPatch.b210 = (2.0 * P0 + P1 - wij(patch[0].Position, patch[0].Normal, patch[1].Position) * N0) / 3.0;
    pnPatch.b120 = (2.0 * P1 + P0 - wij(patch[1].Position, patch[1].Normal, patch[0].Position) * N1) / 3.0;
    pnPatch.b021 = (2.0 * P1 + P2 - wij(patch[1].Position, patch[1].Normal, patch[2].Position) * N1) / 3.0;
    pnPatch.b012 = (2.0 * P2 + P1 - wij(patch[2].Position, patch[2].Normal, patch[1].Position) * N2) / 3.0;
    pnPatch.b102 = (2.0 * P2 + P0 - wij(patch[2].Position, patch[2].Normal, patch[0].Position) * N2) / 3.0;
    pnPatch.b201 = (2.0 * P0 + P2 - wij(patch[0].Position, patch[0].Normal, patch[2].Position) * N0) / 3.0;
    float E = (pnPatch.b210
			+ pnPatch.b120
			+ pnPatch.b021
			+ pnPatch.b012
			+ pnPatch.b102
			+ pnPatch.b201) / 6.0;
    float V = (P0 + P1 + P2) / 3.0;
    pnPatch.b111 = E + (E - V) * 0.5;
    pnPatch.n110 = N0 + N1 - vij(patch[0].Position, patch[0].Normal, patch[1].Position, patch[1].Normal) * (P1 - P0);
    pnPatch.n011 = N1 + N2 - vij(patch[1].Position, patch[1].Normal, patch[2].Position, patch[2].Normal) * (P2 - P1);
    pnPatch.n101 = N2 + N0 - vij(patch[2].Position, patch[2].Normal, patch[0].Position, patch[0].Normal) * (P0 - P2);
    SetPnPatch(output.pnPatch, pnPatch);

    return output;
}

#pragma Domain
struct DomainInput
{
    float4 Position : SV_Position;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 TexCoord0 : TEXCOORD0;
    float pnPatch[10] : TEXCOORD6;
};

struct DSOutput
{
    float4 Position : SV_Position;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 TexCoord0 : TEXCOORD0;
    float4 ScreenPos : POSITION1;
    float3 WorldPosition : POSITION2;
};

#define uvw TessCoord

PnPatch GetPnPatch(float pnPatch[10])
{
    PnPatch output;
    output.b210 = pnPatch[0];
    output.b120 = pnPatch[1];
    output.b021 = pnPatch[2];
    output.b012 = pnPatch[3];
    output.b102 = pnPatch[4];
    output.b201 = pnPatch[5];
    output.b111 = pnPatch[6];
    output.n110 = pnPatch[7];
    output.n011 = pnPatch[8];
    output.n101 = pnPatch[9];
    return output;
}

inline float4 ComputeScreenPos(float4 pos)
{
    float4 o = pos * 0.5f;
    o.xy = float2(o.x, o.y * ProjectionParams.x) + o.w;
    o.zw = pos.zw;
    return o;
}

[domain("tri")]
DSOutput main(ConstantsHSOutput input, float3 TessCoord : SV_DomainLocation, const OutputPatch<DomainInput, 3> patch)
{
    PnPatch pnPatch[3];
    pnPatch[0] = GetPnPatch(patch[0].pnPatch);
    pnPatch[1] = GetPnPatch(patch[1].pnPatch);
    pnPatch[2] = GetPnPatch(patch[2].pnPatch);

    DSOutput output = (DSOutput) 0;
    float3 uvwSquared = uvw * uvw;
    float3 uvwCubed = uvwSquared * uvw;

    // extract control points
    float3 b210 = float3(pnPatch[0].b210, pnPatch[1].b210, pnPatch[2].b210);
    float3 b120 = float3(pnPatch[0].b120, pnPatch[1].b120, pnPatch[2].b120);
    float3 b021 = float3(pnPatch[0].b021, pnPatch[1].b021, pnPatch[2].b021);
    float3 b012 = float3(pnPatch[0].b012, pnPatch[1].b012, pnPatch[2].b012);
    float3 b102 = float3(pnPatch[0].b102, pnPatch[1].b102, pnPatch[2].b102);
    float3 b201 = float3(pnPatch[0].b201, pnPatch[1].b201, pnPatch[2].b201);
    float3 b111 = float3(pnPatch[0].b111, pnPatch[1].b111, pnPatch[2].b111);

    // extract control normals
    float3 n110 = normalize(float3(pnPatch[0].n110, pnPatch[1].n110, pnPatch[2].n110));
    float3 n011 = normalize(float3(pnPatch[0].n011, pnPatch[1].n011, pnPatch[2].n011));
    float3 n101 = normalize(float3(pnPatch[0].n101, pnPatch[1].n101, pnPatch[2].n101));

    // compute texcoords
    float2 texCoord0 = TessCoord[2] * patch[0].TexCoord0 + TessCoord[0] * patch[1].TexCoord0 + TessCoord[1] * patch[2].TexCoord0;

    // normal
    // Barycentric normal
    float3 barNormal = TessCoord[2] * patch[0].Normal + TessCoord[0] * patch[1].Normal + TessCoord[1] * patch[2].Normal;
    float3 barTangent = TessCoord[2] * patch[0].Tangent.xyz + TessCoord[0] * patch[1].Tangent.xyz + TessCoord[1] * patch[2].Tangent.xyz;
    
    float3 pnNormal = patch[0].Normal * uvwSquared[2] + patch[1].Normal * uvwSquared[0] + patch[2].Normal * uvwSquared[1]
                   + n110 * uvw[2] * uvw[0] + n011 * uvw[0] * uvw[1] + n101 * uvw[2] * uvw[1];
    float3 pnTangent = patch[0].Tangent.xyz * uvwSquared[2] + patch[1].Tangent.xyz * uvwSquared[0] + patch[2].Tangent.xyz * uvwSquared[1]
                   + n110 * uvw[2] * uvw[0] + n011 * uvw[0] * uvw[1] + n101 * uvw[2] * uvw[1];
    
    float3 normal0 = normalize(TessAlpha * pnNormal + (1.0 - TessAlpha) * barNormal);
    float4 tangent0 = float4(normalize(pnTangent), patch[0].Tangent.w);
    
    // compute interpolated pos
    float3 barPos = TessCoord[2] * patch[0].Position.xyz
                + TessCoord[0] * patch[1].Position.xyz
                + TessCoord[1] * patch[2].Position.xyz;

    // save some computations
    uvwSquared *= 3.0;

    // compute PN position
    float3 pnPos = patch[0].Position.xyz * uvwCubed[2]
                + patch[1].Position.xyz * uvwCubed[0]
                + patch[2].Position.xyz * uvwCubed[1]
                + b210 * uvwSquared[2] * uvw[0]
                + b120 * uvwSquared[0] * uvw[2]
                + b201 * uvwSquared[2] * uvw[1]
                + b021 * uvwSquared[0] * uvw[1]
                + b102 * uvwSquared[1] * uvw[2]
                + b012 * uvwSquared[1] * uvw[0]
                + b111 * 6.0 * uvw[0] * uvw[1] * uvw[2];

    
    // final position and normal
    float3 finalPos = (1.0 - TessAlpha) * barPos + TessAlpha * pnPos;
    
    //float2 noiseUV = tc0 + (Time.xy * NoiseFrequency);
    //float3 noiseValue = noiseTex2D.SampleLevel(noiseSampler, noiseUV, 0).rgb;
    //float3 posOffset = normal0 * noiseValue * NoiseScale;
    //
    //finalPos += posOffset;
    
    output.WorldPosition = mul(Model, float4(finalPos, 1.0f));
    output.Position = mul(ViewProjection, float4(output.WorldPosition, 1.0f));
    output.Normal = mul(Model, float4(normal0, 0));
    output.Tangent = float4(mul(Model, tangent0).xyz, tangent0.w);
    output.TexCoord0 = texCoord0 * Tiling;
    output.ScreenPos = ComputeScreenPos(output.Position);
    
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
    float4 ScreenPos : POSITION1;
    float3 WorldPosition : POSITION2;
};

struct LightingOutput
{
    float3 Diffuse;
};

// Forward declarations
inline float LinearEyeDepth(float depth)
{
    return 1.0 / (ZBufferParams.x * depth + ZBufferParams.y);
}

LightingOutput CalculateLighting(float3 worldPosition, float3 worldNormal);
float3 CalculateDiffuse(Light light, float3 worldNormal);
float3 CalculateDirectionalLight(Light light, float3 worldNormal);
float3 CalculatePointLight(Light light, float3 worldPosition, float3 worldNormal);

float2 TextureMovement(float2 uv, float2 scale, float speed)
{
    float timeSpeed = Time.y * speed;
    uv = (uv * scale) + float2(timeSpeed, timeSpeed);
    return uv;
}

float3 BlendNormal(float3 A, float3 B)
{
    return normalize(float3(A.rg + B.rg, A.b * B.b));
}

float4 main(PixelInput input) : SV_Target
{
    // Calculate the screen uv coordinates
    float2 screenUV = input.ScreenPos.xy / input.ScreenPos.w;
    
    // Sample from the depth texture
    float depthSample = depthTex2D.Sample(depthSampler, screenUV);
    
    // Linearize the depth sample
    float sceneZ = 1.0f - (LinearEyeDepth(depthSample) / ProjectionParams.z);
    
    // Linearize the input depth
    float eyeZ = 1.0f - (LinearEyeDepth(input.Position.z) / ProjectionParams.z);
    
    // Calculate the distance between the eye depth and sample depth, scaled by the water's depth
    float fade = saturate((eyeZ - sceneZ) * WaterDepth);
    
    // Lerp between the surface and deep color
    float3 FadeColor = lerp(SurfaceColor, DeepColor, fade);
    
    
    // Refraction start
    float2 refactionUV = TextureMovement(input.TexCoord0, RefractionScale, RefractionSpeed);
    float2 inverseRefactionUV = TextureMovement(input.TexCoord0, RefractionScale, -RefractionSpeed);
    
    // Sample the normal map twice, once with forward movement and once with reverse movement
    float3 refractionNormal = refractionNormalTex2D.Sample(refractionNormalSampler, refactionUV);
    float3 inverseRefractionNormal = refractionNormalTex2D.Sample(refractionNormalSampler, inverseRefactionUV);
    
    // Blend the normals together and apply the refraction strength
    float3 blendRefractionNormal = BlendNormal(refractionNormal, inverseRefractionNormal) * RefractionStrength * 0.1f;
    
    // Apply a linear fall-off based on how close the vertex is to the edge of the screen
    float2 refractionPos = lerp(float2(0, 0), blendRefractionNormal.xy, float2(1, 1) - screenUV) + screenUV;
    
    // Sample the camera color texture and distort it based on the refraction position
    float3 refractionColor = cameraColorTex2D.SampleLevel(cameraColorSampler, refractionPos, 0);
    
    return float4(lerp(FadeColor, refractionColor, 0.75f), 1.0f);

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