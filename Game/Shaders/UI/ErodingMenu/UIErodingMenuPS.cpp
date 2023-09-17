#include "Game.pch.h"
#include "UIErodingMenuPS.h"

UIErodingMenuPS::UIErodingMenuPS()
{
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/UIErodingMenuPS.cso", ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	assert(!FAILED(result) && "Loading UI ErodingMenu pixelshader failed!");
	psFile.close();
}

void UIErodingMenuPS::RenderInProperties()
{
	ImGui::Separator();
	myWasEdited = false;
	myWasEdited |= Catbox::DragFloat("Resource",            &myUIErodingMenuData.Resource        );
}

void UIErodingMenuPS::SetData()
{
	if (myUIErodingMenuBufferData.myBuffer != nullptr)
	{
		myUIErodingMenuBufferData.SetData(&myUIErodingMenuData);
	}
}


void UIErodingMenuPS::SetResource()
{
	DX11::Context->PSSetConstantBuffers(10, 1, myUIErodingMenuBufferData.GetAddress());
}

std::shared_ptr<PixelShader> UIErodingMenuPS::MakeInstance()
{
	std::shared_ptr<UIErodingMenuPS> returnShader = std::make_shared<UIErodingMenuPS>();
	returnShader->pixelShader = this->pixelShader;
	returnShader->myUIErodingMenuData = myUIErodingMenuData;
	return returnShader;
}
