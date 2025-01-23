#pragma once
#include <random>

class Utils
{
public:
	template<typename T>
	static T GetRandom(T min, T max)
	{
		// 시드값을 얻기 위한 random_device 생성.
		std::random_device randomDevice;
		// random_device 를 통해 난수 생성 엔진을 초기화 한다.
		std::mt19937 generator(randomDevice());
		// 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.

		if constexpr (std::is_integral_v<T>)
		{
			std::uniform_int_distribution<T> distribution(min, max);
			return distribution(generator);
		}
		else
		{
			std::uniform_real_distribution<T> distribution(min, max);
			return distribution(generator);
		}
	}

	static std::string WStringToString(const std::wstring& wstr)
	{
		if (wstr.empty())
			return std::string();

		// 필요한 버퍼 크기 계산
		int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);

		if (sizeNeeded <= 0)
			return std::string();

		// 변환된 문자열 저장
		std::string result(sizeNeeded - 1, 0); // null-terminator 제외
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], sizeNeeded, nullptr, nullptr);

		return result;
	}

	static std::wstring StringToWString(const std::string& str)
	{
		if (str.empty())
			return std::wstring();

		int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
		std::wstring result(sizeNeeded, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], sizeNeeded);
		result.pop_back(); // Remove the extra null terminator
		return result;
	}


};

