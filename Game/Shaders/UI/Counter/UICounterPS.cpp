#include "Game.pch.h"
#include "UICounterPS.h"

UICounterPS::UICounterPS()
{
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/UICounterPS.cso", ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	assert(!FAILED(result) && "Loading UI Counter pixelshader failed!");
	psFile.close();
}

void UICounterPS::RenderInProperties()
{
	ImGui::Separator();
	myWasEdited = false;
	//myWasEdited |= Catbox::DragFloat("Resource",            &myUICounterData.Resource        );
}

void UICounterPS::SetData()
{
	if (myUICounterBufferData.myBuffer != nullptr)
	{
		myUICounterBufferData.SetData(&myUICounterData);
	}
}


void UICounterPS::SetResource()
{
	DX11::Context->PSSetConstantBuffers(10, 1, myUICounterBufferData.GetAddress());
}

std::shared_ptr<PixelShader> UICounterPS::MakeInstance()
{
	std::shared_ptr<UICounterPS> returnShader = std::make_shared<UICounterPS>();
	returnShader->pixelShader = this->pixelShader;
	returnShader->myUICounterData = myUICounterData;
	return returnShader;
}
