#pragma once
#include "Assets\Shader.h"

class UICookingProgressBarPS : public PixelShader
{
public:
	struct UICookingProgressBufferData
	{
		float Resource = 0;
		Vector3f Padding;
	} myUICookingProgressData;

	UICookingProgressBarPS();
	std::shared_ptr<PixelShader> MakeInstance() override;
	void SetResource() override;
	void SetData() override;

private:

	void RenderInProperties() override;
	Catbox::CBuffer<UICookingProgressBufferData> myUICookingProgressBufferData;

}; 