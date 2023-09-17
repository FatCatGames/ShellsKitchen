#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "CommonUtilities/Matrix4x4.hpp"
#include "../Buffers/CBuffer.hpp"
#include "../Buffers/StructuredBuffer.hpp"
#include "../DXDeviceStates.h"
#include "Editor\Windows\SceneWindow.h"

using namespace Catbox;

class Buffers;
class ModelInstance;
class Light;

class ShadowRenderer
{
public:
	~ShadowRenderer();
	bool Initialize(std::shared_ptr<Buffers> aBuffers);
	void Render(const std::vector<ModelInstance*>& aModelList, std::vector<Light*>& aLightList);
	Texture* GetShadowTexture() { return myShadowTexture; }
	std::shared_ptr<DepthStencil> GetDepthStencil() const { return myDepthStencil; }

private:
	std::shared_ptr<Buffers> myBuffers;
	Texture* myShadowTexture;
	std::shared_ptr<DepthStencil> myDepthStencil;
	
};