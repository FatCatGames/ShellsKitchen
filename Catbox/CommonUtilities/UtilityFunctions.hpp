#ifndef UTILITYFUNCTIONS_H
#define UTILITYFUNCTIONS_H
#include <cassert>
#include <random>
#include <commdlg.h>
#include <string>
//NO ELRIC, BAD ELRIC
#define maybe Catbox::GetRandom(0,2)

namespace Catbox
{
	enum Troolean : char
	{
		Undefined = -1,
		False = 0,
		True = 1
	};

	class Shart
	{
	public:
		short GetShort() { return myValue; };
		char GetChar() { return static_cast<char>(myValue); };
		void SetValue(short aValue) { myValue = aValue; }
		void SetValue(char aValue) { myValue = static_cast<short>(aValue); }

	private:
		short myValue;
	};

	static std::wstring ToWString(std::string aString)
	{
		return std::wstring(aString.begin(), aString.end());
	}

	static std::string ToLowerString(std::string aString)
	{
		std::string returnString = aString;
		for (size_t i = 0; i < returnString.size(); i++)
		{
			returnString[i] = static_cast<char>(std::tolower(static_cast<int>(returnString[i])));
		}
		return returnString;
	}

	static int StringToInt(std::string aString)
	{
		int aReturnInt = 0;
		for (int i = 0; i < aString.size(); i++)
		{
			aReturnInt += (int)aString[i];
			aReturnInt += (aReturnInt) * (10 * (i + 1));
		}
		return aReturnInt;
	}

	static bool CheckText(const char* aText, UINT aSize)
	{
		if (aText[aSize] != '\0')
		{
			return false;
		}
		return true;
	}

	template<class T>
	static bool LowerThan(T& firstThing, T& anotherThing)
	{
		return (firstThing < anotherThing)
	}

	static std::string OpenFile(const char* aFilter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = GetActiveWindow();
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = aFilter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn))
		{
			std::string fullPath = ofn.lpstrFile;
			std::size_t pos = fullPath.find("Bin");
			std::string relativePath = fullPath.substr(pos + 4);

			return relativePath;
		}
		return std::string(); //operation canceled
	}

	static std::string SaveFile(const char* aFilter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = GetActiveWindow();
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = aFilter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_NOCHANGEDIR;

		ofn.lpstrDefExt = strchr(aFilter, '\0') + 1;


		if (GetSaveFileNameA(&ofn))
		{
			return ofn.lpstrFile;
		}
		return std::string(); //operation canceled
	}

	static std::string GetNameFromPath(const std::string& aPath)
	{
		size_t indexOfPeriod = aPath.find_last_of('.');
		if (indexOfPeriod != std::string::npos)
		{
			size_t indexOfSlash = aPath.find_last_of('/') + 1;
			if (indexOfSlash == 0)
			{
				indexOfSlash = aPath.find_last_of('\\') + 1;
			}
			return aPath.substr(indexOfSlash, indexOfPeriod - indexOfSlash);
		}
		return "";
	}

	static std::string GetExtensionFromPath(const std::string& aPath)
	{
		size_t indexOfPeriod = aPath.find_last_of('.');
		if (indexOfPeriod != std::string::npos)
		{
			return aPath.substr(indexOfPeriod, aPath.size() - indexOfPeriod);
		}
		return "";
	}

	static void CreateFileK(std::string& aPath)
	{
		DWORD attributes = GetFileAttributesA((LPCSTR)aPath.c_str());
		if (attributes & FILE_ATTRIBUTE_READONLY)
		{
			SetFileAttributesA((LPCSTR)aPath.c_str(), attributes & ~FILE_ATTRIBUTE_READONLY);
		}

		std::ifstream  src(aPath, std::ios::binary);
		std::ofstream  dst(aPath, std::ios::binary);
		dst << src.rdbuf();
	}

	static void CopyFileK(const std::string& aSource, std::string& aDestination)
	{
		std::string fileName = Catbox::GetNameFromPath(aDestination);
		std::string fileExtension = Catbox::GetExtensionFromPath(aDestination);
		std::string directory = aDestination;
		directory.erase(directory.end() - fileName.size() - fileExtension.size(), directory.end());

		bool success = !std::filesystem::exists(aDestination);
		int num = 2;
		while (!success)
		{
			std::string newName = directory + fileName + std::to_string(num) + fileExtension;

			if (!std::filesystem::exists(newName))
			{
				success = true;
				aDestination = newName;
			}
			++num;
		}

		DWORD attributes = GetFileAttributesA((LPCSTR)aDestination.c_str());
		if (attributes & FILE_ATTRIBUTE_READONLY)
		{
			success = SetFileAttributesA((LPCSTR)aDestination.c_str(), attributes & ~FILE_ATTRIBUTE_READONLY);
		}

		std::ifstream  src(aSource, std::ios::binary);
		std::ofstream  dst(aDestination, std::ios::binary);
		dst << src.rdbuf();
	}

	template <typename T>
	T Max(const T aFirst, const T aSecond)
	{
		if (aFirst >= aSecond)
		{
			return(aFirst);
		}
		if (aFirst < aSecond)
		{
			return(aSecond);
		}
	}

	template <typename T>
	T Min(const T aFirst, const T aSecond)
	{
		if (aFirst <= aSecond)
		{
			return(aFirst);
		}
		if (aFirst > aSecond)
		{
			return(aSecond);
		}
	}

	template <typename T>
	T Abs(const T aValue)
	{
		if (aValue < 0)
		{
			return(-aValue);
		}
		return(aValue);
	}

	template <typename T>
	T Clamp(const T aValue, const T aMinValue, const T aMaxValue)
	{
		//printerror(aMinValue <= aMaxValue && "Entered min value was larger than max value!");

		if (aValue < aMinValue)
		{
			return(aMinValue);
		}
		if (aValue > aMaxValue)
		{
			return(aMaxValue);
		}
		return(aValue);
	}

	template <typename T>
	T Lerp(const T aStartValue, const T anEndValue, float aPercentage)
	{
		return(static_cast<T>(aStartValue + aPercentage * (anEndValue - aStartValue)));
	}

	template <typename T>
	T SmoothStep(const T aMin, const T aMax, T aValue)
	{
		aValue = Clamp((aValue - aMin) / (aMax - aMin), 0.0f, 1.0f);

		return aValue * aValue * (3 - 2 * aValue);
	}

	template <typename T>
	void Swap(T& aFirst, T& aSecond)
	{
		T temp = aFirst;
		aFirst = aSecond;
		aSecond = temp;
	}

	template <typename T>
	T GetRandom(T aLowerBound, T aUpperBound)
	{
		float random = (rand()) / (float)RAND_MAX;
		T diff = aUpperBound - aLowerBound;
		T r = static_cast<T>(random * diff);
		return aLowerBound + r;
	}

	static float Pi()
	{
		return 3.14159265359f;
	}

	static float Rad2Deg(float aRadians)
	{
		return static_cast<float>(aRadians * 180.0f / Pi());
	}

	static Catbox::Vector3<float> GetMouseWorldPos(Vector2i aMousePos, float anYvalue, Camera* aCamera)
	{
		//https://stackoverflow.com/questions/71731722/correct-way-to-generate-3d-world-ray-from-2d-mouse-coordinates

		Catbox::Vector2<int> test0 = Engine::GetInstance()->ViewportToScreenPos(static_cast<int>(aMousePos.x), static_cast<int>(aMousePos.y));

		int winWidth = DX11::GetResolution().x;
		int winHeight = DX11::GetResolution().y;
		float normalised_x = 2 * test0.x / static_cast<float>(winWidth) - 1;
		float normalised_y = 1 - 2 * test0.y / static_cast<float>(winHeight);

		Catbox::Vector4<float> clipCoordsNear = { normalised_x, normalised_y , 0, 1 }; //Nearplane

		Catbox::Matrix4x4<float> invereprojection = Catbox::Matrix4x4<float>::GetFastInverse(aCamera->GetProjectionMatrix());
		Catbox::Matrix4x4<float> inverseViewMatrix = aCamera->GetTransform()->GetWorldTransformMatrix();

		//convert from 2d to a 3d direction
		Catbox::Vector4<float> clipCoordsInCameraSpaceNear = clipCoordsNear * invereprojection;

		Catbox::Vector4<float> clipCoordsInCameraSpaceNearBackup = { clipCoordsInCameraSpaceNear.x , clipCoordsInCameraSpaceNear.y , 1, 0 };

		Catbox::Vector4<float> clipCoordsInWorldSpaceNear = clipCoordsInCameraSpaceNearBackup * inverseViewMatrix;

		Catbox::Vector3<float> RayDirectionNear = { clipCoordsInWorldSpaceNear.x, clipCoordsInWorldSpaceNear.y, clipCoordsInWorldSpaceNear.z };

		RayDirectionNear.Normalize();

		Catbox::Vector3<float> CameraPos = aCamera->GetTransform()->worldPos();

		float multiplierNear = (anYvalue - CameraPos.y) / RayDirectionNear.y;

		Catbox::Vector3<float> ClickPosNear = CameraPos + (RayDirectionNear * multiplierNear);

		//print("X ClickNear value: " + std::to_string(ClickPosNear.x) + "       Y value: " + std::to_string(ClickPosNear.y) + "       Z value: " + std::to_string(ClickPosNear.z));
		return ClickPosNear;
	}

	static Catbox::Vector3<float> GetClickDirection(Vector2i aMousePos, Camera* aCamera)
	{
		Catbox::Vector2<int> test0 = Engine::GetInstance()->ViewportToScreenPos(static_cast<int>(aMousePos.x), static_cast<int>(aMousePos.y));

		int winWidth = DX11::GetResolution().x;
		int winHeight = DX11::GetResolution().y;
		//if (EDITORMODE)
		//{
		//	int winWidth = 1884;
		//	int winHeight = 1042;
		//}

		float normalised_x = 2 * test0.x / static_cast<float>(winWidth) - 1;
		float normalised_y = 1 - 2 * test0.y / static_cast<float>(winHeight);

		Catbox::Vector4<float> clipCoordsNear = { normalised_x, normalised_y , 0, 1 }; //Nearplane
		Catbox::Matrix4x4<float> invereprojection = Catbox::Matrix4x4<float>::GetFastInverse(aCamera->GetProjectionMatrix());
		Catbox::Matrix4x4<float> inverseViewMatrix = aCamera->GetTransform()->GetWorldTransformMatrix();

		Catbox::Vector4<float> clipCoordsInCameraSpaceNear = clipCoordsNear * invereprojection;

		Catbox::Vector4<float> clipCoordsInCameraSpaceNearBackup = { clipCoordsInCameraSpaceNear.x , clipCoordsInCameraSpaceNear.y , 1, 0 };

		Catbox::Vector4<float> clipCoordsInWorldSpaceNear = clipCoordsInCameraSpaceNearBackup * inverseViewMatrix;

		Catbox::Vector3<float> RayDirectionNear = { clipCoordsInWorldSpaceNear.x, clipCoordsInWorldSpaceNear.y, clipCoordsInWorldSpaceNear.z };

		RayDirectionNear.Normalize();

		return RayDirectionNear;
	}

	static float Deg2Rad(float aDegrees)
	{
		return aDegrees * Pi() / 180.0f;
	}

	static bool SameSide(Vector3f aPoint, Vector3f anotherPoint, Vector3f A, Vector3f B, float aOffset)
	{
		Vector3f crossA = (B - A).Cross(aPoint - A);
		Vector3f crossB = (B - A).Cross(anotherPoint - A);
		return crossA.Dot(crossB) >= -aOffset;
	}

	static bool PointInTriangle(Vector3f aPoint, Vector3f A, Vector3f B, Vector3f C, float aOffset = 2.0f)
	{
		return (SameSide(aPoint, A, B, C, aOffset) && SameSide(aPoint, B, A, C, aOffset) && SameSide(aPoint, C, A, B, aOffset));
	}

	static float WrapPi(float theta)
	{
		if (std::fabs(theta) <= Pi())
		{
			const float TWOPPI = 2.0f * Pi();
			float revolutions = std::floor((theta + Pi()) * (1.0f / TWOPPI));

			theta -= revolutions * TWOPPI;
		}
	}

	static bool PointInTriangleBarycentric(Vector3f aPoint, Vector3f A, Vector3f B, Vector3f C, float aOffset = 0.0f)
	{
		// Compute vectors        
		Vector3f v0 = C - A;
		Vector3f v1 = B - A;
		Vector3f v2 = aPoint - A;

		// Compute dot products
		float dot00 = v0.Dot(v0);
		float dot01 = v0.Dot(v1);
		float dot02 = v0.Dot(v2);
		float dot11 = v1.Dot(v1);
		float dot12 = v1.Dot(v2);

		// Compute barycentric coordinates
		float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
		float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		// Check if point is in triangle
		//return (u >= 0 ) && (v >= 0) && (u + v < 1);
		// More Lenient Version
		return (u >= 0 - aOffset) && (v >= 0 - aOffset) && (u + v <= 1 + aOffset);
	}
}

#endif