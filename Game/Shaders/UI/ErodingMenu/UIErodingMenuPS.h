#pragma once
#include "Assets\Shader.h"

class UIErodingMenuPS : public PixelShader
{
public:
	struct UIErodingMenuBufferData
	{
		float Resource = 1;
		Vector3f Padding;
	} myUIErodingMenuData;

	UIErodingMenuPS();
	std::shared_ptr<PixelShader> MakeInstance() override;
	void SetResource() override;
	void SetData() override;

private:

	void RenderInProperties() override;
	Catbox::CBuffer<UIErodingMenuBufferData> myUIErodingMenuBufferData;

}; 