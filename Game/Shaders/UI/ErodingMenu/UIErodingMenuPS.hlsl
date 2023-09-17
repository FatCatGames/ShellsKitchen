#include "../../../../Catbox/Shaders/ShaderFunctions.hlsli"
#include "../../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"
#include "../../../../Catbox/Shaders/UI/UIShaderStructs.hlsli"
#include "../../../../Catbox/Shaders/Color/ColorConversion.hlsli"

Texture2D albedoTexture : register(t0);

cbuffer ResourceInfoBuffer : register(b10)
{
    float Resource;
};



PixelOutput main(UISpriteVertexToPixel input)
{
    PixelOutput result;
    float t = easeOutCirc(Resource) /** .699*/;
    

    //Debug
    //t = easeOutCirc(UI_color.a);
    //
    
    float2 uv = input.uv;

    float4 foreGround = albedoTexture.Sample(wrapSampler, uv);
    float3 grayscale = rgb2hsv(foreGround.rgb);
    grayscale = hsv2rgb(float3(foreGround.r, 0, foreGround.b));
    
    float4 backGround = float4(0, 0, 0, 0);
    float hightMap = SimplexNoise(uv * 10)+0.2;
    float grayMean = (grayscale.r + grayscale.g + grayscale.b + hightMap) * 0.25;
    hightMap = saturate((hightMap + grayMean) * 0.5 + 0.3) ;
    
    float rect = Rect(uv, 0, 1, t, 1, 0.4) ;

    hightMap *= rect;
    
    float adder = Remap(0, 1, 0, 0.39, t);
    float driver = saturate(t * (1 - uv.y)) + adder;
    
    float4 daColor = AlphaGradientErode(hightMap, backGround, foreGround, driver, 0.2);
    
    //if ((daColor.a * foreGround.a) < 0.0001f)
      //  discard;
    result.Color.rgb = daColor.rgb;
    result.Color.a = daColor.a * foreGround.a;
    result.id = UI_id;
    
    return result;
}