#include "../../../Catbox/Shaders/Particle/ParticleShaderStructs.hlsli"
#include "../../../Catbox/Shaders/Struct/ShaderStructs.hlsli"

ParticleVertexData main(ParticleVertexData input)
{
    ParticleVertexData result = input;
    input.Rotation = 0;
    
    return result;
}