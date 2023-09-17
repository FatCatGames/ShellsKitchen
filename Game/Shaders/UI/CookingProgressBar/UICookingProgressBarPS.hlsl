#include "../../../../Catbox/Shaders/ShaderFunctions.hlsli"
#include "../../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"
#include "../../../../Catbox/Shaders/UI/UIShaderStructs.hlsli"
#include "../../../../Catbox/Shaders/Color/ColorConversion.hlsli"



cbuffer ResourceInfoBuffer : register(b10)
{
    float Resource;
};

#define GREEN_HUE float(0.3333333333333333)
#define GREEN_SAT float(0.5)
#define GREEN_VAL float(0.9)

PixelOutput main(UISpriteVertexToPixel input)
{
    PixelOutput result;

    float2 uv = input.uv;
    float resource = Resource;
    float theBar = Rect(uv, 0, resource, 0, 1, 0.015);

    float3 daColor = hsv2rgb(float3(GREEN_HUE, GREEN_SAT, GREEN_VAL));
    
    result.Color.rgb = theBar * daColor;
    result.Color.a = 1;
    result.id = UI_id;
    
    return result;
}