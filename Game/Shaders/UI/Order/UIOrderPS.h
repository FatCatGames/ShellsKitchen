#pragma once
#include "Assets\Shader.h"

class UIOrderPS : public PixelShader
{
public:
	struct UIOrderBufferData
	{
		float Resource = 0;
		Vector3f Padding;
	} myUIOrderData;

	UIOrderPS();
	std::shared_ptr<PixelShader> MakeInstance() override;
	void SetResource() override;
	void SetData() override;

private:

	void RenderInProperties() override;
	Catbox::CBuffer<UIOrderBufferData> myUIOrderBufferData;

}; 