#pragma once
#include "Assets\Shader.h"

class UIProgressBarPS : public PixelShader
{
public:
	struct UIProgressBufferData
	{
		float Resource = 0;
		Vector3f Padding;
	} myUIProgressData;

	UIProgressBarPS();
	std::shared_ptr<PixelShader> MakeInstance() override;
	void SetResource() override;
	void SetData() override;

private:

	void RenderInProperties() override;
	Catbox::CBuffer<UIProgressBufferData> myUIProgressBufferData;

}; 