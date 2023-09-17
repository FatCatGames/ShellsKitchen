#pragma once
namespace Catbox 
{
	class Quaternion
	{
	public:
		Quaternion() = default;
		Quaternion(Vector4f aRot);
		void Normalize();
		static Vector3f ToEulerAngles(Quaternion q);
		static Quaternion ToQuaternion(Vector3f aRotation);
		static Vector3f SlerpToEuler(Quaternion q1, Quaternion q2, float t);

		Vector4f GetVector4f() { return Vector4f(x, y, z, w); }


		float w = 0.0f, x = 0.0f, y = 0.0f, z = 0.0f;
	};
}
