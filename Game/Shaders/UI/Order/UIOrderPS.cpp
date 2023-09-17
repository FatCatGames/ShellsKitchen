#include "Game.pch.h"
#include "UIOrderPS.h"

UIOrderPS::UIOrderPS()
{
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/UIOrderPS.cso", ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	assert(!FAILED(result) && "Loading UI Order pixelshader failed!");
	psFile.close();
}

void UIOrderPS::RenderInProperties()
{
	ImGui::Separator();
	myWasEdited = false;
	myWasEdited |= Catbox::DragFloat("Resource",            &myUIOrderData.Resource        );
}

void UIOrderPS::SetData()
{
	if (myUIOrderBufferData.myBuffer != nullptr)
	{
		myUIOrderBufferData.SetData(&myUIOrderData);
	}
}


void UIOrderPS::SetResource()
{
	DX11::Context->PSSetConstantBuffers(10, 1, myUIOrderBufferData.GetAddress());
}

std::shared_ptr<PixelShader> UIOrderPS::MakeInstance()
{
	std::shared_ptr<UIOrderPS> returnShader = std::make_shared<UIOrderPS>();
	returnShader->pixelShader = this->pixelShader;
	returnShader->myUIOrderData = myUIOrderData;
	return returnShader;
}
