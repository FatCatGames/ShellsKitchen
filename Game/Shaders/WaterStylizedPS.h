#pragma once
#include "Assets\Shader.h"

class WaterStylized : public PixelShader
{
public:
	struct ColorTintData
	{
		Vector3f RGB = {0,0,0};
		float Alpha = 0.f;
		float Darkness = 0.f;
		/*float startDelay = 5;
		float stopDelay = 500;*/
		Vector3f padding;
		//alignas(4) bool loop = true;
		//Vector2i padding;
	} myColorTintData;

	WaterStylized();
	std::shared_ptr<PixelShader> MakeInstance() override;
	void SetResource() override;
	void SetData() override;
	void SaveData() override;
	void LoadData() override;

private:

	void RenderInProperties() override;
	Catbox::CBuffer<ColorTintData> myColorTintDataBuffer;
};