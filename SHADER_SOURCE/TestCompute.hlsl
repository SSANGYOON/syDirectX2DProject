#include "global.hlsli"

RWTexture2D<float4> tex : register(u0);

[numthreads(32, 32, 1)]
void CS_MAIN( uint3 DTid : SV_DispatchThreadID)
{
	tex[DTid.xy] = float4(Rand(float2(DTid.x, DTid.y)), Rand(float2(DTid.y, DTid.x)), Rand(float2(DTid.x * DTid.x, DTid.y * DTid.y)), 1.f);
}