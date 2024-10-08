#pragma once

using namespace std;

class SMTPManager
{
public:
	SMTPManager();
	~SMTPManager();

	void SetServerPort(const int serverPort) { _serverPort = serverPort; }
	void SetSMTPServer(const string& smtpServer) { _smtpServer = smtpServer; }
	void SetDNSAddress(const string& DNSAddress) { _DNSAddress = DNSAddress; }
	void SetEmailTo(const string& emailTo) { _emailTo = emailTo; }
	void SetEmailFrom(const string& emailFrom) { _emailFrom = emailFrom; }
	void SetEmailSubject(const string& emailSubject) { _emailSubject = emailSubject; }
	void SetMessage(const string& message)	{ _message = message; }

	void Transport();

private:
	SOCKET Connect();
	void Disconnect(SOCKET socket);

private:
	string _smtpServer;
	string _DNSAddress;
	string _emailTo;
	string _emailFrom;
	string _emailSubject;
	string _message;

	int _serverPort;
}

