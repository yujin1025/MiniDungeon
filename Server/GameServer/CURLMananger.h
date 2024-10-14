#pragma once
class CURLMananger
{
public:
	CURLMananger();
	~CURLMananger();

public:
	static void SendMail();

	void SetToAddr(const string& toAddr) { this->_toAddr = toAddr; }

	void SetMessageID(const string& messageID) { this->_messageID = messageID; }

private:
	string GenerateMessageID(const std::string& domain);

	string SetPayLoadText();

	string GenerateVerificationCode();
private:
	string _payload_string;
	string _toAddr;
	string _messageID;
};
