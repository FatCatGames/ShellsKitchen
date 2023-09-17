#include "Struct/ShaderStructs.hlsli"
#include "ShaderFunctions.hlsli"

struct CopyPixelOutput
{
	float4 Color : SV_TARGET0;
};

Texture2D sourceTexture : register(t0);

CopyPixelOutput main(DeferredVertexToPixel input)
{
	CopyPixelOutput result;
    float4 pixel = sourceTexture.Sample(defaultSampler, input.UV);
    pixel.rgb = LogDepthToLinDepth(pixel.r, FB_FarPlane, FB_NearPlane).rrr;
	//if (pixel.a < 0.05f) discard;
	result.Color.rgb = pixel.rgb;
    result.Color.a = 1.0f;
	return result;
}