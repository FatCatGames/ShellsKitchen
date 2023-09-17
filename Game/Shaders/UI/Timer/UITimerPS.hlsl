#include "../../../../Catbox/Shaders/ShaderFunctions.hlsli"
#include "../../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"
#include "../../../../Catbox/Shaders/UI/UIShaderStructs.hlsli"
#include "../../../../Catbox/Shaders/Color/ColorConversion.hlsli"

Texture2D albedoTexture : register(t0);

cbuffer ResourceInfoBuffer : register(b10)
{
    float Resource = 2; //Feed me a count down in seconds
};

#define SEXTIODEL float(0.0166666666666667)

PixelOutput main(UISpriteVertexToPixel input)
{
    PixelOutput result;
    
    float seconds = max(0, Resource);
    //Debug
    //seconds = max(0, 12 - OB_AliveTime);
    //Debug
     
    float minutes = seconds * SEXTIODEL;
       
    float2 uv = input.uv;

    
    //--------------------------------------------------------------------------------
    //-- Calculating for global bounce scaling         
    float sub10sec = step(seconds, 10);
    float pulseDriver = smoothstep(0.5, 1, frac(seconds)) * sub10sec;
    float pulse = easeInBounce(pulseDriver);
    
    float scale = Remap(0, 1, 1, 0.8, pulse);
    float2 uvScaled = RotateScaleUV(uv, 0.45, 0, float2(scale, scale));
    float digitScale = Remap(1, 0.8, 1, 0.85, scale);
    
    uvScaled *= float2(3, 0.5);
 
    
    //float2 uvScaled = uv * float2(3,0.5);
    float2 adjPos = float2(0.204, 0.2549);

    //--------------------------------------------------------------------------------
    //-- Calculating time for first seconds digit    
    float secOnesWhole = frac(floor(frac(seconds * 0.1) * 10) * 0.1);
    float secOnesAnim = easeInElastic(smoothstep(0.43, 1, frac(seconds))) * 0.1 + secOnesWhole;
    
    
    //-- Calculating position and what number to sample for the first seconds digit --
    float zeroPoint = 0.447;
    float2 posSecOne = float2((adjPos.r - 1) * 1.5, adjPos.g);
    float2 uvSamp = uvScaled + float2(posSecOne);
    float digitMask = Rect(uvSamp, 0.3, 0.7, 0.46, 0.54, 0.01);
    
    uvSamp = RotateScaleUV(uvSamp, 0.55, 0, float2(digitScale, digitScale)); //<-- Individual bounce scaling
    uvSamp += float2(0, secOnesAnim - zeroPoint);
    
    float4 secondsOne = albedoTexture.Sample(wrapSampler, uvSamp) * digitMask;

    
    //--------------------------------------------------------------------------------
    //-- Calculating time for second seconds digit    
    float secTen = fmod(seconds * 0.1, 6);
    float secTenWhole = frac(floor(frac(secTen * 0.1) * 10) * 0.1);
    float secTenAnim = easeInElastic(smoothstep(0.93, 1, frac(secTen))) * 0.1 + secTenWhole;
    
    //-- Calculating position and what number to sample for the second seconds digit --    
    float2 posSecTen = float2((adjPos.r - 0.8) * 1.5, adjPos.g);
    uvSamp = uvScaled + float2(posSecTen);
    digitMask = Rect(uvSamp, 0.3, 0.7, 0.46, 0.54, 0.01);
    
    uvSamp = RotateScaleUV(uvSamp, 0.56, 0, float2(digitScale, digitScale)); //<-- Individual bounce scaling
    uvSamp += float2(0, secTenAnim - zeroPoint);
    
    float4 secondsTen = albedoTexture.Sample(wrapSampler, uvSamp) * digitMask;

    
    //--------------------------------------------------------------------------------
    //-- Calculating time for first minute digit
    float minOnes = fmod(seconds * 0.1, 6);
    float minOnesWhole = frac(floor(frac(minutes * 0.1) * 10) * 0.1);
    float minOnesAnim = easeInElastic(smoothstep(0.97, 1, frac(minutes))) * 0.1 + minOnesWhole;
    
    //-- Calculating position and what number to sample for the first minute digit --    
    float2 posMinute = float2((adjPos.r - 0.5) * 1.5, adjPos.g);
    uvSamp = uvScaled + float2(posMinute);
    digitMask = Rect(uvSamp, 0.3, 0.7, 0.46, 0.54, 0.01);
        
    uvSamp = RotateScaleUV(uvSamp, 0.55, 0, float2(digitScale, digitScale)); //<-- Individual bounce scaling
    uvSamp += float2(0, minOnesAnim - zeroPoint);
    
    float4 minutesOne = albedoTexture.Sample(wrapSampler, uvSamp) * digitMask;
    
    
    //--------------------------------------------------------------------------------
    //-- Paint two dots
    

    float2 uvN = RotateScaleUV(uv, 0.45, 0, float2(scale, scale)); //<-- Individual bounce scaling
    uvN -= 0.5;
    
    float radius = Remap(1, 0.8, 0.014, 0.02, scale);             //<-- Individual bounce scaling
    float2 aDotPos = float2(uvN.x + 0.11, uvN.y + 0.04);
    float twoDots = Circle(aDotPos, radius, 0.005);
    aDotPos = float2(uvN.x + 0.11, uvN.y + 0.00);
    twoDots += Circle(aDotPos, radius, 0.005);
    
    
    //--------------------------------------------------------------------------------
    //-- Calculate color
    float eachMinColor = smoothstep(0.979, 1, frac(minutes));
    float3 color = lerp(float3(1, 1, 1), float3(1, 0, 0), eachMinColor);
    
    float sub30sec = step(seconds, 30);
    float eachSecSub30 = smoothstep(0.43, 1, frac(seconds)) * sub30sec;
    color = lerp(color, float3(1, 0, 0), eachSecSub30);
    
    float zero = step(seconds, 1);
    color = lerp(color, float3(1, 0, 0), zero);
    
    
    //--------------------------------------------------------------------------------
    float4 counterOut = saturate(secondsOne + secondsTen + minutesOne + twoDots);
    if ((counterOut.a) < 0.001f)
        discard;
    result.Color.rgb = counterOut.rgb * color;
    result.Color.a = counterOut.a;
    result.id = UI_id;
    
    return result;
}