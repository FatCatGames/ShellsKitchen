#include "Struct/ShaderStructs.hlsli"
#include "ShaderFunctions.hlsli"

cbuffer UVPannerBuffer : register(b10)
{
    float xSpeed = 0;
    float ySpeed = 0;
    bool isNegative = false;
    bool isAccelerate = false;
}

Texture2D albedoTexture : register(t0);

Texture2D Noise : register(t11);


PixelOutput main(VertexToPixel input)
{
    PixelOutput result;

    float usedTime = OB_AliveTime;
    float2 speed = float2(xSpeed, ySpeed);
    

    
    if (isAccelerate)
    {
        usedTime = frac(OB_AliveTime) * frac(OB_AliveTime);
    }
    if (isNegative)
    {
        usedTime = -usedTime;

    }
    speed.x *= 0.1;
    speed.y *= 0.1;
    
    float4 albedo = albedoTexture.Sample(wrapSampler, float2(input.AlbedoUV.x + speed.x * usedTime, input.AlbedoUV.y + speed.y * usedTime));
    
    float redCh =   Noise.Sample(defaultSampler, float2(input.AlbedoUV.x + speed.x * usedTime, input.AlbedoUV.y + speed.y * usedTime)).r;
    float greenCh = Noise.Sample(defaultSampler, float2(input.AlbedoUV.x + speed.x * usedTime, input.AlbedoUV.y + speed.y * usedTime)).g;
    float blueCh =  Noise.Sample(defaultSampler, float2(input.AlbedoUV.x + speed.x * usedTime, input.AlbedoUV.y + speed.y * usedTime)).b;
    float alphaCh = Noise.Sample(defaultSampler, float2(input.AlbedoUV.x + speed.x * usedTime, input.AlbedoUV.y + speed.y * usedTime)).a;
    
    if (albedo.a < 0.05f)
        discard;
    result.Color = albedo;
    return result;
}