#pragma once
#include "Assets\Shader.h"

class WindShaderVS : public VertexShader
{
public:
	struct WindShaderVSData
	{
		float WindStrength = 0;
		float WindSpeed = 0;
		float WindScale = 0;
		float WindAngle = 0;
	} myWindShaderVSData;

	WindShaderVS();
	std::shared_ptr<VertexShader> MakeInstance() override;
	void SetResource() override;
	void SetData() override;
	void SaveData() override;
	void LoadData() override;

private:
	
	void RenderInProperties() override;
	Catbox::CBuffer<WindShaderVSData> myWindShaderVSDataBuffer;
};