#include "Game.pch.h"
#include "UIProgressBarPS.h"

UIProgressBarPS::UIProgressBarPS()
{
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/UIProgressBarPS.cso", ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	assert(!FAILED(result) && "Loading UI Progress pixelshader failed!");
	psFile.close();
}

void UIProgressBarPS::RenderInProperties()
{
	ImGui::Separator();
	myWasEdited = false;
	myWasEdited |= Catbox::DragFloat("Resource",            &myUIProgressData.Resource        );
}

void UIProgressBarPS::SetData()
{
	if (myUIProgressBufferData.myBuffer != nullptr)
	{
		myUIProgressBufferData.SetData(&myUIProgressData);
	}
}


void UIProgressBarPS::SetResource()
{
	DX11::Context->PSSetConstantBuffers(10, 1, myUIProgressBufferData.GetAddress());
}

std::shared_ptr<PixelShader> UIProgressBarPS::MakeInstance()
{
	std::shared_ptr<UIProgressBarPS> returnShader = std::make_shared<UIProgressBarPS>();
	returnShader->pixelShader = this->pixelShader;
	returnShader->myUIProgressData = myUIProgressData;
	return returnShader;
}
