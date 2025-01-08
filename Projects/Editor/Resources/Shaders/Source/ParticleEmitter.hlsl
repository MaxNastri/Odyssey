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
#define SPHERE 2

static const uint Add = 1;
static const uint Subtract = -1;
static const float PI = 3.14159265f;

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

float3 RandomInsideCircle(float id, float radius)
{
    float elevationAngle = random(float2(id.x, FrameIndex)) * PI;
    float azimuth = random(float2(id.x, elevationAngle)) * 2 * PI;
    
    float x = Radius * sin(elevationAngle) * cos(azimuth);
    float y = 0.0f;
    float z = Radius * cos(elevationAngle);
    return float3(x, y, z);
}

float3 RandomInsideUnitPlane(float id)
{
    float rndX = random(float2(id.x, FrameIndex));
    float rndZ = random(float2(rndX, id.x));

    return float3(rndX, 0.0f, rndZ);
}

float3 RandomInsideSphere(float id, float radius)
{
    float elevationAngle = random(float2(id.x, FrameIndex)) * PI;
    float azimuth = random(float2(id.x, elevationAngle)) * 2 * PI;
    
    float x = radius * sin(elevationAngle) * cos(azimuth);
    float y = radius * sin(elevationAngle) * sin(azimuth);
    float z = radius * cos(elevationAngle);
    return float3(x, y, z);
}

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    // Don't emit more than we should
    if (id.x >= EmitCount)
        return;
    
    float rnd = random(float2(id.x, FrameIndex));
    
    // Revive a dead particle
    int deadCount;
    CounterBuffer.InterlockedAdd(DEAD_COUNT_OFFSET, Subtract, deadCount);
    
    // Make sure its a valid particle
    if (deadCount < 1)
        return;
    
    // Get the index of the revived particle and assign it to the particle buffer
    uint particleIndex = DeadBuffer[deadCount - 1];
    Particle particle = ParticleBuffer[particleIndex];
    
    // Generate a random position and velocity multiplier based on the shape
    float3 randomPos = RandomInsideCircle(id.x, Radius);
    particle.Position = Position + float4(randomPos, 0.0f);
    particle.Velocity = Velocity;
    
    // Apply sphere velocity logic
    //if (Shape == SPHERE)
    //{
    //    float4 velocity = float4(normalize(randomPos), 0.0f);
    //    particle.Velocity = velocity;
    //}
    // Apply cone velocity logic
    if (Angle > 0.0f)
    {
        float3 intialVelo = randomPos;
        float radialAngle = (Angle / 90.0f);
        float3 radialVelo = radialAngle * intialVelo;
        float3 upVelo = (1.0f - radialAngle) * particle.Velocity.xyz;
        float3 finalVelo =  radialVelo + upVelo;
        particle.Velocity = float4(finalVelo, 0.0f);
    }
    
    float lifetime = lerp(Lifetime.x, Lifetime.y, rnd);
    particle.Lifetime = float2(lifetime, lifetime);
    particle.Size = lerp(Size.x, Size.y, rnd);
    particle.Speed = lerp(Speed.x, Speed.y, rnd);
    particle.Color = StartColor;
    
    ParticleBuffer[particleIndex] = particle;
    
    // Add the index to the pre-sim alive list (push):
    uint aliveCount;
    CounterBuffer.InterlockedAdd(ALIVE_PRE_SIM_COUNT_OFFSET, Add, aliveCount);
    AliveBufferPreSim[aliveCount] = particleIndex;
}