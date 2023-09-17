#include "stdafx.h"
#include "WindShaderVS.h"

WindShaderVS::WindShaderVS()
{
	ifstream vsFile;
	vsFile.open("Resources/BuiltIn/Shaders/WindShaderVS.cso", ios::binary);
	std::string vsData = { std::istreambuf_iterator<char>(vsFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);
	assert(!FAILED(result) && "Loading WindShaderVS vertex shader failed!");
	vsFile.close();
}

void WindShaderVS::RenderInProperties()
{
	ImGui::Separator();
	myWasEdited = false;
	myWasEdited |= Catbox::DragFloat("WindStrength", &myWindShaderVSData.WindStrength);
	myWasEdited |= Catbox::DragFloat("WindSpeed", &myWindShaderVSData.WindSpeed);
	myWasEdited |= Catbox::DragFloat("WindScale", &myWindShaderVSData.WindScale);
	myWasEdited |= Catbox::DragFloat("WindAngle", &myWindShaderVSData.WindAngle);
	//myWasEdited |= Catbox::InputInt("Columns", &myUVPannerData.columns);
	//myWasEdited |= Catbox::InputInt("Framerate", &myWaterDisplacementData.framerate);
	//Catbox::InputFloat("Start delay", &myWaterDisplacementData.startDelay);
	//Catbox::InputFloat("Stop delay", &myWaterDisplacementData.stopDelay);
}

void WindShaderVS::SetData()
{
	if ( myWindShaderVSDataBuffer.myBuffer != nullptr)
	{
		 myWindShaderVSDataBuffer.SetData(&myWindShaderVSData);
	}
}

void WindShaderVS::SaveData()
{
	auto wrapper = RapidJsonWrapper::GetInstance();

	wrapper->SaveValue<DataType::Float>("WindStrength", myWindShaderVSData.WindStrength);
	wrapper->SaveValue<DataType::Float>("WindSpeed", myWindShaderVSData.WindSpeed);
	wrapper->SaveValue<DataType::Float>("WindScale", myWindShaderVSData.WindScale);
	wrapper->SaveValue<DataType::Float>("WindAngle", myWindShaderVSData.WindAngle);
	//wrapper->SaveValue<DataType::Int>("Framerate", myWaterDisplacementData.framerate);
	//wrapper->SaveValue<DataType::Bool>("Loop", myWaterDisplacementData.loop);
}

void WindShaderVS::LoadData()
{
	auto wrapper = RapidJsonWrapper::GetInstance();
	auto& graphics = *wrapper->GetTarget();

	if (graphics.HasMember("Columns"))
	{
		myWindShaderVSData.WindStrength =  graphics["WindStrength"].GetFloat();
		myWindShaderVSData.WindSpeed  =  graphics["WindSpeed"].GetFloat();
		myWindShaderVSData.WindScale  =  graphics["WindScale"].GetFloat();
		myWindShaderVSData.WindAngle  =  graphics["WindAngle"].GetFloat();
		//myWaterDisplacementData.framerate = graphics["Framerate"].GetInt();
		//myWaterDisplacementData.loop = graphics["Loop"].GetBool();
	}
}

void WindShaderVS::SetResource()
{
	DX11::Context->VSSetConstantBuffers(10, 1, myWindShaderVSDataBuffer.GetAddress());
}

std::shared_ptr<VertexShader> WindShaderVS::MakeInstance()
{
	std::shared_ptr<WindShaderVS> returnShader = std::make_shared<WindShaderVS>();
	returnShader->vertexShader = this->vertexShader;
	returnShader->myWindShaderVSData = myWindShaderVSData;
	return returnShader;
}
