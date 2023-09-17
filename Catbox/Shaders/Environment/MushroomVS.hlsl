//#include "Struct/ShaderStructs.hlsli"
#include "../../../Catbox/Shaders/ShaderFunctions.hlsli"
#include "../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"


//Good values
//30.2 Pulse
//2.8 Speed
//6.2 Intensity


cbuffer ScalingBuffer : register(b10)
{
    float Pulse = 30.2f;
    float Intensity = 6.2f;
    float Speed = 2.8f;
    int Power = 2;
}


VertexToPixel main(VertexInput input)
{
    VertexToPixel result;

    float4x4 skinningMatrix =
    {
        1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
    };

    if (!OB_isStatic)
    {
        skinningMatrix = 0;
        skinningMatrix += input.BoneWeights.x * SB_BoneData[input.BoneIDs.x];
        skinningMatrix += input.BoneWeights.y * SB_BoneData[input.BoneIDs.y];
        skinningMatrix += input.BoneWeights.z * SB_BoneData[input.BoneIDs.z];
        skinningMatrix += input.BoneWeights.w * SB_BoneData[input.BoneIDs.w];
    }
    
    //float pulse = 30.2;
    //float intensity = 6.2 * input.VxColor;
    //float speed = 0.8;
	
    //float4 vertexWorldPosition;
    //float4 localPos = input.Position;
    //localPos *= 0.001;
    //float dist = length(float3(localPos.xyz));

    //float pulseDist = dist * pulse;
	
    //float motion = easeInOutBounce(pow(sin(FB_TotalTime * speed + pulseDist), 2));
    //float3 posOffset = input.Normal * motion;
    //posOffset.x *= 1.6;
    //posOffset *= intensity;
    //float4 noise = NoiseTex.Sample(defaultSampler, input.AlbedoUV + FB_TotalTime);
    float noiseAmp =  SimplexNoise(input.AlbedoUV + FB_TotalTime * 0.6f);
    float noiseAmp2 = SimplexNoise(input.AlbedoUV + FB_TotalTime * 0.37f);
    float noiseAmp3 = SimplexNoise(input.AlbedoUV + FB_TotalTime * 0.44f);
    
    float4 vertexWorldPosition;
    vertexWorldPosition = mul(OB_ToWorld, mul(input.Position, skinningMatrix));
    float4 localPos = input.Position;
    localPos *= 0.001;
    float dist = length(float3(localPos.xyz));
    
    float pulseDist = dist * Pulse;
	
    float motion = easeInOutBounce(pow(sin((FB_TotalTime * Speed + pulseDist) + vertexWorldPosition.y), 2));
    float3 posOffset = (input.Normal + input.Binormal) * motion;
    //posOffset.x *= 1.6;
    


    posOffset *= (input.VxColor * Intensity) * noiseAmp;

    //vertexWorldPosition = mul(OB_ToWorld, mul(input.Position + float4(normalize(input.Normal), 0) * BellCurve(easeInOutBounce(frac(FB_TotalTime * 0.5f))) * 45, skinningMatrix));
    vertexWorldPosition = mul(OB_ToWorld, mul(input.Position + float4(posOffset, 0), skinningMatrix));
    
    if (OB_isInstanced)
    {
        vertexWorldPosition = mul(input.World, mul(input.Position, skinningMatrix));
    }
    
    const float4 vertexViewPosition = mul(FB_ToView, vertexWorldPosition);
    const float4 vertexProjectionPosition = mul(FB_ToProjection, vertexViewPosition);

    const float3x3 worldNormalRotation = (float3x3) OB_ToWorld;
    const float3x3 skinNormalRotation = (float3x3) skinningMatrix;

    result.ProjectedPosition = vertexProjectionPosition;
    result.WorldPosition = vertexWorldPosition;

    result.VxColor = input.VxColor;
    result.NormalTex = input.NormalTex;
    result.Roughness = input.Roughness;
    result.AO = input.AO;
    result.AlbedoUV = input.AlbedoUV;
    result.NormalUV = input.NormalUV;
    result.RoughnessUV = input.RoughnessUV;
    result.AOUV = input.AOUV;

    result.Tangent = mul(worldNormalRotation, mul(input.Tangent, skinNormalRotation));
    result.Binormal = mul(worldNormalRotation, mul(input.Binormal, skinNormalRotation));
    result.Normal = normalize(mul(worldNormalRotation, mul(input.Normal, skinNormalRotation)));

    return result;
}