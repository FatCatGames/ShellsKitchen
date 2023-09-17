#include "../Struct/ShaderStructs.hlsli"
#include "../PBRFunctions.hlsli"
#include "../ShaderFunctions.hlsli"
#include "../Color/ColorConversion.hlsli"

Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D pbrTexture : register(t2);
Texture2D ambientOcclusionTexture : register(t5);
Texture2D ScreenUVTexture : register(t6);
Texture2D shadowMap : register(t7);
Texture2D SSAOTexture : register(t8);
StructuredBuffer<LightData> lights : register(t9);
TextureCube environmentTexture : register(t10);

cbuffer ColorTint : register(b10)
{
    float3 RGB = float3(0, 0, 0);
    float alpha = 0.f;
    float darkness = 1.f;
};


PixelOutput main(VertexToPixel input)
{
    PixelOutput result;

    //float2 WorldSpaceUV = (input.WorldPosition.xz / 10.f);
    
    //float4 shadowNoise = float4(SimplexNoise(float2((input.WorldPosition.x * 0.15 + (FB_DeltaTime * 0.09)), input.WorldPosition.z * 0.15 - (FB_TotalTime * 0.19))), 1, 1, 1);
    
    float tiling1 = 0.15f;
    float tiling2 = 0.25f;
    
    float valueNoiseTiling = MB_CustomFloat4.x;
    
    float UVtiling = 5;
    
    float valueNoise = float(SimplexNoise(float2(((input.WorldPosition.x * valueNoiseTiling) + (FB_DeltaTime * 0.09)), (input.WorldPosition.z * valueNoiseTiling) - (FB_TotalTime * 0.19))));
    
    float4 noiseX = float4(SimplexNoise(float2((input.WorldPosition.x * tiling1 + (FB_DeltaTime * 0.09)), input.WorldPosition.z * tiling1 - (FB_TotalTime * 0.19))), 1, 1, 0);
    float4 noiseY = float4(SimplexNoise(float2((input.WorldPosition.x * tiling2 - (FB_TotalTime * 0.1)), input.WorldPosition.z * tiling2 + (FB_TotalTime * 0.16))), 1, 1, 0);
    float4 noiseZ = float4(1, 1, SimplexNoise(float2((input.WorldPosition.z * tiling1 - (FB_TotalTime * 0.1)), input.WorldPosition.x * tiling1 - (FB_TotalTime * 0.16))), 0);
    float4 noiseW = float4(1, 1, SimplexNoise(float2((input.WorldPosition.z * tiling2 + (FB_TotalTime * 0.1)), input.WorldPosition.x * tiling2 + (FB_TotalTime * 0.16))), 0);
    
    float4 noise2 = lerp(noiseX, noiseY, 0.5);
    float4 noise3 = lerp(noiseZ, noiseW, 0.5);
    
    float4 noise = lerp(noise2, noise3, 0.5) * 0.5f;
    
    float3 causticsNoise = CowSticks(input.WorldPosition.xyz * 0.0008f, FB_TotalTime, input.Normal.xyz);
    
    float2 NoiseUV = CowSticks(float3(input.WorldPosition.xz * 0.0008f, 0), FB_TotalTime, 1);
    
    //float2 panningUV = float2((input.AlbedoUV.x * 3) + FB_TotalTime * 0.01f, (input.AlbedoUV.y * 3) + FB_TotalTime * 0.01f);
    //float2 panningUV2 = float2((input.AlbedoUV.x * 3) - FB_TotalTime * 0.01f, (input.AlbedoUV.y * 3) + FB_TotalTime * 0.01f);
    
    float2 panningUV = float2((input.AlbedoUV.x * UVtiling) + FB_TotalTime * 0.01f, (input.AlbedoUV.y * UVtiling) + FB_TotalTime * 0.01f);
    float2 panningUV2 = float2((input.AlbedoUV.x * UVtiling) - FB_TotalTime * 0.01f, (input.AlbedoUV.y * UVtiling) + FB_TotalTime * 0.01f);
    float2 panningUV3 = float2((input.AlbedoUV.x * UVtiling) - FB_TotalTime * 0.01f, (input.AlbedoUV.y * UVtiling) - FB_TotalTime * 0.01f);
    
    float2 resolutionUV = FB_Resolution;
    
    float4x4 mvp_matrix = mul(mul(FB_ToProjection, FB_ToView), OB_ToWorld);
    
    //float2 UV = mul(mvp_matrix,input.AlbedoUV);
    
    /*
    float resolutionClampedX = RemapClamp(0, 1080, 0, 1, resolutionUV.x);
    float resolutionClampedY = RemapClamp(0, 1920, 0, 1, resolutionUV.y);
    
    float2 resolutionResult = float2(resolutionClampedX, resolutionClampedY);
    */
    
    float4 albedo = albedoTexture.Sample(wrapSampler, panningUV).rgba;
    float4 albedo2 = albedoTexture.Sample(wrapSampler, panningUV2).rgba;
    float4 albedo3 = albedoTexture.Sample(wrapSampler, panningUV3).rgba;
    
    //float4 albedo = saturate(albedo1 + albedo3);
    
    float2 screen = float2(ScreenUVTexture.Sample(defaultSampler, input.AlbedoUV).rg);
    //float4 depth = DepthTex.Sample(defaultSampler, screen.xy).rgba;
  
    albedo.rgb = GammaToLinear(albedo.rgb);
    albedo = albedo.rgba * MB_Color;

    if (albedo.a < 0.05f)
        discard;


    //depth.rgb = LogDepthToLinDepth(depth.r, FB_FarPlane, FB_NearPlane).rrr;
    float depth = GetLinDepth(input.WorldPosition, FB_ToView, FB_ToProjection, 500, 7.87f);
    //result.Color.a = 1.0f;
    //return result;
    
	//tangent space matrix
    const float3x3 TBN = float3x3(
		normalize(input.Tangent),
		normalize(input.Binormal),
		normalize(input.Normal));

	//R = 0
	//G = normal Y
	//B = AO
	//A = normal X
    float3 normalMap = normalTexture.Sample(wrapSampler, NoiseUV).agb;
    const float ao = normalMap.b;
	
    float3 pixelNormal = normalMap;
    pixelNormal = 1-pixelNormal;
    pixelNormal = 2.0f * pixelNormal - 1;
    pixelNormal.z = sqrt(normalize(dot(pixelNormal.xy, pixelNormal.xy)));
    pixelNormal = normalize(pixelNormal);
    pixelNormal = normalize(mul(pixelNormal, TBN));

    float4 pbr = pbrTexture.Sample(wrapSampler, panningUV);
    const float metalness = 0;
    const float roughness = 0.55f;
    const float emissive = pbr.b;
    const float emissiveStr = pbr.a;
	//const float3 vertexNormal = vertexNormalTexture.Sample(defaultSampler, input.).rgb;

    const float3 toEye = normalize(FB_CamPosition.xyz - input.WorldPosition.xyz);
    const float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metalness);
    const float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metalness);

    float dotProduct = dot(toEye, input.Normal);
    
    //float2 screenUV = ScreenUVTexture.Sample(defaultSampler, input.AlbedoUV).rg;
    const float ssaoValue = SSAOTexture.Sample(wrapSampler, screen).r;

    result.id = OB_id;

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
            result.Color = input.VxColor * MB_Color;
        }
		//vertex normals
        else if (FB_DebugMode == 3)
        {
			//float3 debugNormal = pixelNormal;
            float3 debugNormal = input.Normal;
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
            const float3 normalMap = normalTexture.Sample(defaultSampler, input.AlbedoUV).agb;
            result.Color = float4(normalMap.r, normalMap.g, 1, 1);
        }
		//UVs
        else if (FB_DebugMode == 5)
        {
            result.Color = float4(input.AlbedoUV.r, input.AlbedoUV.g, 1, 1);
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
            result.Color.rgb = emissive * emissiveStr;
        }
        return result;
    }

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
            const float4 worldToLightView = mul(lightView, input.WorldPosition);
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
				
            ////////////////// CLOUDS
    
            float3 worldPos = input.WorldPosition.xyz;
            
            float worldScale = 0.02f;
            float timeScale = FB_TotalTime * 1.5f;
            float clouds = fbm((float3(worldPos + timeScale) * worldScale));
            float cloudsTweaked = 1 - saturate(clouds * 0.5f);
			
            lightAmount *= shadow;
            lightAmount *= cloudsTweaked;
        }

        lightResult += lightAmount * ssaoValue;
    }

	//Calculate point lights
    for (i = SLB_PointStartIndex; i < SLB_SpotStartIndex; i++)
    {
		//PBR TGA VER

        lightResult += EvaluatePointLight(
			diffuseColor,
			specularColor,
			pixelNormal,
			roughness,
			lights[i].LB_Color.rgb,
			lights[i].LB_Intensity * 100,
			lights[i].LB_MaxRange,
			lights[i].LB_Pos,
			toEye,
			input.WorldPosition.xyz
		);
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
			input.WorldPosition.xyz
		);
    }


	//Calculate IBL light
    for (i = SLB_IBLStartIndex; i < SLB_LightsCount; i++)
    {
        float3 ambientLighting = EvaluateAmbience(
			environmentTexture,
			pixelNormal.rgb,
			input.Normal.rgb,
			toEye.xyz,
			roughness,
			ao,
			diffuseColor.rgb,
			specularColor.rgb
		);
        ambientLighting *= lights[i].LB_Intensity * lights[i].LB_Color ;

        lightResult += ambientLighting * ssaoValue;
    }
    
    float3 cubeMapTexture = EvaluateAmbience(
			environmentTexture,
			pixelNormal.rgb,
			normalMap,
			toEye.xyz,
			roughness,
			ao,
			diffuseColor.rgb*5,
			specularColor.rgb
		);
    
    float3 emissiveResult = albedo.rgb * emissive * emissiveStr * MB_EmissiveStrength;
    lightResult += emissiveResult;
    
    float3 lightResultwReflection = lerp(lightResult, 1, pow(dotProduct * 0.55f, 2));
    
    //result.Color.rgb = DepthTex.Sample(defaultSampler, screen.rg);
    //result.Color.rgb = RemapClamp(0.98f, 1.0f, 0, 1, result.Color.r);
    
    float3 combinedWavesColor = saturate(albedo.rgb);
    float combinedWavesAlpha = albedo.a;
    
    float3 tint1 = float3(RGB.r, RGB.g, RGB.b);
    float3 tint2 = MB_Color;
    
    float3 tintLerp = lerp(tint1, tint2, dotProduct);
    
    float alphaAmount = alpha;

    float3 whiteLines = lerp(depth * saturate(MB_Color - darkness), 1, albedo.a);
    float3 darkerLines = lerp(0.9f, 0.1f, albedo2.a);
    
    float3 whiteAndDarkLines = lerp(darkerLines, whiteLines , 0.55f);
 
    float3 Water = (abs(lerp(cubeMapTexture, whiteAndDarkLines + lightResult, dotProduct)) * tintLerp + whiteLines);
    //result.Color.rgb = clouds;
    result.Color.rgb = Water;

    //result.Color.a = lerp(0.05f, depth * 2, dotProduct*0.55f) ;
    result.Color.a = saturate((abs(1 - dotProduct) + albedo.a) + alphaAmount);
    //result.Color.a = lerp(1, (depth * 0.001f) + albedo.a, dotProduct);
    //result.Color.a = lerp(albedo.a + 0.5f, pow(albedo.a, 5), pow(dotProduct * 1, 2)) + 0.25f;
    result.WorldPosition = input.WorldPosition;
    result.LightResult = lightResult;

    return result;
}