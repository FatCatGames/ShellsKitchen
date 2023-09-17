#include "ParticleShaderStructs.hlsli"
#include "../Struct/ShaderStructs.hlsli"

ParticleVertexData main(ParticleVertexData input)
{
    ParticleVertexData result = input;
    
    if (result.Position.y > -4.5)
    {
        result.Position.y = -4.5;
    }
    
    else
        result.Position.y = result.Position.y;
        

    return result;
}