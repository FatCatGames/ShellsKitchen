#include "../../../../Catbox/Shaders/ShaderFunctions.hlsli"
#include "../../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"
#include "../../../../Catbox/Shaders/UI/UIShaderStructs.hlsli"
#include "../../../../Catbox/Shaders/Color/ColorConversion.hlsli"



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
    float resource = Resource;
    float theBar = Rect(uv, 0, resource, 0, 1, 0.015);
    
    float Hue = Remap(0, 1, RED_HUE, GREEN_HUE, resource);
    
    float Saturation = Remap(0.1, 1, RED_SAT, GREEN_SAT, resource);
    
    float Value = Remap(0.1, 1, RED_VAL, GREEN_VAL, resource);
    
    float3 HSV = float3(Hue, Saturation, Value);
    float3 daColor = hsv2rgb(HSV);
    
    result.Color.rgb = daColor * theBar;
    result.Color.a = 1;
    result.id = UI_id;
    
    return result;
}