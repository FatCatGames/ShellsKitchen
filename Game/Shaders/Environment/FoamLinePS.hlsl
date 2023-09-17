#include "../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"
#include "../../../Catbox/Shaders/PBRFunctions.hlsli"
#include "../../../Catbox/Shaders/ShaderFunctions.hlsli"


Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D pbrTexture : register(t2);
Texture2D ambientOcclusionTexture : register(t5);
Texture2D ScreenUVTexture : register(t6);
Texture2D shadowMap : register(t7);
Texture2D SSAOTexture : register(t8);
StructuredBuffer<LightData> lights : register(t9);
TextureCube environmentTexture : register(t10);
Texture2D depthTexture : register(t11);



PixelOutput main(VertexToPixel input)
{
	PixelOutput result;
	
	
	
    float objectScale = (((OB_Scale.x + OB_Scale.y + OB_Scale.z) * 0.33f) * -0.25f)*MB_CustomFloat4.x;
    //float objectScale = 15;
	
    float2 panningUV = float2((input.AlbedoUV.x * 3) + FB_TotalTime * 0.1f, (input.AlbedoUV.y * 3) + FB_TotalTime * 0.1f);
	
    float4 noiseX = float4(SimplexNoise(float2(((input.WorldPosition.x * 2.15f / objectScale) + (FB_DeltaTime * 1.19f)), (input.WorldPosition.z * 2.15f / objectScale) - (FB_TotalTime * 0.19f))), 1, 1, 0);
    float4 noiseY = float4(SimplexNoise(float2(((input.WorldPosition.x * 1.25f / objectScale) - (FB_TotalTime * -0.35f)), (input.WorldPosition.z * 2.25f / objectScale) + (FB_TotalTime * 0.36f))), 1, 1, 0);
    float4 noiseZ = float4(1, 1, SimplexNoise(float2(((input.WorldPosition.z * 2.15f / objectScale) - (FB_TotalTime * 0.3f)), (input.WorldPosition.x * 2.15f / objectScale) + (FB_TotalTime * 0.56f))), 0);
    float4 noiseW = float4(1, 1, SimplexNoise(float2(((input.WorldPosition.z * 1.25f / objectScale) - (FB_TotalTime * 0.55f)), (input.WorldPosition.x * 4.25f / objectScale) - (FB_TotalTime * 0.56f))), 0);
    
    float4 noise2 = lerp(noiseX, noiseY, 0.5);
    float4 noise3 = lerp(noiseZ, noiseW, 0.5);
    
    float4 noise = lerp(noise2, noise3, 0.5) * 0.5f;
	
	float4 albedo = albedoTexture.Sample(wrapSampler, input.AlbedoUV).rgba;
	albedo.rgb = GammaToLinear(albedo.rgb);
	albedo = albedo.rgba * MB_Color;
	if (albedo.a < 0.2f) discard;

	//tangent space matrix
	const float3x3 TBN = float3x3(
		normalize(input.Tangent),
		normalize(input.Binormal),
		normalize(input.Normal));

	//R = 0
	//G = normal Y
	//B = AO
	//A = normal X
    float3 normalMap = normalTexture.Sample(defaultSampler, input.AlbedoUV).agb;
	const float ao = normalMap.b;
	
	float3 pixelNormal = normalMap;
	pixelNormal = 2.0f * pixelNormal - 1;
	pixelNormal.z = sqrt(normalize(dot(pixelNormal.xy, pixelNormal.xy)));
	pixelNormal = normalize(pixelNormal);
	pixelNormal = normalize(mul(pixelNormal, TBN));

	float4 pbr = pbrTexture.Sample(defaultSampler, input.AlbedoUV);
	const float metalness = pbr.r;
	const float roughness = pbr.g;
	const float emissive = pbr.b;
	const float emissiveStr = pbr.a;
	//const float3 vertexNormal = vertexNormalTexture.Sample(defaultSampler, input.).rgb;

	const float3 toEye = normalize(FB_CamPosition.xyz - input.WorldPosition.xyz);
	const float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metalness);
	const float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metalness);

	float2 screenUV = ScreenUVTexture.Sample(defaultSampler, input.AlbedoUV).rg;
	const float ssaoValue = SSAOTexture.Sample(defaultSampler, screenUV).r;

	result.id = OB_id;

	if (FB_DebugMode != 0)
	{
		result.Color.a = 1;
		//albedo
		if (FB_DebugMode == 1)
        {
            result.Color.rgb = (1,1,1,1);
			//result.Color = albedo;
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
        else if (FB_DebugMode == 11)
        {
            result.Color = albedo;
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

            shadow = pcfShadow(coords, shadowMap, 2048.0f, shadowSampler);

            float worldScale = 0.02f;
            float timeScale = FB_TotalTime * 1.5f;
            float clouds = fbm((float3(input.WorldPosition.xyz + timeScale) * worldScale));
            float cloudsTweaked = 1 - saturate(clouds);
			
            lightAmount *= shadow;
            
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
		ambientLighting *= lights[i].LB_Intensity * lights[i].LB_Color * lights[i].LB_Color;
		lightResult += ambientLighting * ssaoValue;
	}

    float2 SwirlMask = SwirlUV(input.AlbedoUV, 0, 0.1f, 2);
	
    //float circleMask = pow((input.AlbedoUV.x * input.AlbedoUV.y), 20);
    //float circleMask = abs(sin(pow(input.AlbedoUV.x + 0.5f, 5)));
	
    float2 transformUV = float2(input.AlbedoUV.x - 0.5f, input.AlbedoUV.y - 0.5f);
	
	//foam line length - 0.15f to 0.35f
	
    float4 foamNoise = float4(noise.xyz, 0);
	
	
    float2 circleMask = Circle(transformUV * foamNoise.y, 0.35f * foamNoise + 0.05f, 0.001f);
	//float circleMask = UVFadeInAndOut(input.AlbedoUV.x, 1);
	
	float3 emissiveResult = albedo.rgb * emissive * emissiveStr * MB_EmissiveStrength;
	lightResult += emissiveResult;
    result.Color.rgb = lightResult;
    result.Color.a = circleMask;
	//result.Color.a = albedo.a;
	result.WorldPosition = input.WorldPosition;
	result.LightResult = lightResult;

	return result;
}