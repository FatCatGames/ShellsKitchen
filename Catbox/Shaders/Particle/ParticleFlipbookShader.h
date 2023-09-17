#pragma once
#include "Assets\Shader.h"

class ParticleFlipbookShader : public PixelShader
{
public:
	struct FlipbookBufferData
	{
		int rows = 8;
		int columns = 8;
		int framerate = 24;
		/*float startDelay = 5;
		float stopDelay = 500;*/

		alignas(4) bool loop = true;
		//Vector2i padding;
	} myFlipbookData;

	ParticleFlipbookShader();
	std::shared_ptr<PixelShader> MakeInstance() override;
	void SetResource() override;
	void SetData() override;
	void SaveData() override;
	void LoadData() override;

private:

	void RenderInProperties() override;
	Catbox::CBuffer<FlipbookBufferData> myFlipbookDataBuffer;
};