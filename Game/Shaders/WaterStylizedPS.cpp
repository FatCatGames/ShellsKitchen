#include "Game.pch.h"
#include "WaterStylizedPS.h"

WaterStylized::WaterStylized()
{
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/WaterStylizedPS.cso", ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	assert(!FAILED(result) && "Loading Water stylized PS shader failed!");
	psFile.close();
}

void WaterStylized::RenderInProperties()
{
	ImGui::Separator();
	myWasEdited = false;
	myWasEdited |= Catbox::DragFloat3("RGB", &myColorTintData.RGB, 0.01f, 0.f, 10.f);
	myWasEdited |= Catbox::DragFloat("Alpha", &myColorTintData.Alpha, 0.01f, 0.f, 1.f);
	myWasEdited |= Catbox::DragFloat("Darkness", &myColorTintData.Darkness, 0.01f, 0.f, 1.f);
}

void WaterStylized::SetData()
{
	if (myColorTintDataBuffer.myBuffer != nullptr)
	{
		myColorTintDataBuffer.SetData(&myColorTintData);
	}
}

void WaterStylized::SaveData()
{
	//
	//if (myWasEdited)
	//{
		//myColorTintData.RGB *= 0.01f;
		//myColorTintData.Alpha *= 0.01f;
		auto wrapper = RapidJsonWrapper::GetInstance();

		wrapper->SaveValue<DataType::Float>("Red", myColorTintData.RGB.x);
		wrapper->SaveValue<DataType::Float>("Green", myColorTintData.RGB.y);
		wrapper->SaveValue<DataType::Float>("Blue", myColorTintData.RGB.z);
		wrapper->SaveValue<DataType::Float>("Alpha", myColorTintData.Alpha);
		wrapper->SaveValue<DataType::Float>("Darkness", myColorTintData.Darkness);
	//}

	//wrapper->SaveValue<DataType::Int>("Framerate", myUVPannerData.framerate);
	//wrapper->SaveValue<DataType::Bool>("Loop", myUVPannerData.loop);
}

void WaterStylized::LoadData()
{
	auto wrapper = RapidJsonWrapper::GetInstance();
	auto& graphics = *wrapper->GetTarget();

	//if (graphics.HasMember("Columns"))
	//{
	Vector3f tempCol = {};
	tempCol.x = graphics["Red"].GetFloat();
	tempCol.y = graphics["Green"].GetFloat();
	tempCol.z = graphics["Blue"].GetFloat();
	myColorTintData.RGB = tempCol;
	myColorTintData.Alpha = graphics["Alpha"].GetFloat();
	myColorTintData.Darkness = graphics["Darkness"].GetFloat();
		//myUVPannerData.framerate = graphics["Framerate"].GetInt();
		//myUVPannerData.loop = graphics["Loop"].GetBool();
	//}
}

void WaterStylized::SetResource()
{
	DX11::Context->PSSetConstantBuffers(10, 1, myColorTintDataBuffer.GetAddress());
}

std::shared_ptr<PixelShader> WaterStylized::MakeInstance()
{
	std::shared_ptr<WaterStylized> returnShader = std::make_shared<WaterStylized>();
	returnShader->pixelShader = this->pixelShader;
	returnShader->myColorTintData = myColorTintData;
	return returnShader;
}
