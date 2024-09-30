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

	static std::string WCHARToString(const WCHAR* wstr) 
	{
		// 필요한 버퍼 크기를 계산
		size_t bufferSize = 0;
		wcstombs_s(&bufferSize, nullptr, 0, wstr, 0); // 필요한 크기 계산 (널 문자 포함)

		std::string str(bufferSize, '\0'); // 문자열을 위한 버퍼 생성
		wcstombs_s(nullptr, &str[0], bufferSize, wstr, bufferSize - 1); // 변환 및 복사

		return str;
	}

	static std::wstring stringToWString(const std::string& str) 
	{
		std::wstring wstr(str.begin(), str.end());
		wstr.push_back(L'\0'); // 널 종결자 추가
		return wstr;
	}

	static WCHAR* StringToWCHAR(const std::string& str) 
	{
		// 변환 후의 와이드 문자열을 담을 배열 크기 계산
		size_t wcharCount = str.size() + 1; // 널 종결자를 포함한 크기
		WCHAR* wstr = new WCHAR[wcharCount]; // 변환된 문자열을 담을 배열

		// 문자열 변환: 멀티바이트 -> 와이드 문자
		size_t convertedChars = 0;
		mbstowcs_s(&convertedChars, wstr, wcharCount, str.c_str(), _TRUNCATE);

		return wstr; // 변환된 WCHAR* 반환
	}
};

