#include "Game.pch.h"
#include "HighlightPS.h"

HighlightPS::HighlightPS()
{
	ifstream psFile;
	psFile.open("Resources/BuiltIn/Shaders/HighlightPS.cso", ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), istreambuf_iterator<char>() };
	HRESULT result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	assert(!FAILED(result) && "Loading Highlight pixelshader failed!");
	psFile.close();
}

void HighlightPS::RenderInProperties()
{
	ImGui::Separator();
	myWasEdited = false;
	myWasEdited |= Catbox::DragFloat("Highlight",            &myHighlightData.Highlight        );
}

void HighlightPS::SetData()
{
	if (myHighlightBufferData.myBuffer != nullptr)
	{
		myHighlightBufferData.SetData(&myHighlightData);
		DX11::Context->PSSetConstantBuffers(10, 1, myHighlightBufferData.GetAddress());
	}
}

void HighlightPS::SaveData()
{
	auto wrapper = RapidJsonWrapper::GetInstance();

	wrapper->SaveValue<DataType::Float>("Highlight",   myHighlightData.Highlight);
}

void HighlightPS::LoadData()
{
	auto wrapper = RapidJsonWrapper::GetInstance();
	auto& graphics = *wrapper->GetTarget();

	if (graphics.HasMember("Highlight"))
	{
		myHighlightData.Highlight = graphics["Highlight"].GetFloat();
	}

}

void HighlightPS::SetResource()
{
	DX11::Context->PSSetConstantBuffers(10, 1, myHighlightBufferData.GetAddress());
}

std::shared_ptr<PixelShader> HighlightPS::MakeInstance()
{
	std::shared_ptr<HighlightPS> returnShader = std::make_shared<HighlightPS>();
	returnShader->pixelShader = this->pixelShader;
	returnShader->myHighlightData = myHighlightData;
	return returnShader;
}
