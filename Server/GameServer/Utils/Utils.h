#pragma once
#include <random>

class Utils
{
public:
	template<typename T>
	static T GetRandom(T min, T max)
	{
		// �õ尪�� ��� ���� random_device ����.
		std::random_device randomDevice;
		// random_device �� ���� ���� ���� ������ �ʱ�ȭ �Ѵ�.
		std::mt19937 generator(randomDevice());
		// �յ��ϰ� ��Ÿ���� �������� �����ϱ� ���� �յ� ���� ����.

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

		// �ʿ��� ���� ũ�� ���
		int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);

		if (sizeNeeded <= 0)
			return std::string();

		// ��ȯ�� ���ڿ� ����
		std::string result(sizeNeeded - 1, 0); // null-terminator ����
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

