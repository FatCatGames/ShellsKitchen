#include "Game.pch.h"
#include "UICookingProgressBarPS.h"

UICookingProgressBarPS::UICookingProgressBarPS()
{
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/UICookingProgressBarPS.cso", ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	assert(!FAILED(result) && "Loading UI Cooking Progress Bar pixelshader failed!");
	psFile.close();
}

void UICookingProgressBarPS::RenderInProperties()
{
	ImGui::Separator();
	myWasEdited = false;
	myWasEdited |= Catbox::DragFloat("Resource",            &myUICookingProgressData.Resource        );
}

void UICookingProgressBarPS::SetData()
{
	if (myUICookingProgressBufferData.myBuffer != nullptr)
	{
		myUICookingProgressBufferData.SetData(&myUICookingProgressData);
	}
}


void UICookingProgressBarPS::SetResource()
{
	DX11::Context->PSSetConstantBuffers(10, 1, myUICookingProgressBufferData.GetAddress());
}

std::shared_ptr<PixelShader> UICookingProgressBarPS::MakeInstance()
{
	std::shared_ptr<UICookingProgressBarPS> returnShader = std::make_shared<UICookingProgressBarPS>();
	returnShader->pixelShader = this->pixelShader;
	returnShader->myUICookingProgressData = myUICookingProgressData;
	return returnShader;
}
