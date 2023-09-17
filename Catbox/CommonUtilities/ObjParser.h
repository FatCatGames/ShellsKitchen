#pragma once
#include "ObjParser.h"
#include "Assets\MeshData.h"
#include "Assets\AssetRegistry.h"
//#include <strstream>
#include "3D/ModelInstance.h"
#include <sstream>
#include <ostream>
#include "Assets/Model.h"

static std::shared_ptr<MeshData> ParseObj(const char* aPath)
{
	std::ifstream in(aPath, std::ios::in);

	if (!in)
	{
		MessageHandler::debugMessages.errors.push_back("Cannot find " + std::string(aPath));
		return nullptr;
	}

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	std::string line;
	while (std::getline(in, line))
	{
		//check v for vertices
		if (line.substr(0, 2) == "v ") 
		{
			std::istringstream v(line.substr(2));
			Vertex vert;
			float x, y, z;
			v >> x; v >> y; v >> z;
			vert.position = Vector4f(x, y, z, 1);
			vertices.push_back(vert);
		}

		//check for faces
		else if (line.substr(0, 2) == "f ") 
		{
			int a, b, c;
			int A, B, C;
			const char* chh = line.c_str();
			// Unity format
			//sscanf_s(chh, "f %i//%i %i//%i %i//%i", &a, &A, &c, &C, &b, &B);
			

			//Blender format
			sscanf_s(chh, "f %i/%i/%i %i/%i/%i %i/%i/%i", &a, &A, &A, &b, &B, &B, &c, &C, &C);

			indices.push_back(a-1);
			indices.push_back(b-1);
			indices.push_back(c-1);
		}
	}

	auto meshData = CreateAsset<MeshData>(aPath);
	meshData->SetVertices(vertices);
	meshData->SetIndices(indices);
	return meshData;
}

static bool ExportObjectsToObj(std::shared_ptr<MeshData> aMeshdata) 
{
	std::ofstream obj("exportedScene.obj");
	auto vertices = aMeshdata->myVertices;
	for (int i = 0; i < aMeshdata->myVertices.size(); i++)
	{
		obj << "v" << " " << std::to_string(vertices[i].position.x) << " " << std::to_string(vertices[i].position.y) << " " << std::to_string(vertices[i].position.z) << std::endl;
	}
	auto indecies = aMeshdata->myIndices;
	for (int i = 0; i < aMeshdata->myIndices.size() - 3; i += 3)
	{
		obj << "f" << " " << std::to_string(indecies[i]) << " " << std::to_string(indecies[i + 1]) << " " << std::to_string(indecies[i + 2]) << std::endl;
	}
	return true;
}

static bool ExportObjectsToObj(std::vector<GameObject*> aGameObjectList)
{
	std::vector<Vertex> vertices;
	std::vector<int> indecies;

	Vector3f pivotPoint;
	float lowestYPoint = FLT_MAX;
	for (auto object : aGameObjectList)
	{
		pivotPoint += object->GetTransform()->worldPos();
	}
	pivotPoint = pivotPoint / (float)aGameObjectList.size();

	Vector4f pivotPoint4 = { pivotPoint.x, pivotPoint.y, pivotPoint.z, 1.0f };

	for (auto object : aGameObjectList)
	{
		if (object->GetComponent<ModelInstance>()) 
		{
			auto model = object->GetComponent<ModelInstance>()->GetModel();
			if (model)
			{
				Vector3f tempRot = object->GetTransform()->worldRot();

				Catbox::Matrix4x4<float> worldTransfrom = object->GetTransform()->GetWorldTransformMatrix();
				Catbox::Matrix4x4<float> rotationMatrix = Catbox::Matrix4x4<float>();

				rotationMatrix = rotationMatrix.CreateRotationAroundZ(Catbox::Deg2Rad(tempRot.z)) * rotationMatrix;
				rotationMatrix = rotationMatrix.CreateRotationAroundY(Catbox::Deg2Rad(tempRot.y)) * rotationMatrix;
				rotationMatrix = rotationMatrix.CreateRotationAroundX(Catbox::Deg2Rad(tempRot.x)) * rotationMatrix;

				worldTransfrom(4, 1);
				worldTransfrom(4, 2);
				worldTransfrom(4, 3);

				for (int i = 0; i < model->GetModelParts().size(); i++)
				{
					std::unordered_map<unsigned int, size_t> tempIndecies;
					for (int j = 0; j < model->GetModelPart(i)->myVertices.size(); j++)
					{
						vertices.push_back(model->GetModelPart(i)->myVertices[j]);
						vertices[vertices.size() - 1].position = ((vertices[vertices.size() - 1].position) * worldTransfrom);
						vertices[vertices.size() - 1].position.x *= -1.0f;
						tempIndecies.insert({ j, vertices.size() - 1 });
					}
					for (int j = 0; j < model->GetModelPart(i)->myIndices.size(); j += 3)
					{
						if (tempIndecies.find(model->GetModelPart(i)->myIndices[j]) != tempIndecies.end())
						{
							if (tempIndecies.find(model->GetModelPart(i)->myIndices[j + 1]) != tempIndecies.end())
							{
								if (tempIndecies.find(model->GetModelPart(i)->myIndices[j + 2]) != tempIndecies.end())
								{
									indecies.push_back(tempIndecies[model->GetModelPart(i)->myIndices[j]] + 1);
									indecies.push_back(tempIndecies[model->GetModelPart(i)->myIndices[j + 1]] + 1);
									indecies.push_back(tempIndecies[model->GetModelPart(i)->myIndices[j + 2]] + 1);

								}
							}
						}
					}
				}
			}
		}
	}

	std::ofstream obj("exportedScene.obj");
	for (int i = 0; i < vertices.size(); i++)
	{
		obj << "v" << " " << std::to_string(vertices[i].position.x) << " " << std::to_string(vertices[i].position.y) << " " << std::to_string(vertices[i].position.z) << " 1.0" << std::endl;
	}
	//for (int i = 0; i < vertices.size(); i++)
	//{
	//	obj << "vt" << " " << std::to_string(vertices[i].UVs->x) << " " << std::to_string(vertices[i].UVs->y) << std::endl;
	//}
	//for (int i = 0; i < vertices.size(); i++)
	//{
	//	obj << "vn" << " " << std::to_string(vertices[i].normal.x) << " " << std::to_string(vertices[i].normal.y) << " " << std::to_string(vertices[i].normal.z) << std::endl;
	//}
	for (int i = 0; i < indecies.size(); i += 3)
	{
		obj << "f" << " " << std::to_string(indecies[i]) << " " << std::to_string(indecies[i + 1]) << " " << std::to_string(indecies[i + 2]) << std::endl;
	}
	obj.close();

	//obj << "l" << " ";

	//for (int i = 0; i < indecies.size(); i++)
	//{
	//	obj << std::to_string(indecies[i]);
	//}
	return true;
}