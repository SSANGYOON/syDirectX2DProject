#include "global.hlsli"

struct Particle
{
    float3  worldPos;
    float   curTime;
    float3  worldDir;
    float   lifeTime;
    int     alive;
    float3  padding;
};

struct ComputeShared
{
    int addCount;
    float3 padding;
};

RWStructuredBuffer<Particle> g_particle : register(u0);
RWStructuredBuffer<ComputeShared> g_shared : register(u1);

[numthreads(16, 16, 1)]
void CS_MAIN(int3 DispatchID : SV_DispatchThreadID, int GroupIndex : SV_GroupIndex)
{

    uint maxCount = g_int_0;
    uint addCount = g_int_1;
    float zOffSet = g_float_0;

    float deltaTime = g_vec2_0.x;
    float accTime = g_vec2_0.y;

    float2 particleSpriteSize = g_vec2_1;
    float2 force = g_vec2_3;

    float2 initialPos = g_vec2_2.xy;
    float2 initialPosVarFrom = g_vec4_0.xy;
    float2 initialPosVarTo = g_vec4_0.zw;

    float2 initialDir = g_vec4_1.xy;
    float2 initialDirVar = g_vec4_1.zw;

    float2 aliveZone = g_vec4_2.xy;

    uint aliveZoneType = g_vec4_2.z;
    uint posvarType = g_vec4_2.w;

    float minLifeTime = g_vec4_3.x;
    float maxLifeTime = g_vec4_3.y;
    float minSpeed = g_vec4_3.z;
    float maxSpeed = g_vec4_3.w;
    
    g_shared[0].addCount = addCount;
    GroupMemoryBarrierWithGroupSync();

    float pi = 3.1415928;
    if (g_particle[GroupIndex].alive == 0)
    {
        while (true)
        {
            int remaining = g_shared[0].addCount;
            if (remaining <= 0)
                break;

            int expected = remaining;
            int desired = remaining - 1;
            int originalValue;
            InterlockedCompareExchange(g_shared[0].addCount, expected, desired, originalValue);

            if (originalValue == expected)
            {
                g_particle[GroupIndex].alive = 1;
                break;
            }
        }

        float x = DispatchID.x;
        float y = DispatchID.y;
        if (g_particle[GroupIndex].alive == 1)
        {
            float r1 = Rand(float2(x, y));
            float r2 = Rand(float2(y, x));
            float r3 = Rand(float2(x * accTime, y));
            float r4 = Rand(float2(x, y * accTime));
            float r5 = Rand(float2(x * accTime, y * accTime));
            // [0.5~1] -> [0~1]
            float4 noise =
            {
                2 * r1 - 1,
                2 * r2 - 1,
                2 * r3 - 1,
                2 * r4 - 1
            };

            float2 dir = initialDir + initialDirVar * float2(cos(noise.x * 2 * pi), sin(noise.x * 2 * pi));
            dir = normalize(dir);
            g_particle[GroupIndex].worldDir = float3(dir.x, dir.y, 0.f);

            if (posvarType == 0) {
                float2 pos = initialPos + (initialPosVarFrom + (initialPosVarTo - initialPosVarFrom) * noise.z) * float2(cos(noise.y * 2 * pi), sin(noise.y * 2 * pi));
                g_particle[GroupIndex].worldPos = float3(pos, zOffSet);
            }
            else if (posvarType == 1) {
                float2 pos = initialPos + (initialPosVarFrom + (initialPosVarTo - initialPosVarFrom) * noise.z) * float2(dir);
                g_particle[GroupIndex].worldPos = float3(pos, zOffSet);
            }
            else {
                float2 pos = initialPos + initialPosVarTo * float2(noise.y, noise.z);
                g_particle[GroupIndex].worldPos = float3(pos, zOffSet);
            }

            g_particle[GroupIndex].lifeTime = (maxLifeTime - minLifeTime) * (2 * r5 - 1) + minLifeTime;
            g_particle[GroupIndex].curTime = 0;
        }
    }
    else
    {
        g_particle[GroupIndex].curTime += deltaTime;
        if (g_particle[GroupIndex].lifeTime < g_particle[GroupIndex].curTime)
        {
            g_particle[GroupIndex].alive = 0;
            return;
        }

        float ratio = g_particle[GroupIndex].curTime / g_particle[GroupIndex].lifeTime;
        float2 velocity = g_particle[GroupIndex].worldDir.xy * ((maxSpeed - minSpeed) * ratio + minSpeed) - force.x * g_particle[GroupIndex].worldPos.xy * g_particle[GroupIndex].curTime;
   
        g_particle[GroupIndex].worldPos += float3(velocity * deltaTime, 0.f);

        if (aliveZoneType == 1)
        {
            if (length(g_particle[GroupIndex].worldPos) > aliveZone.x) {
                g_particle[GroupIndex].alive = 0;
                return;
            }
        }
    }
}