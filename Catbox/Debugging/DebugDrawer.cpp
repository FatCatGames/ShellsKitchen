#include "stdafx.h"
#include "DebugDrawer.h"
#include "Graphics\Rendering\Renderers\DebugRenderer.h"


void DebugDrawer::DrawLine(const Vector3f& aFrom, const Vector3f& aTo, const Color& aColor, float aAliveTime)
{
	if (EDITORMODE && Engine::GetInstance()->GetGraphicsEngine()->GetDebugDrawerToggle())
	{
		DebugLine line;
		line.color = aColor;
		line.from = Vector4f(aFrom.x, aFrom.y, aFrom.z, 1);
		line.to = Vector4f(aTo.x, aTo.y, aTo.z, 1);
		line.aliveTime = aAliveTime;
		Engine::GetInstance()->GetGraphicsEngine()->AddToRenderQueue(line);
	}
}

void DebugDrawer::DrawCurve(const std::array<Vector3f, 4> somePoints, const Color& aColor, const unsigned int aComplexity, float aAliveTime)
{
	Vector3f prevPos = somePoints[0];

	// Draw a curve
	for (unsigned int i = 0; i < aComplexity; ++i)
	{
		const float percentOfCurve = static_cast<float>(i) / static_cast<float>(aComplexity);

		// DeCasteljau lerping
		Vector3f a = Catbox::Lerp(somePoints[0], somePoints[1], percentOfCurve);
		Vector3f b = Catbox::Lerp(somePoints[1], somePoints[2], percentOfCurve);
		Vector3f c = Catbox::Lerp(somePoints[2], somePoints[3], percentOfCurve);
		Vector3f d = Catbox::Lerp(a, b, percentOfCurve);
		Vector3f e = Catbox::Lerp(b, c, percentOfCurve);
		Vector3f P = Catbox::Lerp(d, e, percentOfCurve);

		DebugDrawer::DrawLine(prevPos, P, aColor, aAliveTime);
		prevPos = P;
	}

	DebugDrawer::DrawLine(prevPos, somePoints[3], aColor, aAliveTime);
}

void DebugDrawer::DrawCube(const Vector3f& aPosition, const Vector3f& aScale, const Vector3f& aRotation, const Color& aColor, const Vector3f& aCustomPivot, const Transform& aOffsetTransfrom)
{
	DebugCube box;
	box.transform(4, 1) = aCustomPivot.x;
	box.transform(4, 2) = aCustomPivot.y;
	box.transform(4, 3) = aCustomPivot.z;

	Catbox::Matrix4x4<float> rotationMatrix = Catbox::Matrix4x4<float>();
	rotationMatrix = rotationMatrix.CreateRotationAroundZ(Catbox::Deg2Rad(aRotation.z)) * rotationMatrix;
	rotationMatrix = rotationMatrix.CreateRotationAroundY(Catbox::Deg2Rad(aRotation.y)) * rotationMatrix;
	rotationMatrix = rotationMatrix.CreateRotationAroundX(Catbox::Deg2Rad(aRotation.x)) * rotationMatrix;

	Vector4f scale = { aScale.x, aScale.y, aScale.z, 1 };
	Catbox::Matrix4x4<float> scaleMatrix;
	scaleMatrix = scaleMatrix * scale;

	Catbox::Matrix4x4<float> aTranslation = Catbox::Matrix4x4<float>();
	aTranslation(4, 1) = aPosition.x;
	aTranslation(4, 2) = aPosition.y;
	aTranslation(4, 3) = aPosition.z;

	Catbox::Matrix4x4<float> myWorldTransform = scaleMatrix * (box.transform * rotationMatrix) * aTranslation;

	box.transform = myWorldTransform;
	box.color = aColor;

	Engine::GetInstance()->GetGraphicsEngine()->AddToRenderQueue(box);
}

// Code taken from outside sources.
void DebugDrawer::DrawSphere(const Vector3f& aPosition, float aRadius, const Color& aColor, const unsigned int aComplexity, float aAliveTime)
{
	enum Axis
	{
		X = 0,
		Y,
		Z,

		Count
	};

	// For each axis
	for (int axis = X; axis < Axis::Count; axis++)
	{
		Vector3f firstPos;
		Vector3f prevPos;

		// Draw a circle
		for (int i = 0; i < aComplexity; ++i)
		{
			const float percentOfCircle = static_cast<float>(i) / static_cast<float>(aComplexity);

			const float radiansOfCircle = Catbox::Pi() * 2.f * percentOfCircle;

			Vector3f posInCircle;

			switch (axis)
			{
			case X:
				posInCircle = Vector3f(
					cos(radiansOfCircle) * aRadius,
					sin(radiansOfCircle) * aRadius,
					0.f);
				break;
			case Y:
				posInCircle = Vector3f(
					cos(radiansOfCircle) * aRadius,
					0.f,
					sin(radiansOfCircle) * aRadius);
				break;
			case Z:
				posInCircle = Vector3f(
					0.f,
					cos(radiansOfCircle) * aRadius,
					sin(radiansOfCircle) * aRadius);
				break;
			default:
				assert(false && "whot");
				break;
			}

			if (i == 0)
			{
				firstPos = aPosition + posInCircle;
				prevPos = aPosition + posInCircle;
				continue;
			}

			const auto toPos = aPosition + posInCircle;

			DebugDrawer::DrawLine(prevPos, toPos, aColor, aAliveTime);

			prevPos = toPos;
		}

		// Koppla (swedish word, cant english) together the last part with the first part of the circle
		DebugDrawer::DrawLine(prevPos, firstPos, aColor, aAliveTime);
	}
}

void DebugDrawer::DrawBillboard(const Vector3f& aPosition, float aSize, std::shared_ptr<Texture> aTexture)
{
	if (EDITORMODE && Engine::GetInstance()->GetGraphicsEngine()->GetDebugDrawerToggle())
	{
		BillBoard billboard;

		billboard.texture = aTexture;

		Vector4f scale = { aSize, aSize, aSize, 1 };
		Catbox::Matrix4x4<float> scaleMatrix;
		scaleMatrix = scaleMatrix * scale;

		Catbox::Matrix4x4<float> translationMatrix = Catbox::Matrix4x4<float>();
		translationMatrix(4, 1) = aPosition.x;
		translationMatrix(4, 2) = aPosition.y;
		translationMatrix(4, 3) = aPosition.z;
		billboard.transform = scaleMatrix * translationMatrix;

		//sEngine::GetInstance()->GetGraphicsEngine()->AddToRenderQueue(billboard);
	}
}
