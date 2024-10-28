#pragma once
#include <curl/curl.h>

struct AuthInfo
{
	std::chrono::steady_clock::time_point expiryTime;
	int authNo = 000000;
};

class Email
{
public:
	Email(const string& toAddr,
		const string& fromAddr,
		const string& nameFrom,
		const string& subject,
		int authNum,
		const string& ccAddr = ""
	);

public:
	CURLcode Send();

private:
	struct StringData
	{
		string msg;
		size_t bytesleft;
		StringData(string&& m) : msg{ m }, bytesleft{ msg.size() } {}
		StringData(string& m) = delete;
	};

	string DateTimeNow() const;

private:
	string SetPayloadText();
	string GenerateMessageID() const;
	static size_t payload_source(void* ptr, size_t size, size_t nmemb, void* userp);

private:
	string _toAddr;
	string _fromAddr;
	string _nameFrom;
	string _subject;
	string _htmlFormat;
	string _ccAddr;
	
	int _authNum;
};

class AuthManager
{
public:
	// Singleton
	static AuthManager& GetInstance()
	{
		static AuthManager instance;
		return instance;
	}

private:
	// Singleton
	AuthManager() = default;
	AuthManager(const AuthManager&) = delete;
	AuthManager& operator=(const AuthManager&) = delete;
	AuthManager(AuthManager&&) = delete;
	AuthManager& operator=(AuthManager&&) = delete;

public:
	bool SendMail(const string& toAddr, int authNo);

	bool AddAuthWaiter(const string& email);

	bool CheckAuthWaiter(const string& email, const string& authNum);

	void RemoveExpiredWaiters();

private:
	int GenerateVerificationCode();

private:
	USE_LOCK;
	map<string, AuthInfo> _authWaiters;
};
