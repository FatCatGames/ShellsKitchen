//#pragma once
//#include "Assets\MeshData.h"
//
//class GameObject;
//class NavMesh;
//class ModelInstance;
//struct NavmeshBakingSettings;
//struct OffMeshConnection;
//class NavMeshExporter
//{
//public:
//	NavMeshExporter() = default;
//	~NavMeshExporter() = default;
//	static MeshData ExportScene(NavmeshBakingSettings aNavmeshSetting);
//	static std::shared_ptr<NavMesh> BakeNavMesh(NavmeshBakingSettings aNavmeshSetting, const std::vector<float>& aConnectionsList);
//
//private:
//	static void AddObjectToNavmesh(GameObject* anObject);
//	static std::vector<ModelInstance*> myObjectsToExport;
//	static std::vector<std::shared_ptr<Model>> myModels;
//};