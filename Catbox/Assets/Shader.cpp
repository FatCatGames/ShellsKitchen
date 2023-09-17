#include "stdafx.h"
#include "Shader.h"
#include <d3dcompiler.h>

bool RenderCBInt(const std::string& aName, int* aInt)
{
	ImGui::Text(aName.c_str());
	ImGui::SameLine();
	ImGui::Text(" : ");
	ImGui::SameLine();
	return Catbox::InputInt(("##" + aName).c_str(), aInt);
}
bool RenderCBFloat(const std::string& aName, float* aFloat)
{
	ImGui::Text(aName.c_str());
	ImGui::SameLine();
	ImGui::Text(" : ");
	ImGui::SameLine();
	return Catbox::DragFloat(("##" + aName).c_str(), aFloat, 0.01f);
}
bool RenderCBFloat2(const std::string& aName, Vector2f* aVector2)
{
	ImGui::Text(aName.c_str());
	ImGui::SameLine();
	ImGui::Text(" : ");
	ImGui::SameLine();
	if (Catbox::DragFloat(("##" + aName + "x").c_str(), &aVector2->x)) 
	{
		return true;
	}
	ImGui::SameLine();
	if (Catbox::DragFloat(("##" + aName + "y").c_str(), &aVector2->y)) 
	{
		return true;
	}
	return false;
}
bool RenderCBFloat3(const std::string& aName, Vector3f* aVector3)
{
	ImGui::Text(aName.c_str());
	ImGui::SameLine();
	ImGui::Text(" : ");
	ImGui::SameLine();
	return Catbox::DragFloat3(("##" + aName).c_str(), aVector3);
}
bool RenderCBFloat4(const std::string& aName, Vector4f* aVector3)
{
	ImGui::Text(aName.c_str());
	ImGui::SameLine();
	ImGui::Text(" : ");
	ImGui::SameLine();
	return Catbox::DragFloat4(("##" + aName).c_str(), aVector3);
}
bool RenderCBBool(const std::string& aName, bool* aFlag)
{
	ImGui::Text(aName.c_str());
	ImGui::SameLine();
	ImGui::Text(" : ");
	ImGui::SameLine();
	return ImGui::Checkbox(("##" + aName).c_str(), aFlag);
}

void VertexShader::RenderInProperties()
{
	char nameInput[32] = "";
	for (size_t i = 0; i < myName.size(); i++)
	{
		nameInput[i] = myName[i];
	}

	//std::string emptyText = "##VertexShaderNameInput";
	//if (ImGui::InputText(emptyText.c_str(), nameInput, IM_ARRAYSIZE(nameInput)))
	//{
	//	myName = nameInput;
	//}
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		AssetRegistry::GetInstance()->RenameAsset(this, myName);
	}

}
void PixelShader::RenderInProperties()
{
	char nameInput[32] = "";
	for (size_t i = 0; i < myName.size(); i++)
	{
		nameInput[i] = myName[i];
	}

	//std::string emptyText = "##PixelShaderNameInput";
	//if (ImGui::InputText(emptyText.c_str(), nameInput, IM_ARRAYSIZE(nameInput)))
	//{
	//	myName = nameInput;
	//}
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		AssetRegistry::GetInstance()->RenameAsset(this, myName);
	}

	for (auto buffer : myConstantBuffers)
	{
		for (auto variable : myConstantBufferValues[buffer.first])
		{
			D3D11_SHADER_TYPE_DESC varDesc = myConstantBufferValueTypes[buffer.first][variable.first];
			bool edited = false;
			switch (varDesc.Type)
			{
			case D3D_SVT_BOOL: 
			{
				edited = RenderCBBool(variable.first, static_cast<bool*>(variable.second));
				break;
			}
			case D3D_SVT_INT:
			{
				edited = RenderCBInt(variable.first, static_cast<int*>(variable.second));
				break;
			}
			case D3D_SVT_FLOAT:
			{
				if (varDesc.Rows == 4)
				{
					edited = RenderCBFloat4(variable.first, static_cast<Vector4f*>(variable.second));
				}
				else if (varDesc.Rows == 3)
				{
					edited = RenderCBFloat3(variable.first, static_cast<Vector3f*>(variable.second));
				}
				else if (varDesc.Rows == 2)
				{
					edited = RenderCBFloat2(variable.first, static_cast<Vector2f*>(variable.second));
				}
				else 
				{
					edited = RenderCBFloat(variable.first, static_cast<float*>(variable.second));
				}
				break;
			}
			default:
				ImGui::Text(variable.first.c_str());
				ImGui::SameLine();
				ImGui::Text(" : ");
				ImGui::SameLine();
				ImGui::Text(("Undefined"));
				break;
			}

			if (edited) 
			{
				SetBufferData(buffer.first, variable.first, variable.second);
			}

		}
	}
}
void PixelShader::LoadDefaultValues()
{
}

std::shared_ptr<PixelShader> PixelShader::MakeInstance()
{
	auto instance = std::make_shared<PixelShader>();
	instance->pixelShader = pixelShader;
	return instance;
};

void PixelShader::SetResource()
{
	if (myConstantBuffers.size() > 0) 
	{
		for (auto buffer : myConstantBuffers )
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			HRESULT hr = DX11::Context->Map(buffer.second.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if (SUCCEEDED(hr))
			{
				std::vector<char> data;
				UINT size = 0;
				for (auto bufferValueSizes : myConstantBufferValuesSize[buffer.first])
				{
					size += bufferValueSizes.second;
				}
				data.resize(size);
				UINT destIndex = 0;
				for (int i = 0; i < myConstantBufferValuesSize[buffer.first].size(); i++)
				{
					void* dataPtr = myConstantBufferValues[buffer.first][i].second;
					if (dataPtr)
					{
						memcpy_s(&data[destIndex], myConstantBufferValuesSize[buffer.first][i].second, dataPtr, myConstantBufferValuesSize[buffer.first][i].second);
					}
					destIndex += myConstantBufferValuesSize[buffer.first][i].second;
				}
				BYTE* pBufferData = reinterpret_cast<BYTE*>(mappedResource.pData);

				memcpy_s(pBufferData, size, data.data(), size);
				DX11::Context->Unmap(buffer.second.Get(), 0);
			}
			DX11::Context->PSSetConstantBuffers(myConstatBufferBindPoints[buffer.first], 1, buffer.second.GetAddressOf());
		}
	}
	DX11::Context->PSSetShader(pixelShader, nullptr, 0);
}

std::shared_ptr<VertexShader> VertexShader::MakeInstance()
{
	auto instance = std::make_shared<VertexShader>();
	instance->inputLayout = inputLayout;
	instance->vertexShader = vertexShader;
	return instance;
};

void VertexShader::SetResource()
{
	DX11::Context->VSSetShader(vertexShader, nullptr, 0);
}

void VertexShader::Reload()
{
	ID3DBlob* shaderBlob = nullptr;
	std::string shaderName = myName;
	std::filesystem::path shaderPath;

	if (!shaderName.empty())
	{
		shaderPath = FindShaderFile(defaultPathReg, shaderName);


		if (shaderPath.empty())
		{
			shaderPath = FindShaderFile(defaultPathProj, shaderName);
		}
		if (!shaderPath.empty())
		{
			shaderPath += "\\";
			shaderPath += shaderName;
			shaderPath += ".hlsl";
		}
	}

	HRESULT hr = D3DCompileFromFile((shaderPath).c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0",
		NULL, 0, &shaderBlob, nullptr);
	if (FAILED(hr))
	{
		printerror(myName + " was scuffed");
		return;
	}
	else 
	{
		print(myName + " reload succeeded");
		DX11::Device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &vertexShader);
	}
	if (shaderBlob) 
	{
		shaderBlob->Release();
	}
}

void GeometryShader::SetResource()
{
	DX11::Context->GSSetShader(geometryShader, nullptr, 0);
}

void GeometryShader::Reload()
{
	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	std::string shaderName = myName;
	std::filesystem::path pixelShaderPath;

	if (!shaderName.empty())
	{
		pixelShaderPath = FindShaderFile(defaultPathReg, shaderName);


		if (pixelShaderPath.empty())
		{
			pixelShaderPath = FindShaderFile(defaultPathProj, shaderName);
		}
		if (!pixelShaderPath.empty())
		{
			pixelShaderPath += "\\";
			pixelShaderPath += shaderName;
			pixelShaderPath += ".hlsl";
		}
	}
	HRESULT hr = D3DCompileFromFile((pixelShaderPath).c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "gs_5_0",
		NULL, 0, &shaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		printerror(myName + " was scuffed");
		return;
	}
	else
	{
		print(myName + " reload succeeded");
		DX11::Device->CreateGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &geometryShader);
	}
	shaderBlob->Release();
	errorBlob->Release();
}

std::shared_ptr<GeometryShader> GeometryShader::MakeInstance()
{
	auto instance = std::make_shared<GeometryShader>();
	instance->geometryShader = geometryShader;
	instance->myName = myName;
	return instance;
};

PixelShader::PixelShader()
{
}

void PixelShader::Reload()
{
	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	std::string shaderName = myName;
	std::filesystem::path pixelShaderPath;

	if (!shaderName.empty())
	{
		pixelShaderPath = FindShaderFile(defaultPathReg, shaderName);


		if (pixelShaderPath.empty())
		{
			pixelShaderPath = FindShaderFile(defaultPathProj, shaderName);
		}
		if (!pixelShaderPath.empty())
		{
			pixelShaderPath += "\\";
			pixelShaderPath += shaderName;
			pixelShaderPath += ".hlsl";
		}
	}

	HRESULT hr = D3DCompileFromFile((pixelShaderPath).c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0",
		NULL, 0, &shaderBlob, &errorBlob);

	if (FAILED(hr))
	{
		printerror(myName + " was scuffed");
		return;
	}
	else
	{
		print(myName + " reload succeeded");
		DX11::Device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &pixelShader);
	}
	shaderBlob->Release();
	errorBlob->Release();
}

void GeometryShader::RenderInProperties()
{
	char nameInput[32] = "";
	for (size_t i = 0; i < myName.size(); i++)
	{
		nameInput[i] = myName[i];
	}

	//std::string emptyText = "##GeometryShaderNameInput";
	//if (ImGui::InputText(emptyText.c_str(), nameInput, IM_ARRAYSIZE(nameInput)))
	//{
	//	myName = nameInput;
	//}
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		AssetRegistry::GetInstance()->RenameAsset(this, myName);
	}

	//if (ImGui::Button("Reload Shader"))
	//{
	//	Reload();
	//}

}
const std::filesystem::path& Shader::FindShaderFile(const std::filesystem::path& aDirectory, const std::filesystem::path& aName)
{
	for (auto& directoryEntry : std::filesystem::directory_iterator(aDirectory))
	{
		if (directoryEntry.is_directory())
		{
			std::filesystem::path foundPath = FindShaderFile(directoryEntry.path(), aName);
			if (!foundPath.empty())
			{
				return foundPath;
			}
		}
		else
		{

			if (directoryEntry.path().string().find(aName.string()) != std::string::npos)
			{
				return aDirectory;
			}
		}
	}
	return L"";
}
void Shader::ConstructConstantBuffers()
{
	ID3D11ShaderReflection* pReflector = NULL;

	ID3DBlob* shaderBlob = nullptr;
	std::wstring wName = std::wstring(myName.begin(), myName.end());
	HRESULT hr = D3DReadFileToBlob((L"Resources/BuiltIn/Shaders/" + wName + L".cso").c_str(), &shaderBlob);
	if (FAILED(hr))
	{
		return;
	}

	D3DReflect(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
		IID_ID3D11ShaderReflection, (void**)&pReflector);

	D3D11_SHADER_DESC shaderDesc;
	pReflector->GetDesc(&shaderDesc);
	for (int i = 0; i < shaderDesc.ConstantBuffers; i++)
	{
		D3D11_SHADER_BUFFER_DESC shaderBufferDesc{};
		ID3D11ShaderReflectionConstantBuffer* constantBuffer = pReflector->GetConstantBufferByIndex(i);
		constantBuffer->GetDesc(&shaderBufferDesc);

		std::string name = shaderBufferDesc.Name;
		if (name == "FrameBuffer" || name == "MaterialBuffer" || name == "ObjectBuffer" || name == "lights" || name == "SceneLightsBuffer" || name == "SkeletonBuffer" || name == "UITransformBuffer")
		{
			continue;
		}

		D3D11_SHADER_INPUT_BIND_DESC desc;
		pReflector->GetResourceBindingDescByName(shaderBufferDesc.Name, &desc);
		myConstatBufferBindPoints.insert({ shaderBufferDesc.Name, desc.BindPoint });


		Microsoft::WRL::ComPtr<ID3D11Buffer> newBuffer;
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc = { 0 };
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.ByteWidth = shaderBufferDesc.Size;

		assert(bufferDesc.ByteWidth % 16 == 0 && "Buffer byte width not dividable by 16");

		HRESULT result = DX11::Device->CreateBuffer(&bufferDesc, nullptr, newBuffer.GetAddressOf());
		if (FAILED(result));
		else 
		{
			myConstantBuffers.insert({shaderBufferDesc.Name ,newBuffer });
			myConstantBufferValues.insert({ shaderBufferDesc.Name, std::vector<std::pair<std::string, void*>>()});
		}
		for (int j = 0; j < shaderBufferDesc.Variables; j++)
		{
			auto name = constantBuffer->GetVariableByIndex(j);
			D3D11_SHADER_VARIABLE_DESC varDesc;
			D3D11_SHADER_TYPE_DESC typeDesc;
			name->GetDesc(&varDesc);
			void* newVoid = new void*;
			if (varDesc.DefaultValue) 
			{
				memcpy_s(newVoid, varDesc.Size, varDesc.DefaultValue, varDesc.Size);
			}
			myConstantBufferValues[shaderBufferDesc.Name].push_back({ varDesc.Name, newVoid });
			myConstantBufferValuesSize[shaderBufferDesc.Name].push_back({ varDesc.Name, varDesc.Size });
			name->GetType()->GetDesc(&typeDesc);
			myConstantBufferValueTypes[shaderBufferDesc.Name].insert({ varDesc.Name, typeDesc });
		}
	}
	pReflector->Release();
	shaderBlob->Release();
}

bool Shader::SetBufferData(const std::string& aBufferName, const std::string& aVariableName, void* aValue)
{
	if (myConstantBufferValues.find(aBufferName) != myConstantBufferValues.end())
	{
		for (int i = 0; i < myConstantBufferValues[aBufferName].size(); i++)
		{
			if (myConstantBufferValues[aBufferName][i].first == aVariableName) 
			{
				if (myConstantBufferValues[aBufferName][i].second)
				{
					memcpy_s(myConstantBufferValues[aBufferName][i].second, myConstantBufferValuesSize[aBufferName][i].second, aValue, myConstantBufferValuesSize[aBufferName][i].second);
				}
				else
				{
					myConstantBufferValues[aBufferName][i].second = aValue;
				}
			}
		}
		return true;
	}
	return false;
}
