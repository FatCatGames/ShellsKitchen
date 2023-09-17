#include "../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"
#include "../../../Catbox/Shaders/PBRFunctions.hlsli"
#include "../../../Catbox/Shaders/ShaderFunctions.hlsli"


Texture2D Noise : register(t11);


PixelOutput main(VertexToPixel input)
{
    PixelOutput result;

    float usedTime = min(OB_AliveTime, 1.2f);
    float2 speed = float2(MB_CustomValue1, MB_CustomValue2);
    

    if (MB_CustomValue3 > 0.f)
    {
        usedTime = -usedTime;

    }
    speed.x *= 0.1;
    speed.y *= 0.1;
    
    float2 uv = input.AlbedoUV;
    
    float redCh = AlbedoTex.Sample(wrapSampler, float2(uv.x + (speed.x * 1.04f) * usedTime, uv.y + speed.y * usedTime)).r;
    float greenCh = AlbedoTex.Sample(wrapSampler, float2(uv.x + (speed.x * 0.96f) * usedTime, uv.y + speed.y * usedTime)).g;
    float blueCh = AlbedoTex.Sample(wrapSampler, float2(uv.x + (speed.x * 0.84f) * usedTime, uv.y + speed.y * usedTime)).b;
    float alphaCh = AlbedoTex.Sample(wrapSampler, float2(uv.x + speed.x * usedTime, uv.y + speed.y * usedTime)).a;
    
    float4 white = float4(1, 1, 1, 1);
    float4 red = float4(1, 0, 0, greenCh);
    
    float4 albedo = float4(MB_Color.rgb * uv.xyy * 0.2f, alphaCh);
    albedo += white * redCh * 0.7f;
    albedo += float4(0, 0.4f, 0.8f, 1) * greenCh;
    albedo += float4(0.9f, 0.1f, 0.7f, 1) * blueCh * 3;
    albedo *= MB_EmissiveStrength;
    albedo.a = saturate(lerp(1, 0, OB_AliveTime * 1.85f));
    
    if (albedo.a < 0.05f)
        discard;

    result.Color = albedo;
    return result;
}