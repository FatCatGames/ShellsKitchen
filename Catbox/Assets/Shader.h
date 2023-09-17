#pragma once
#include "Graphics/Rendering/DX11/DX11.h"
#include <wrl.h>
#include "Graphics/Rendering/Buffers/CBuffer.hpp"
#include <d3d11shader.h>
#include <vector>

class Shader : public Asset
{
public:
	virtual void SaveData() {};
	virtual void LoadData() {};

	virtual void SetData() {};
	virtual void SetResource() {};

	const std::filesystem::path& FindShaderFile(const std::filesystem::path& aDirectory, const std::filesystem::path& aName);

	virtual void RenderInProperties() {};

	void ConstructConstantBuffers();

	bool SetBufferData(const std::string& aBufferName, const std::string& aVariableName, void* aValue);

	bool WasEditedThisFrame() 
	{
		bool returnValue = myWasEdited;
		myWasEdited = false;
		return returnValue;
	}
	std::unordered_map<std::string, UINT> myConstatBufferBindPoints;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11Buffer>> myConstantBuffers;
	std::unordered_map<std::string, std::vector<std::pair<std::string, void*>>> myConstantBufferValues;
	std::unordered_map<std::string, std::vector<std::pair<std::string, size_t>>> myConstantBufferValuesSize;
	std::unordered_map<std::string, std::unordered_map<std::string, D3D11_SHADER_TYPE_DESC>> myConstantBufferValueTypes;

	std::filesystem::path defaultPathReg = "..\\Catbox\\Shaders";
	std::filesystem::path defaultPathProj = "..\\Game\\Shaders";

protected:
	bool myWasEdited = false;
};


class PixelShader : public Shader
{
public:

	PixelShader();
	void Reload();

	ID3D11PixelShader* pixelShader = NULL;
	virtual std::shared_ptr<PixelShader> MakeInstance();


	virtual void RenderInProperties() override;

	virtual void LoadDefaultValues() override;

	void SetResource() override;

private:
	void SaveAsset(const char* /*aPath*/) override
	{
	}
};

class VertexShader : public Shader
{
public:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	ID3D11VertexShader* vertexShader = NULL;
	virtual std::shared_ptr<VertexShader> MakeInstance();
	void SetResource() override;
	void Reload();

	virtual void RenderInProperties() override;

private:
	void SaveAsset(const char* /*aPath*/) override
	{
	}
};

class GeometryShader : public Shader
{
public:
	ID3D11GeometryShader* geometryShader = NULL;
	void SetResource() override;

	void Reload();
	virtual std::shared_ptr<GeometryShader> MakeInstance();
	virtual void RenderInProperties() override;

private:
	void SaveAsset(const char* /*aPath*/) override
	{
	}
};