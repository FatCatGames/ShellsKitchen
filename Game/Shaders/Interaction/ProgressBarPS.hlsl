#include "../../../Catbox/Shaders/ShaderFunctions.hlsli"
#include "../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"



cbuffer ResourceInfoBuffer : register(b10)
{
    float Resource;
};

Texture2D albedoTexture : register(t0);

PixelOutput main(VertexToPixel input)
{
	PixelOutput result;

	float4 albedo = albedoTexture.Sample(defaultSampler, input.AlbedoUV).rgba;
	if (albedo.a < 0.05f) discard;

	float2 uv = input.AlbedoUV;
	float resource = Resource;
	float theBar = Rect(uv, 0, resource, 0, 1, 0.005);

	result.Color.rgba = albedo * MB_Color * theBar;
	return result;
}