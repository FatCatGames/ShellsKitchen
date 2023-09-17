#include "stdafx.h"
#include "Quaternion.h"

Catbox::Quaternion::Quaternion(Vector4f aRot)
{
	x = aRot.x;
	y = aRot.y;
	z = aRot.z;
	w = aRot.w;
}

void Catbox::Quaternion::Normalize()
{
	float d = std::sqrt(std::powf(w, 2) + std::powf(x, 2) + std::powf(y, 2) + std::powf(z, 2));
	w = w / d;
	x = x / d;
	y = y / d;
	z = z / d;
}

Vector3f Catbox::Quaternion::ToEulerAngles(Quaternion q)
{
	float sinpitch = -2.0f * (q.y * q.z - q.w * q.x);

	//out angles radians
	//heading, pitch, bank
	//y, x, z
	float h, p, b;

	//assuming object to world quaternion
	if (std::fabsf(sinpitch) > 1 - 0.00001) //check for gimbal lock
	{
		p = 3.14f * 0.5f * sinpitch;
		h = atan2f(-q.x * q.z + q.w * q.y, 0.5f - q.y * q.y - q.z * q.z);
		b = 0.0f;
	}
	else
	{
		p = asinf(sinpitch);
		h = atan2f(q.x * q.z + q.w * q.y, 0.5f - q.x * q.x - q.y * q.y);
		b = atan2f(q.x * q.y + q.w * q.z, 0.5f - q.x * q.x - q.z * q.z);
	}

	return { Rad2Deg(p), Rad2Deg(h), Rad2Deg(b) };
}

Catbox::Quaternion Catbox::Quaternion::ToQuaternion(Vector3f aRotation)
{
	float c1 = cosf(Catbox::Deg2Rad(aRotation.x) * 0.5f);
	float c2 = cosf(Catbox::Deg2Rad(aRotation.y) * 0.5f);

	float c3 = cosf(Catbox::Deg2Rad(aRotation.z) * 0.5f);
	float s1 = sinf(Catbox::Deg2Rad(aRotation.x) * 0.5f);
	float s2 = sinf(Catbox::Deg2Rad(aRotation.y) * 0.5f);

	float s3 = sinf(Catbox::Deg2Rad(aRotation.z) * 0.5f);



	Quaternion q;
	q.x = s1 * c2 * c3 - c1 * s2 * s3;
	q.y = c1 * s2 * c3 + s1 * c2 * s3;
	q.z = c1 * c2 * s3 - s1 * s2 * c3;
	q.w = c1 * c2 * c3 + s1 * s2 * s3;

	return q;
}

Vector3f Catbox::Quaternion::SlerpToEuler(Quaternion q1, Quaternion q2, float t)
{
	{
		Quaternion qr;
		//Calculate angle between them.

		float cosHalfTheta = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
		if (abs(cosHalfTheta) >= 1.0f)
		{
			qr.w = q1.w; qr.x = q1.x; qr.y = q1.y; qr.z = q1.z;
		}
		else
		{
			if (cosHalfTheta < 0)
			{
				q2.w = -q2.w; q2.x = -q2.x; q2.y = -q2.y; q2.z = q2.z;
				cosHalfTheta = -cosHalfTheta;
			}
			// Calculate temporary values.
			float halfTheta = acosf(cosHalfTheta);
			float sinHalfTheta = sqrt(1.0f - cosHalfTheta * cosHalfTheta);
			// if theta = 180 degrees then result is not fully defined
			// we could rotate around any axis normal to qa or qb
			if (fabs(sinHalfTheta) < 0.001)
			{
				// fabs is floating point absolute
				qr.w = (q1.w * 0.5f + q2.w * 0.5f);
				qr.x = (q1.x * 0.5f + q2.x * 0.5f);
				qr.y = (q1.y * 0.5f + q2.y * 0.5f);
				qr.z = (q1.z * 0.5f + q2.z * 0.5f);
			}
			else
			{
				float ratioA = sinf((1 - t) * halfTheta) / sinHalfTheta;
				float ratioB = sinf(t * halfTheta) / sinHalfTheta;
				//calculate Quaternion.
				qr.w = (q1.w * ratioA + q2.w * ratioB);
				qr.x = (q1.x * ratioA + q2.x * ratioB);
				qr.y = (q1.y * ratioA + q2.y * ratioB);
				qr.z = (q1.z * ratioA + q2.z * ratioB);
			}
		}
		Vector3f returnVector;
		float sinr_cosp = 2 * (qr.w * qr.x + qr.y * qr.z);
		float cosr_cosp = 1 - 2 * (qr.x * qr.x + qr.y * qr.y);
		returnVector.x = std::atan2(sinr_cosp, cosr_cosp);

		float siny_cosp = 2 * (qr.w * qr.z + qr.x * qr.y);
		float cosy_cosp = 1 - 2 * (qr.y * qr.y + qr.z * qr.z);
		returnVector.z = std::atan2(siny_cosp, cosy_cosp);

		float sinp = 2.0f * (qr.w * qr.y - qr.z * qr.x);
		int aSign = 0;
		aSign = (int(0) < sinp) - (sinp < int(0));
		if (std::abs(sinp) >= 1) {
			returnVector.y = (Catbox::Pi() / 2 * aSign) * 2;
		}
		else
		{
			returnVector.y = std::asinf(sinp);
		}

		returnVector.x = Rad2Deg(returnVector.x);
		returnVector.y = Rad2Deg(returnVector.y);
		returnVector.z = Rad2Deg(returnVector.z);

		return returnVector;
	}
}
