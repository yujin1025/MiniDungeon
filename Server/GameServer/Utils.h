#pragma once
#include <random>
#include <string>
#include <cwchar>

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

	static std::string WCHARToString(const WCHAR* wstr) 
	{
		// �ʿ��� ���� ũ�⸦ ���
		size_t bufferSize = 0;
		wcstombs_s(&bufferSize, nullptr, 0, wstr, 0); // �ʿ��� ũ�� ��� (�� ���� ����)

		std::string str(bufferSize, '\0'); // ���ڿ��� ���� ���� ����
		wcstombs_s(nullptr, &str[0], bufferSize, wstr, bufferSize - 1); // ��ȯ �� ����

		return str;
	}

	static std::wstring stringToWString(const std::string& str) 
	{
		std::wstring wstr(str.begin(), str.end());
		wstr.push_back(L'\0'); // �� ������ �߰�
		return wstr;
	}

	static WCHAR* StringToWCHAR(const std::string& str) 
	{
		// ��ȯ ���� ���̵� ���ڿ��� ���� �迭 ũ�� ���
		size_t wcharCount = str.size() + 1; // �� �����ڸ� ������ ũ��
		WCHAR* wstr = new WCHAR[wcharCount]; // ��ȯ�� ���ڿ��� ���� �迭

		// ���ڿ� ��ȯ: ��Ƽ����Ʈ -> ���̵� ����
		size_t convertedChars = 0;
		mbstowcs_s(&convertedChars, wstr, wcharCount, str.c_str(), _TRUNCATE);

		return wstr; // ��ȯ�� WCHAR* ��ȯ
	}
};

