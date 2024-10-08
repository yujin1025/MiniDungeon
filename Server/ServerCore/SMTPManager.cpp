#include "pch.h"
#include "SMTPManager.h"

SMTPManager::SMTPManager()
{
}

SMTPManager::~SMTPManager()
{
}

void SMTPManager::Transport()
{
	std::ofstream report;
	SOCKET serverSocket;
	char recvBuffer[0x200];
	char sendBuffer[0x200];
	int recvBytes;

	/// Packet Log File
	report.open("SMTP.txt");
	if (report.fail())
	{
		throw runtime_error("SMTP.txt failed");
	}

	if (!report.is_open())
	{
		throw runtime_error("SMTP.txt is not open");
	}

	serverSocket = Connect();

	/// EHLO
	recvBytes = recv(serverSocket, recvBuffer, sizeof(recvBuffer), 0);
	recvBuffer[recvBytes] = '\0';
	report << recvBuffer;
	sprintf(sendBuffer, "ehlo %s\r\n", _DNSAddress.c_str());
	report << sendBuffer;
	send(serverSocket, sendBuffer, strlen(sendBuffer), 0);

	/// MAIL FROM
	recvBytes = recv(serverSocket, recvBuffer, sizeof(recvBuffer), 0);
	recvBuffer[recvBytes] = '\0';
	report << recvBuffer;
	sprintf(sendBuffer, "mail from: <%s>\r\n", _emailFrom.c_str());
	report << sendBuffer;
	send(serverSocket, sendBuffer, strlen(sendBuffer), 0);

	/// RCPT TO
	recvBytes = recv(serverSocket, recvBuffer, sizeof(recvBuffer), 0);
	recvBuffer[recvBytes] = '\0';
	report << recvBuffer;
	sprintf(sendBuffer, "rcpt to:<%s>\r\n", _emailTo.c_str());
	report << sendBuffer;
	send(serverSocket, sendBuffer, (int)strlen(sendBuffer), 0);

	/// DATA
	recvBytes = recv(serverSocket, recvBuffer, sizeof(recvBuffer), 0);
	recvBuffer[recvBytes] = '\0';
	report << recvBuffer;
	sprintf(sendBuffer, "data\r\n");
	report << sendBuffer;
	send(serverSocket, sendBuffer, static_cast<int>(strlen(sendBuffer)), 0);

	/// SUBJECT
	recvBytes = recv(serverSocket, recvBuffer, sizeof(recvBuffer), 0);
	recvBuffer[recvBytes] = '\0';
	report << recvBuffer;
	sprintf(sendBuffer, "To:%s \nFrom:%s \nSubject%s\r\n\r\n%s\r\n.\r\n", _emailTo.c_str(), _emailFrom.c_str(), _emailSubject.c_str(), _message.c_str());
	report << sendBuffer;
	send(serverSocket, sendBuffer, static_cast<int>(strlen(sendBuffer)), 0);

	/// QUIT
	recvBytes = recv(serverSocket, recvBuffer, sizeof(recvBuffer), 0);
	recvBuffer[recvBytes] = '\0';
	report << recvBuffer;
	sprintf(sendBuffer, "quit\r\n");
	report << sendBuffer;
	send(serverSocket, sendBuffer, static_cast<int>(strlen(sendBuffer)), 0);

	recvBytes = recv(serverSocket, recvBuffer, sizeof(recvBuffer), 0);
	recvBuffer[recvBytes] = '\0';
	report << recvBuffer;

	Disconnect(serverSocket);

	report.close();
}

SOCKET SMTPManager::Connect()
{
	WSADATA wsaData;
	SOCKET serverSocket;
	SOCKADDR_IN serverAddrIn;
	hostent* host;
	int result;

	result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (result != 0)
	{
		throw runtime_error("WSAStartup failed");
	}

	serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	if(serverSocket == INVALID_SOCKET)
	{
		throw runtime_error("socket failed");
	}

	memset(&serverAddrIn, 0, sizeof(serverAddrIn));

	host = gethostbyname(_smtpServer.c_str());
	if (host == nullptr)
	{
		throw runtime_error("gethostbyname failed");
	}

	memcpy(&(serverAddrIn.sin_addr), host->h_addr, host->h_length);
	serverAddrIn.sin_family = host->h_addrtype;
	serverAddrIn.sin_port = htons(_serverPort);

	result = connect(serverSocket, (SOCKADDR*)&serverAddrIn, sizeof(serverAddrIn));
	if (result == SOCKET_ERROR)
	{
		throw runtime_error("connect failed");
	}

	return serverSocket;
}

void SMTPManager::Disconnect(SOCKET socket)
{
	closesocket(socket);
	WSACleanup();
}
