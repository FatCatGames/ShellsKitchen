#include "../../../../Catbox/Shaders/ShaderFunctions.hlsli"
#include "../../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"
#include "../../../../Catbox/Shaders/UI/UIShaderStructs.hlsli"
#include "../../../../Catbox/Shaders/Color/ColorConversion.hlsli"

Texture2D albedoTexture : register(t0);

cbuffer ResourceInfoBuffer : register(b10)
{
    float Resource;
};

#define GREEN_HUE float(0.3333333333333333)
#define GREEN_SAT float(0.75)
#define GREEN_VAL float(0.9)

#define RED_HUE float(0.0)
#define RED_SAT float(1.0)
#define RED_VAL float(1.0)

PixelOutput main(UISpriteVertexToPixel input)
{
    PixelOutput result;

    float2 uv = input.uv;
    float timeNorm = Resource;
    
    
    //Debug
    //timeNorm = 1-frac(FB_TotalTime*0.1);
    //
    float time = timeNorm * 2 * 60;     // Two minutes in seconds 
    
    float3 green = hsv2rgb(float3(GREEN_HUE, GREEN_SAT, GREEN_VAL));
    float3 red = hsv2rgb(float3(RED_HUE, RED_SAT, RED_VAL));
    
                                //Start             //Stop
    float firstShake = step(time, 40) - step(time, 39);     //<-- time in seconds
                                //Start             //Stop
    float secondShake = step(time, 10) - step(time, 0);     //<-- time in seconds
    
    float shakes = max(firstShake, secondShake);
    
    float shake = (sin(FB_TotalTime * 45) + 1) * 0.5;
    float driver = Remap(0, 1, -0.025, 0.025, shake) * shakes;
    
    uv = RotateScaleUV(uv, 0.5, driver);
    
    float4 sprite = albedoTexture.Sample(wrapSampler, uv);
    
    result.Color.rgb = sprite.rgb;
    result.Color.a = sprite.a;
    result.id = UI_id;
    
    return result;
}