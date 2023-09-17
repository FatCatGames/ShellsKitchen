#pragma once
#include "EditorWindow.h"
#include "Recast.h"
#include "Navigation/NavMesh.h"

class NavMesh;
class NavigationWindow : public EditorWindow
{
public:


private:
	void Render() override;
	std::shared_ptr<MeshData> mySelectedMeshData = nullptr;
	std::shared_ptr<NavMesh> mySelectedNavMesh = nullptr;
	std::shared_ptr<MeshData> mySavedMeshData = nullptr;
	MeshData myRCNavmeshData;

	std::vector<std::string> myPartitioningStyles = { "Watershed", "Monotone", "Layers" };

	NavmeshBakingSettings mySettings;

	std::vector<OffMeshConnection> myOffMeshConnections;

	bool myCreatingOffMeshConnections = false;

	bool mySelectingOffMeshConnectionA = false;
	bool mySelectingOffMeshConnectionB = false;
	Vector3f myOffMeshConnectionAPos = Vector3f::zero();
	Vector3f myOffMeshConnectionBPos = Vector3f::zero();
};