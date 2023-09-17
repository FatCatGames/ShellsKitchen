#pragma once
#include "GameObjects\Component.h"

class RayPickerComponent : public Component
{
public:
	RayPickerComponent() = default;
	~RayPickerComponent() = default;
	void Init() override;
	void Awake() override;
	void Update() override;
	void RunInEditor() override;
	Catbox::Vector3<float> GetMouseWorldPos(Vector2i aMousePos, float anYvalue = 0);
	Catbox::Vector3<float> GetClickDirection(Vector2i aMousePos);

private:

};