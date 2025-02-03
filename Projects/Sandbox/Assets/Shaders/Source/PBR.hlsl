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
    float Exposure;
    float GammaCorrection;
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
Texture2D brdfLutTex2D : register(t11);
SamplerState brdfLutSampler : register(s11);
TextureCube IrradianceTex3D : register(t12);
SamplerState IrradianceSampler : register(s12);

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
#define PI 3.1415926535897932384626433832795
#define ALBEDO(uv) pow(AlbedoMapTexture.Sample(AlbedoMapSampler, uv).rgb, float3(2.2, 2.2, 2.2))
static const float3 Gamma_Correction_Exp = float3(0.45, 0.45, 0.45);

struct PixelInput
{
    float4 Position : SV_Position;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 TexCoord0 : TEXCOORD0;
    float3 WorldPosition : POSITION1;
    float4 ShadowCoord : POSITION2;
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
    color = Uncharted2Tonemap(color * Exposure);
    color = color * (1.0f / Uncharted2Tonemap((11.2f).xxx));
}

// Normal Distribution function --------------------------------------
float D_GGX(float NdotH, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = NdotH * NdotH * (alpha2 - 1.0) + 1.0;
    return (alpha2) / (PI * denom * denom);
}

// Geometric Shadowing function --------------------------------------
float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float GL = dotNL / (dotNL * (1.0 - k) + k);
    float GV = dotNV / (dotNV * (1.0 - k) + k);
    return GL * GV;
}

// Fresnel function ----------------------------------------------------
float3 F_Schlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
float3 F_SchlickR(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max((1.0 - roughness).xxx, F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 specularContribution(float2 inUV, float3 lightColor, float lightIntensity, float3 L, float3 V, float3 N, float3 F0, float metallic, float roughness)
{
	// Precalculate vectors and dot products
    float3 H = normalize(V + L);
    float NdotH = clamp(dot(N, H), 0.0, 1.0);
    float NdotV = clamp(dot(N, V), 0.0, 1.0);
    float NdotL = clamp(dot(N, L), 0.0, 1.0);

    float3 color = float3(0.0, 0.0, 0.0);

    if (NdotL > 0.0)
    {
		// D = Normal distribution (Distribution of the microfacets)
        float D = D_GGX(NdotH, roughness);
		// G = Geometric shadowing term (Microfacets shadowing)
        float G = G_SchlicksmithGGX(NdotL, NdotV, roughness);
		// F = Fresnel factor (Reflectance depending on angle of incidence)
        float3 F = F_Schlick(NdotV, F0);
        float3 spec = D * F * G / (4.0 * NdotL * NdotV + 0.001);
        float3 kD = (float3(1.0, 1.0, 1.0) - F) * (1.0 - metallic);
        float3 diffuse = (kD * ALBEDO(inUV) / PI + spec);
        color += (diffuse) * NdotL * lightColor * lightIntensity;
    }

    return color;
}

float4 main(PixelInput input) : SV_TARGET
{
    float3 N = CalculateNormal(input.Normal, input.Tangent, input.TexCoord0);
    float3 V = normalize(ViewPos.xyz - input.WorldPosition);
    float3 R = reflect(-V, N);

    float4 metallicGloss = MetallicMapTexture.Sample(MetallicMapSampler, input.TexCoord0);
    float metallic = metallicGloss.r;
    float roughness = 1.0 - metallicGloss.a;
    
    float2 NdotV = (max(dot(N, V), 0.0), roughness);
    
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), ALBEDO(input.TexCoord0), metallic);

    float3 Lo = float3(0.0, 0.0, 0.0);
    
    for (int i = 0; i < LightCount; i++)
    {
        if (SceneLights[i].Type == DIRECTIONAL_LIGHT)
        {
            float3 L = -normalize(SceneLights[i].Direction);
            Lo += specularContribution(input.TexCoord0, SceneLights[i].Color.rgb, SceneLights[i].Intensity * PI, L, V, N, F0, metallic, roughness);
        }
    }
    
    float2 brdf = brdfLutTex2D.SampleLevel(brdfLutSampler, NdotV, 0);
    float3 irradiance = IrradianceTex3D.Sample(IrradianceSampler, N).rgb;
    float ao = AOMapTexture.Sample(AOMapSampler, input.TexCoord0).a;
    
    float3 diffuse = irradiance * ALBEDO(input.TexCoord0);
    float3 F = F_SchlickR(max(dot(N, V), 0.0), F0, roughness);
    
    float3 specular = float3(1, 1, 1) * (F * brdf.x + brdf.y);
    
    // Ambient
    float3 kD = 1.0 - F;
    kD *= 1.0 - metallic;
    float3 ambient = (kD * diffuse) * ao;
    
    // TODO: Ambient lighting
    //return float4(ambient, 1.0);
    float3 color = ambient + Lo;

	// Tone mapping
    ApplyTonemapping(color);
    
	// Gamma correction
    color = pow(color, Gamma_Correction_Exp);
    
    return float4(color, 1.0);
}