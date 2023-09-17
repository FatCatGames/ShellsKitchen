#pragma once
#include "Assets\Shader.h"

class HighlightPS : public PixelShader
{
public:
	struct HighlightBufferData
	{
		float Highlight = 0;
		Vector3f Padding;
	} myHighlightData;

	HighlightPS();
	std::shared_ptr<PixelShader> MakeInstance() override;
	void SetResource() override;
	void SetData() override;
	void SaveData() override;
	void LoadData() override;

private:

	void RenderInProperties() override;
	Catbox::CBuffer<HighlightBufferData> myHighlightBufferData;

}; 