#include "Struct/ShaderStructs.hlsli"
#include "PBRFunctions.hlsli"
#include "ShaderFunctions.hlsli"

struct DeferredPixelOutput
{
	float4 Color : SV_TARGET0;
	float3 LightResult : SV_TARGET1;
	float2 ScreenUV	: SV_TARGET6;
};

Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D pbrTexture : register(t2);
Texture2D vertexNormalTexture : register(t3);
Texture2D worldPositionTexture : register(t4);
Texture2D ambientOcclusionTexture : register(t5);
Texture2D emmissiveTexture : register(t6);
Texture2D shadowMap : register(t7);
Texture2D SSAOTexture : register(t8);
StructuredBuffer<LightData> lights : register(t9);
TextureCube environmentTexture : register(t10);
Texture2D pointShadowMaps[6] : register(t11);

DeferredPixelOutput main(DeferredVertexToPixel input)
{
	DeferredPixelOutput result;
	//Load data from all our GBuffer textures.

	/*result.Color.rgb = environmentTexture.Sample(defaultSampler, input.UV).rgb;
	result.Color.a = 1;
	return result;*/

	/*float shadow = shadowMap.Sample(defaultSampler, input.UV).r;
	result.Color = float4(shadow, 0, 0, 1);
	return result;*/

	float4 albedo = albedoTexture.Sample(defaultSampler, input.UV);
	if (albedo.a == 0) discard;

	float3 pixelNormal = normalTexture.Sample(defaultSampler, input.UV).rgb;
	const float4 pbr = pbrTexture.Sample(defaultSampler, input.UV);
	const float3 vertexNormal = vertexNormalTexture.Sample(defaultSampler, input.UV).rgb;
	const float4 worldPosition = worldPositionTexture.Sample(defaultSampler, input.UV);
	//const float4 pixelPosition = worldPositionTexture.Sample(defaultSampler, input.UV);
	const float ao = ambientOcclusionTexture.Sample(defaultSampler, input.UV).r;

	const float metalness = pbr.r;
	const float roughness = pbr.g;
	
    const float3 emissive = emmissiveTexture.Sample(defaultSampler, input.UV).rgb;


    if (FB_DebugMode != 0)
    {
        result.Color.a = 1;
		//albedo
        if (FB_DebugMode == 1)
        {
            result.Color = albedo;
        }
		//vx Color
        else if (FB_DebugMode == 2)
        {
			//result.Color = input.VxColor * MB_Color;
        }
		//vertex normals
        else if (FB_DebugMode == 3)
        {
            float3 debugNormal = vertexNormal;
            const float signedLength = (debugNormal.r + debugNormal.g + debugNormal.b) / 3;
            if (signedLength < 0)
            {
                debugNormal = float3(1 - abs(debugNormal));
            }
            result.Color.rgb = debugNormal;
        }
		//normal map
        else if (FB_DebugMode == 4)
        {
            float4 normalMap = normalTexture.Sample(defaultSampler, input.UV).rgba;
            result.Color = float4(normalMap.r, normalMap.g, 1, 1);
        }
		//UVs
        else if (FB_DebugMode == 5)
        {
            result.Color = float4(input.UV.r, input.UV.g, 0, 1);
        }
		//AO
        else if (FB_DebugMode == 6)
        {
            result.Color.rgb = ao;
        }
		//Roughness
        else if (FB_DebugMode == 7)
        {
            result.Color.rgb = roughness;
        }
		//Metallic
        else if (FB_DebugMode == 8)
        {
            result.Color.rgb = metalness;
        }
		//Emissive
        else if (FB_DebugMode == 9)
        {
            result.Color.rgb = emissive;
        }
        return result;
    }


	const float3 toEye = normalize(FB_CamPosition.xyz - worldPosition.xyz);
	const float3 specularColor = lerp((float3)0.04f, albedo.rgb, metalness);
	const float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metalness);

	const float ssaoValue = SSAOTexture.Sample(defaultSampler, input.UV).r;

	float3 lightResult;
	//Calculate directional lights
	for (int i = 0; i < SLB_PointStartIndex; i++)
	{
		float3 lightAmount = EvaluateDirectionalLight(
			diffuseColor,
			specularColor,
			pixelNormal,
			roughness,
			lights[i].LB_Color.rgb,
			lights[i].LB_Intensity,
			-lights[i].LB_Forward,
			toEye
		);

		if (lights[i].LB_CastShadows)
		{
			const float4x4 lightView = lights[i].LB_View;
			const float4x4 lightProj = lights[i].LB_Projection;
			const float4 worldToLightView = mul(lightView, worldPosition);
			const float4 lightViewToProj = mul(lightProj, worldToLightView);

			float2 projectedTexCoord;
			projectedTexCoord.x = lightViewToProj.x / lightViewToProj.w / 2.0f + 0.5f;
			projectedTexCoord.y = -lightViewToProj.y / lightViewToProj.w / 2.0f + 0.5f;
			
			const float shadowBias = 0.0001f;
			float shadow = 0.0f;
			const float viewDepth = (lightViewToProj.z / lightViewToProj.w) - shadowBias;
				
            float4 coords;
            coords.xy = projectedTexCoord;
            coords.z = viewDepth - shadowBias;
            coords.w = 1.0f;
				
            shadow = pcfShadow(coords, shadowMap, 2048.0f * 2, shadowSampler);
				
            float3 worldPos = worldPositionTexture.Sample(defaultSampler, input.UV).rgba;
			
            float worldScale = 0.02f;
            float timeScale = FB_TotalTime * 1.5f;
            float clouds = fbm((float3(worldPos + timeScale) * worldScale));
            float cloudsTweaked = 1 - saturate(clouds * 1.5f);
			
            lightAmount *= shadow;
            lightAmount *= cloudsTweaked;
        }

		lightResult += lightAmount * ssaoValue;
	}

	//Calculate point lights
	for (i = SLB_PointStartIndex; i < SLB_SpotStartIndex; i++)
	{
        float3 lightAmount = EvaluatePointLight(
			diffuseColor,
			specularColor,
			pixelNormal,
			roughness,
			lights[i].LB_Color.rgb,
			lights[i].LB_Intensity * 100,
			lights[i].LB_MaxRange,
			lights[i].LB_Pos,
			toEye,
			worldPosition.xyz
		);
		lightResult += lightAmount * ssaoValue;
    }
	

	//Calculate spot lights
	for (int i = SLB_SpotStartIndex; i < SLB_IBLStartIndex; i++)
	{
		lightResult += EvaluateSpotLight(
			diffuseColor.rgb,
			specularColor.rgb,
			pixelNormal.rgb,
			roughness,
			lights[i].LB_Color.rgb,
			lights[i].LB_Intensity,
			lights[i].LB_MaxRange,
			lights[i].LB_Pos.xyz,
			lights[i].LB_Forward.xyz,
			lights[i].LB_OuterRadius,
			lights[i].LB_InnerRadius,
			toEye.xyz,
			worldPosition.xyz
		);
	}

    lightResult = lightResult + emissive;
	result.LightResult = lightResult;

	//Calculate IBL light
	for (i = SLB_IBLStartIndex; i < SLB_LightsCount; i++)
	{
		float3 ambientLighting = EvaluateAmbience(
			environmentTexture,
			pixelNormal.rgb,
			vertexNormal.rgb,
			toEye.xyz,
			roughness,
			ao,
			diffuseColor.rgb,
			specularColor.rgb
		);
		ambientLighting *= lights[i].LB_Intensity * lights[i].LB_Color * lights[i].LB_Color;

		lightResult += ambientLighting * ssaoValue;
	}

	result.ScreenUV = input.UV;
	result.Color.rgb = lightResult;
	result.Color.a = 1;
	return result;
}