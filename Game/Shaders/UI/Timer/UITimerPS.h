#pragma once
#include "Assets\Shader.h"

class UITimerPS : public PixelShader
{
public:
	struct UITimerBufferData
	{
		float Resource = 0;
		Vector3f Padding;
	} myUITimerData;

	UITimerPS();
	std::shared_ptr<PixelShader> MakeInstance() override;
	void SetResource() override;
	void SetData() override;

private:

	void RenderInProperties() override;
	Catbox::CBuffer<UITimerBufferData> myUITimerBufferData;

}; 