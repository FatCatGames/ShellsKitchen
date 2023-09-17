#pragma once
#include "Assets\Shader.h"

class ProgressBarPS : public PixelShader
{
public:
	struct ProgressBarData
	{
		float Resource = 0;
		Vector3f Padding;
	} myProgressData;

	ProgressBarPS();
	std::shared_ptr<PixelShader> MakeInstance() override;
	void SetResource() override;
	void SetData() override;

private:

	void RenderInProperties() override;
	Catbox::CBuffer<ProgressBarData> myProgressBarBufferData;

}; 