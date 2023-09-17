#include "Game.pch.h"
#include "UITimerPS.h"

UITimerPS::UITimerPS()
{
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/UITimerPS.cso", ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	assert(!FAILED(result) && "Loading UI Timer pixelshader failed!");
	psFile.close();
}

void UITimerPS::RenderInProperties()
{
	ImGui::Separator();
	myWasEdited = false;
	myWasEdited |= Catbox::DragFloat("Resource",            &myUITimerData.Resource        );
}

void UITimerPS::SetData()
{
	if (myUITimerBufferData.myBuffer != nullptr)
	{
		myUITimerBufferData.SetData(&myUITimerData);
	}
}


void UITimerPS::SetResource()
{
	DX11::Context->PSSetConstantBuffers(10, 1, myUITimerBufferData.GetAddress());
}

std::shared_ptr<PixelShader> UITimerPS::MakeInstance()
{
	std::shared_ptr<UITimerPS> returnShader = std::make_shared<UITimerPS>();
	returnShader->pixelShader = this->pixelShader;
	returnShader->myUITimerData = myUITimerData;
	return returnShader;
}
