#pragma once
#include "Assets\Shader.h"

class UICounterPS : public PixelShader
{
public:
	struct UICounterBufferData
	{
		float Driver = 0.0f;    
		int TotalPoints = 0; 
		int Points = 0;      
		float Padding;
	} myUICounterData;

	UICounterPS();
	std::shared_ptr<PixelShader> MakeInstance() override;
	void SetResource() override;
	void SetData() override;

private:

	void RenderInProperties() override;
	Catbox::CBuffer<UICounterBufferData> myUICounterBufferData;

}; 