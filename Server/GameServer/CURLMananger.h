#pragma once
#include <curl/curl.h>

struct AuthInfo
{
	string password;
	int authNo;
};

class Email
{
public:
	Email(const string& toAddr,
		const string& fromAddr,
		const string& nameFrom,
		const string& subject,
		const string& message,
		const string& ccAddr = ""
	);
	~Email();

private:
	struct StringData 
	{
		string msg;
		size_t bytesleft;
		StringData(string&& m) : msg{ m }, bytesleft{ msg.size() } {}
		StringData(string& m) = delete;
	};

	string DateTimeNow() const;
	CURLcode Send(const string& url, const string& userName, const string& password);
private:
	string SetPayloadText();
	string GenerateMessageID() const;
	static size_t payload_source(void* ptr, size_t size, size_t nmemb, void* userp);
private:
	string _toAddr;
	string _fromAddr;
	string _nameFrom;
	string _subject;
	string _message;
	string _ccAddr;
};

class CURLMananger
{
public:
	CURLMananger();
	~CURLMananger();

public:
	static void SendMail();

	void SendMail(const string& toAddr, const string& password);

	void SetToAddr(const string& toAddr) { this->_toAddr = toAddr; }

	void SetMessageID(const string& messageID) { this->_messageID = messageID; }

	void AddAuthWaiter(const string& email, const string& password);

private:
	string GenerateMessageID(const std::string& domain);

	string SetPayLoadText(const string& email, AuthInfo info);

	int GenerateVerificationCode();
private:
	string _toAddr;
	string _messageID;

private: 
	USE_LOCK;
	map<string, AuthInfo> _authWaiters;
	CURL* _connection;
	string _payload;
};
