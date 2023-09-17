#include "../../../../Catbox/Shaders/ShaderFunctions.hlsli"
#include "../../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"
#include "../../../../Catbox/Shaders/UI/UIShaderStructs.hlsli"
#include "../../../../Catbox/Shaders/Color/ColorConversion.hlsli"

Texture2D albedoTexture : register(t0);

cbuffer ResourceInfoBuffer : register(b10)
{
    float Driver = 0.0f;     //Drives the effect 0..1 to add Points and subtract points
    int TotalPoints = 0;  //Feed me curent amount of points
    int Points = 0;       //Points to add or subtract
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
    
    float driver = min(1, Driver);
    int totalPoints = TotalPoints;
    int points = Points;
    
    //Debug
        //driver = frac(FB_TotalTime * 0.5);
        //totalPoints = int(UI_color.a * 255);
        //points = int(UI_color.b * 255);
    
    //Debug
    
    
    //--------------------------------------------------------------------------------
    float scaleBouncePulse = easeOutBounce(smoothstep(0.25, 0.5, driver)) - easeOutBounce(smoothstep(0.7, 0.9, driver));
    //float scalePulse = smoothstep(0.3, 0.7, driver) - smoothstep(0.90, 1, driver);
    
    float scale = Remap(0, 1, 1, 0.85, scaleBouncePulse);
    float digitScale = Remap(0, 1, 1, 0.82, scaleBouncePulse);
    float2 uvScaled = RotateScaleUV(uv, 0.75, 0, float2(scale, scale));
    uvScaled *= float2(2., 0.35);
    
   
    float pointDriver = easeInBounce(smoothstep(0, 0.38, driver));
    float neg = step(points, 0); //<-- used to adjust the subratction to work with negative points
    float pointsOut = max(0, lerp(totalPoints - (points - neg), totalPoints, pointDriver));
    
    
    //--------------------------------------------------------------------------------
    //-- Calculating time for first digit
    
    float onesWhole = int(pointsOut) * 0.1;
    float onesAnim = onesWhole;
    
    
    //-- Calculating pos and sampling position for first digit --

    float2 posAdj = float2(0.7843, 0.3255);
    
    float zeroPoint = 0.447 /* UI_color.a */ /*0.549*/;
    float2 posOne = float2((posAdj.x - 1.5) * 1.5, posAdj.y);
    float2 uvSamp = uvScaled + float2(posOne);
    float digitMask = Rect(uvSamp, 0.3, 0.7, 0.45, 0.55, 0.01);
    
    uvSamp = RotateScaleUV(uvSamp, 0.55, 0, float2(digitScale, digitScale)); //<-- Individual bounce scaling
    uvSamp += float2(0, onesAnim - zeroPoint);
    
    float4 onesSample = albedoTexture.Sample(wrapSampler, uvSamp) * digitMask;

    
    //--------------------------------------------------------------------------------
    //-- Calculating pos and sampling position for digit 10
    
    float tensWhole = int(onesWhole) * 0.1;
    float tensAnim = tensWhole;
    float showDigit = step(1, onesWhole);
    
    //-- Calculating position and what number to sample for digit 10 --
    
    float2 posTens = float2((posAdj.x - 1.3) * 1.5, posAdj.y);
    uvSamp = uvScaled + float2(posTens);
    digitMask = Rect(uvSamp, 0.3, 0.7, 0.45, 0.55, 0.01);
    
    uvSamp = RotateScaleUV(uvSamp, 0.56, 0, float2(digitScale, digitScale)); //<-- Individual bounce scaling
    uvSamp += float2(0, tensAnim - zeroPoint);
    
    float4 tensSample = albedoTexture.Sample(wrapSampler, uvSamp) * digitMask * showDigit;

    
    //--------------------------------------------------------------------------------
    //-- Calculating pos and sampling position for 100 digit
    
    float hundredsWhole = int(tensWhole) * 0.1;
    float hundredsAnim = hundredsWhole;
    showDigit = step(1, tensWhole);
    
    //-- Calculating position and what number to sample for digit 100 --
    
    float2 posHundreds = float2((posAdj.x - 1.1) * 1.5, posAdj.y);
    uvSamp = uvScaled + float2(posHundreds);
    digitMask = Rect(uvSamp, 0.3, 0.7, 0.45, 0.55, 0.01);
    
    uvSamp = RotateScaleUV(uvSamp, 0.55, 0, float2(digitScale, digitScale)); //<-- Individual bounce scaling
    uvSamp += float2(0, hundredsAnim - zeroPoint);
    
    float4 hundredsSample = albedoTexture.Sample(wrapSampler, uvSamp) * digitMask * showDigit;

    
    //--------------------------------------------------------------------------------
    //-- Calculating pos and sampling position for 1000 digit
    
    float thousandsWhole = int(hundredsWhole) * 0.1;
    float thousandsAnim = thousandsWhole;
    showDigit = step(1, hundredsWhole);
    
    //-- Calculating position and what number to sample for digit 100 --
    
    float2 posThousands = float2((posAdj.x - 0.9) * 1.5, posAdj.y);
    uvSamp = uvScaled + float2(posThousands);
    digitMask = Rect(uvSamp, 0.3, 0.7, 0.45, 0.55, 0.01);
    
    uvSamp = RotateScaleUV(uvSamp, 0.55, 0, float2(digitScale, digitScale)); //<-- Individual bounce scaling
    uvSamp += float2(0, thousandsAnim - zeroPoint);
    
    float4 thousandsSample = albedoTexture.Sample(wrapSampler, uvSamp) * digitMask * showDigit;
    
    //--------------------------------------------------------------------------------
    //-- Calculate color

    float3 green = hsv2rgb(float3(GREEN_HUE, GREEN_SAT, GREEN_VAL));
    float3 red = hsv2rgb(float3(RED_HUE, RED_SAT, RED_VAL));
    float3 neutral = float3(1, 1, 1);
    float negPoints = step(points, 0);
    
    float3 color = lerp(green, red, negPoints);
    color = lerp(neutral, color, scaleBouncePulse);
    
    float4 counterOut = saturate(onesSample + tensSample + hundredsSample + thousandsSample);
    //if ((counterOut.a) < 0.001f)
      //  discard;
    result.Color.rgb = counterOut.rgb * color;
    result.Color.a = counterOut.a;
    result.id = UI_id;
    
    return result;
}