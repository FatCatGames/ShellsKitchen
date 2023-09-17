#include "../../../../Catbox/Shaders/ShaderFunctions.hlsli"
#include "../../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"
#include "../../../../Catbox/Shaders/UI/UIShaderStructs.hlsli"
#include "../../../../Catbox/Shaders/Color/ColorConversion.hlsli"

Texture2D albedoTexture : register(t0);

cbuffer ResourceInfoBuffer : register(b10)
{
    int Points;       //Points to add or subtract
    float Driver;     //Drives the animation
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
    
    float driver = min(1, Driver * 0.5);
    int points = Points;
    
    //Debug
    //driver = frac(FB_TotalTime * 0.5);
    //points = 75;
    
    //Debug

    float pointsSign = step(1, points);
    points = abs(points);
    
    

    //--------------------------------------------------------------------------------  
    float animDriver = easeOutCirc(linearStep(0.25, 1, driver)) /*easeInOutSine(linearStep(0.3, 1, driver))*/;
    float scale = Remap(0, 1, 3.5, 0.80, animDriver);
    float posX = Remap(0, 1, 0, -0.25, animDriver);
    float posY = Remap(0, 1, 0, 0.2, animDriver);
    
    float2 posAdj = float2(1.5 + posX, 0.1176 + posY);
    
    
    float digitScale = 1/*Remap(0, 1, 1, 0.84, scaleBouncePulse)*/;
    float2 uvScaled = RotateScaleUV(uv, 0.75, 0, float2(scale, scale));
    uvScaled *= float2(2.,0.35);
    
   
    float pointDriver = easeInBounce(smoothstep(0, 0.38, driver));
    float pointsOut = points;
    
    
    int ones = points % 10;
    int tens = (points / 10) % 10;
    int hundreds = (points / 100) % 10;
    
    //--------------------------------------------------------------------------------
    //-- Calculating time for first digit
    
    float onesWhole = float(ones) * 0.1;
    float onesAnim = onesWhole;
    
    
    //-- Calculating position and what number to sample for the first digit --

    float zeroPoint = 0.447 /* UI_color.a *//*0.549*/;
    float2 posOne = float2((posAdj.x - 1.41) * 1.5, posAdj.y);
    float2 uvSamp = uvScaled + float2(posOne);
    float digitMask = Rect(uvSamp, 0.3, 0.7, 0.45, 0.55, 0.01);
    
    uvSamp = RotateScaleUV(uvSamp, 0.55, 0, float2(digitScale, digitScale)); //<-- Individual bounce scaling
    uvSamp += float2(0, onesAnim - zeroPoint);
    
    float4 onesSample = albedoTexture.Sample(wrapSampler, uvSamp) * digitMask;

    
    //--------------------------------------------------------------------------------
    //-- Calculating time for digit 10
    
    float tensWhole = float(tens) * 0.1;
    float tensAnim = tensWhole;
    
    //-- Calculating position and what number to sample for digit 10 --
    
    float2 posTens = float2((posAdj.x - 1.245) * 1.5, posAdj.y);
    uvSamp = uvScaled + float2(posTens);
    digitMask = Rect(uvSamp, 0.3, 0.7, 0.45, 0.55, 0.01);
    
    uvSamp = RotateScaleUV(uvSamp, 0.56, 0, float2(digitScale, digitScale)); //<-- Individual bounce scaling
    uvSamp += float2(0, tensAnim - zeroPoint);
    
    float4 tensSample = albedoTexture.Sample(wrapSampler, uvSamp) * digitMask;

    
    //--------------------------------------------------------------------------------
    //-- Calculating time for 100 digit
    
    float hundredsWhole = float(hundreds) * 0.1;
    float hundredsAnim = hundredsWhole;
    float moreThan99 = step(1, hundreds);
    //-- Calculating position and what number to sample for digit 100 --
    
    float2 posHundreds = float2((posAdj.x - 1.09) * 1.5, posAdj.y);
    uvSamp = uvScaled + float2(posHundreds);
    digitMask = Rect(uvSamp, 0.3, 0.7, 0.45, 0.55, 0.01);
    
    uvSamp = RotateScaleUV(uvSamp, 0.55, 0, float2(digitScale, digitScale)); //<-- Individual bounce scaling
    uvSamp += float2(0, hundredsAnim - zeroPoint);
    
    float4 hundredsSample = albedoTexture.Sample(wrapSampler, uvSamp) * digitMask * moreThan99;
    
    
    
    //--------------------------------------------------------------------------------
    //-- Draw - signs
    float moveSign = Remap(1, 0, 0, 0.15, moreThan99);
    float2 posMinSign = float2((posAdj.x - 1.2 - moveSign) * 1.5, posAdj.y);
    uvSamp = uvScaled + float2(posMinSign);
    float signMask = Rect(uvSamp, 0.054, 0.196, 0.492, 0.50, 0.002);   
    signMask = saturate(signMask + Rect(uvSamp, 0.102, 0.149, 0.4845, 0.508, 0.002) * pointsSign);
    
    
    //--------------------------------------------------------------------------------
    //-- Calculate color
    
    float3 green = hsv2rgb(float3(GREEN_HUE, GREEN_SAT, GREEN_VAL));
    float3 red = hsv2rgb(float3(RED_HUE, RED_SAT, RED_VAL));
    
    float3 color = lerp(red, green, pointsSign);
    
    float4 counterOut = saturate(onesSample + tensSample + hundredsSample + signMask);
    //if ((counterOut.a) < 0.001f)
      //  discard;
    result.Color.rgb = counterOut.rgb * color;
    result.Color.a = counterOut.a * (1 - easeInCirc(animDriver));
    result.id = UI_id;
    
    return result;
}