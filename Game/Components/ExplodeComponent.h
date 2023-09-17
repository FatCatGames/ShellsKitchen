#pragma once

class ExplodeComponent : public Component
{
public:
	ExplodeComponent();
	~ExplodeComponent() = default;
	void Update() override;
	void SetCollisionOrigin(Vector3f aCollisionOrigin);
	void RenderInProperties(std::vector<Component*>& aComponentList);

private:
	void Save(rapidjson::Value& aComponentData) override;
	void Load(rapidjson::Value& aComponentData) override;

	bool myHasExploded = false;
	float myForce = 500;
	Vector3f myExplosionOrigin = { 0, 0, 0 };
	Vector3f myCollisionOrigin = { 0, 0, 0 };
};