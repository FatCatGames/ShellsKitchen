#pragma once
#include "GameObjects\Component.h"
struct VAT 
{
	std::shared_ptr<Texture> texture;
	std::string name = "Undefined";
	bool loop = true;
};

class VertexAnimator : public Component
{
public:
	~VertexAnimator() override;
	void Init() override;
	void RenderInProperties(std::vector<Component*>& aComponentList) override;
	void Awake() override;
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	void PlayVAT(const int aId, float aDelay = 0);
	void StopVAT();

private:
	std::unordered_map<int, VAT> myVATS;

	int myNextVATSId = -1;
	int myCurrentVATSId = -1;
	bool myIsPlaying = false;
	bool myHasModel = false;
	std::vector<UINT> myTimerIds;

	void Update() override;
	void AddVAT(const int aId, std::shared_ptr<Texture> aTexture, const std::string& aName, bool aFlag = false);
	void RunInEditor();
};

