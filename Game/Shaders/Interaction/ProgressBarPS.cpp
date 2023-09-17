#include "Game.pch.h"
#include "ProgressBarPS.h"

ProgressBarPS::ProgressBarPS()
{
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/ProgressBarPS.cso", ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	assert(!FAILED(result) && "Loading Progress pixelshader failed!");
	psFile.close();
}

void ProgressBarPS::RenderInProperties()
{
	ImGui::Separator();
	myWasEdited = false;
	myWasEdited |= Catbox::DragFloat("Resource",            &myProgressData.Resource        );
}

void ProgressBarPS::SetData()
{
	if (myProgressBarBufferData.myBuffer != nullptr)
	{
		myProgressBarBufferData.SetData(&myProgressData);
	}
}


void ProgressBarPS::SetResource()
{
	DX11::Context->PSSetConstantBuffers(10, 1, myProgressBarBufferData.GetAddress());
}

std::shared_ptr<PixelShader> ProgressBarPS::MakeInstance()
{
	std::shared_ptr<ProgressBarPS> returnShader = std::make_shared<ProgressBarPS>();
	returnShader->pixelShader = this->pixelShader;
	returnShader->myProgressData = myProgressData;
	return returnShader;
}
