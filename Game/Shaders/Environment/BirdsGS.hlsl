#include "../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"
#include "../../../Catbox/Shaders/Particle/ParticleShaderStructs.hlsli"
#include "../../../Catbox/Shaders/ShaderFunctions.hlsli"

[maxvertexcount(4)]
void main(
	point ParticleVertexData input[1],
	inout TriangleStream<ParticleGeometryToPixel> output
)
{
    const float2 offsets[4] =
    {
        { -1, 1 },
        { 1, 1 },
        { -1, -1 },
        { 1, -1 }
    };

    const float2 uvs[4] =
    {
        { 0.f, 0.f },
        { 1.f, 0.f },
        { 0.f, 1.f },
        { 1.f, 1.f }
    };

    const ParticleVertexData inputParticle = input[0];
    for (unsigned int index = 0; index < 4; ++index)
    {
        
        float defaultScale = 1;

        float time = FB_TotalTime;
        
        float flaps = (sin(time * 4 * (2 - ((inputParticle.Position.y+1) * 0.01)))) + (1 * 0.5f);
        
        //float flaps = time * inputParticle.Lifetime * 0.1f;
        
        if (inputParticle.Lifetime > 10)
            flaps = 1;
  
        ParticleGeometryToPixel result;
        result.Position = mul(FB_ToView, inputParticle.Position);
        result.Position.xy += offsets[index] * float2(inputParticle.Scale.x, inputParticle.Scale.y * flaps + 1) * -1;
        result.Position = mul(FB_ToProjection, result.Position);
        result.Color = inputParticle.Color;
        result.Lifetime = inputParticle.Lifetime;
        result.UV = RotateUV(uvs[index], inputParticle.Rotation);
        result.ParticleID = inputParticle.ParticleID;
        output.Append(result);
    }
}