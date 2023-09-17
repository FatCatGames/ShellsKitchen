#include "Game.pch.h"
#include "UIPointsPS.h"

UIPointsPS::UIPointsPS()
{
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/UIPointsPS.cso", ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	assert(!FAILED(result) && "Loading UI Points pixelshader failed!");
	psFile.close();
}

void UIPointsPS::RenderInProperties()
{
	//ImGui::Separator();
	//myWasEdited = false;
	//myWasEdited |= Catbox::DragFloat("Resource", &myUIProgressData.Resource);
}

void UIPointsPS::SetData()
{
	if (myUIPointsBufferData.myBuffer != nullptr)
	{
		myUIPointsBufferData.SetData(&myUIPointsData);
	}
}


void UIPointsPS::SetResource()
{
	DX11::Context->PSSetConstantBuffers(10, 1, myUIPointsBufferData.GetAddress());
}

std::shared_ptr<PixelShader> UIPointsPS::MakeInstance()
{
	std::shared_ptr<UIPointsPS> returnShader = std::make_shared<UIPointsPS>();
	returnShader->pixelShader = this->pixelShader;
	returnShader->myUIPointsData = myUIPointsData;
	return returnShader;
}
