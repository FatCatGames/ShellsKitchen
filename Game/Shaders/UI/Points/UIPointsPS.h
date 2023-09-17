#pragma once
#include "Assets\Shader.h"

class UIPointsPS : public PixelShader
{
public:
	struct UIPointsBufferData
	{
		int Points = 0;
		float Driver = 0.0f;
		Vector2f Padding;
	} myUIPointsData;

	UIPointsPS();
	std::shared_ptr<PixelShader> MakeInstance() override;
	void SetResource() override;
	void SetData() override;

private:

	void RenderInProperties() override;
	Catbox::CBuffer<UIPointsBufferData> myUIPointsBufferData;

}; 