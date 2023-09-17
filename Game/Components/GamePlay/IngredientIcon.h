#pragma once

class IngredientIcon : public Component
{
public:
	void Awake() override;
	void Update() override;
	void SetOffset(Vector3f anOffset);

private:
	virtual void RenderInProperties(std::vector<Component*>& aComponentList);
	virtual void Save(rapidjson::Value& aComponentData);
	virtual void Load(rapidjson::Value& aComponentData);

	Camera* myCamera = nullptr;
	RayPickerComponent* myRayPickerComponent = nullptr;

	Transform* myFirstChild = nullptr;

	Vector3f myParentPos;
	Vector3f myOffset = {0.0f, 0.1f, 0.0f};

	bool myDeactivateInAwake = true;
};