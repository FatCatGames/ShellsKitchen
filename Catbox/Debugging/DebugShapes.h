#pragma once
#include "CommonUtilities\Matrix4x4.hpp"
#include "CommonUtilities\Vector.h"
#include "Assets/Texture.h"

struct DebugLine
{
	Vector4f from;
	Vector4f to;
	Color color;
	float aliveTime = 0;
};

struct DebugCube
{
	Catbox::Matrix4x4<float> transform;
	Color color;
};

struct BillBoard 
{
	Catbox::Matrix4x4<float> transform;
	std::shared_ptr<Texture> texture = nullptr;
	UINT instanceId = 0;
};