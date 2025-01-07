#pragma Compute
struct Particle
{
    float4 Position;
    float4 Color;
    float4 Velocity;
    float2 Lifetime;
    float Size;
    float Speed;
};

#define CIRCLE 0
#define CONE 1

static const uint Add = 1;
static const uint Subtract = -1;

static const uint DEAD_COUNT_OFFSET = 0;
static const uint ALIVE_PRE_SIM_COUNT_OFFSET = DEAD_COUNT_OFFSET + 4;
static const uint ALIVE_POST_SIM_COUNT_OFFSET = ALIVE_PRE_SIM_COUNT_OFFSET + 4;

RWStructuredBuffer<Particle> ParticleBuffer : register(b2);
RWByteAddressBuffer CounterBuffer : register(b3);
RWStructuredBuffer<uint> AliveBufferPreSim : register(b4);
RWStructuredBuffer<uint> AliveBufferPostSim : register(b5);
RWStructuredBuffer<uint> DeadBuffer : register(b6);

cbuffer EmitterData : register(b7)
{
    float4 Position;
    float4 StartColor;
    float4 EndColor;
    float4 Velocity;
    float2 Lifetime;
    float2 Size;
    float2 Speed;
    uint EmitCount;
    uint EmitterIndex;
    uint FrameIndex;
    uint Shape;
    float Radius;
    float Angle;
}

float random(float2 st)
{
    return frac(sin(dot(st.xy, float2(12.9898, 78.233))) * 43758.5453123);
}

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    // Don't emit more than we should
    if (id.x >= EmitCount)
        return;
    
    float rnd = random(float2(id.x, FrameIndex));
    float rnd2 = (random(float2(rnd, FrameIndex)) - 0.5f) * 2.0f * Radius;
    float rnd3 = (random(float2(rnd, rnd2)) - 0.5f) * 2.0f * Radius;
    
    // Revive a dead particle
    int deadCount;
    CounterBuffer.InterlockedAdd(DEAD_COUNT_OFFSET, Subtract, deadCount);
    
    // Make sure its a valid particle
    if (deadCount < 1)
        return;
    
    // Get the index of the revived particle and assign it to the particle buffer
    uint particleIndex = DeadBuffer[deadCount - 1];
    Particle particle = ParticleBuffer[particleIndex];
    
    float lifetime = lerp(Lifetime.x, Lifetime.y, rnd);
    float4 randomPos = float4(rnd2, 0.0f, rnd3, 0.0f);
    // Construct a particle based on the emitter's params
    particle.Position = Position + randomPos;
    particle.Color = StartColor;
    particle.Velocity = Velocity;
    
    // Apply cone velocity logic
    if (Angle > 0.0f)
    {
        float3 intialVelo = randomPos;
        float radialAngle = (Angle / 90.0f);
        float3 radialVelo = radialAngle * intialVelo;
        float3 upVelo = (1.0f - radialAngle) * float3(0, 1, 0);
        float3 finalVelo = (radialVelo + upVelo);
        particle.Velocity = float4(finalVelo, 0.0f);
    }
    
    particle.Lifetime = float2(lifetime, lifetime);
    particle.Size = lerp(Size.x, Size.y, rnd);
    particle.Speed = lerp(Speed.x, Speed.y, rnd);
    
    ParticleBuffer[particleIndex] = particle;
    
    // Add the index to the pre-sim alive list (push):
    uint aliveCount;
    CounterBuffer.InterlockedAdd(ALIVE_PRE_SIM_COUNT_OFFSET, Add, aliveCount);
    AliveBufferPreSim[aliveCount] = particleIndex;
}