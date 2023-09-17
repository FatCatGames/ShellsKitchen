#pragma once

class PlayerModelComponent : public Component
{
public:
	PlayerModelComponent();
	~PlayerModelComponent();
	void Awake() override;
	void Update() override;
	void RenderInProperties(std::vector<Component*>& aComponentList);
	void RunInEditor() override;

private:
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

};
