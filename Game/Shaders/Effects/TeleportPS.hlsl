#include "../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"
#include "../../../Catbox/Shaders/PBRFunctions.hlsli"
#include "../../../Catbox/Shaders/ShaderFunctions.hlsli"

Texture2D Noise : register(t11);


PixelOutput main(VertexToPixel input)
{
    PixelOutput result;
    float time = -(FB_TotalTime * 0.6f);
    
    float yNoise = SimplexNoise(input.AlbedoUV + (time));
    float xNoise = SimplexNoise((input.WorldPosition.zw + (time)));
    
    float2 noise = float2(xNoise, yNoise);
    
    float angle = time % -6.28f;
    float angleTwo = (FB_TotalTime * 0.345f) % 6.28f;
    
    float2 noisedUV = input.AlbedoUV + noise * 0.1f;
    float2 noisedUVDos = input.AlbedoUV + noise * 0.225f;
    
    float2 snurrUV = RotateUV(noisedUV, angle);
    float2 snurrUVDos = RotateUV(noisedUVDos, angle);
    
    float whirlMask = Spiral(snurrUV, -FB_TotalTime * 0.4f);
    whirlMask *= Circle((input.AlbedoUV - 0.5), OB_CustomFloat, 0.05f);

    float circleMask = Circle((input.AlbedoUV - 0.5) , min(OB_CustomFloat, 0.52f), max(1.f - OB_CustomFloat, 0.6f));
    float4 bgCol = (MB_Color * circleMask) * (OB_CustomFloat);

    float dotsUno = AlbedoTex.Sample(defaultSampler, snurrUV).r;
    float dotDos = AlbedoTex.Sample(defaultSampler, snurrUVDos).g;
    
    float cycle = pow(sin(FB_TotalTime * 0.3f), 2) + 1;
    float cycleDos = pow(sin(FB_TotalTime * 0.547f), 2) + 1;
    
    float spiralUno = AlbedoTex.Sample(wrapSampler, RotateScaleUV(noisedUV, 0.5, angle, float2(cycle, cycle))).b;
    float spiralDos = AlbedoTex.Sample(wrapSampler, RotateScaleUV(noisedUV, 0.5, angle, float2(cycleDos, cycleDos))).a;
    
    float dotsMask = lerp(0, 1, dotsUno * dotDos);
    float spiralMask = 10 * smoothstep(0, 1, spiralUno * spiralDos) * circleMask;
    float whirlCirc = Circle(snurrUV - 0.5f, 0.25f, 0.6f);
    float4 white = float4(0.8f, 0.8f, 1, 1);
    
    
    float4 col = MB_Color * MB_EmissiveStrength * OB_CustomFloat;
    float4 lightCol = float4(MB_Color.r * 0.8, MB_Color.g * 1.8f, MB_Color.b * 2.3f, 1.f) * MB_EmissiveStrength * OB_CustomFloat;
    float4 darkCol = float4(MB_Color.r * 1.8, MB_Color.g * 0.5f, MB_Color.b * 0.8f, circleMask) * MB_EmissiveStrength * OB_CustomFloat;
    float4 color = lerp(float4(0.3,0,0.3, 0), darkCol, circleMask);
    
    if (color.a < 0.05f || OB_CustomBool)
        discard;
    
    color += (white * dotsMask) * 20;
    color += (lightCol * spiralMask) * 2;
    color += (col * whirlMask * whirlCirc) * 4;
    color.a = saturate(color.a);

    result.Color = color; //(whirl + dotsMask) * (MB_EmissiveStrength * OB_CustomFloat * MB_Color); //float4(circle, 0, 0, circle);
    result.id = OB_id;
    return result;
}