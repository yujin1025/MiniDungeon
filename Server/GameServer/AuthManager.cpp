#include "pch.h"
#include "AuthManager.h"
#include <fstream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <random>
#include <cstring>

#define FROM_ADDR "hans4809@gmail.com"// 발신자의 이메일 주소
#define TO_ADDR "hondaestudy@gmail.com"// 수신자의 이메일 주소
#define PASSWORD "ekyz edrh nphx kvge"  // 구글에서 생성한 앱 비밀번호

//static const char* payload_text =
//"To: " TO_ADDR  "\r\n"
//"From: " FROM_ADDR "\r\n"
//"Meesage-ID: <dcd7cb36-11db-487a-9f3a-e652a9458efd@"
//"rfcpedant.gmail.com>\r\n"
//"Subject: SMTP Email Test\r\n"
//"MIME-Version: 1.0\r\n"
//"Content-Type: text/html; charset=UTF-8\r\n"
//"\r\n"
//"<!DOCTYPE html>\r\n"
//"<html xmlns:th=\"http://www.thymeleaf.org\">\r\n"
//"<head>\r\n"
//"<meta charset=\"UTF - 8\">\r\n"
//"<title>이메일 인증 코드</title>\r\n"
//"<style>\r\n"
//"body {"
//"font-family: Arial, sans-serif;"
//"background-color: #f4f4f4;"
//"margin: 0;"
//"padding: 0;"
//"}\r\n"
//".container {"
//"max-width: 600px;"
//"margin: 0 auto;"
//"background-color: #ffffff;"
//"padding: 20px;"
//"border: 1px solid #dddddd;"
//"}\r\n"
//".header {"
//"text-align: center;"
//"background-color: #3498db;"
//"padding: 20px;"
//"color: #ffffff;"
//"}\r\n"
//".content {"
//"margin-top: 20px;"
//"font-size: 16px;"
//"line-height: 1.6;"
//"}\r\n"
//".code {"
//"background-color: #f1f1f1;"
//"padding: 10px;"
//"font-size: 20px;"
//"text-align: center;"
//"font-weight: bold;"
//"margin: 20px 0;"
//"letter-spacing: 2px;"
//"}\r\n"
//".footer {"
//"margin-top: 20px;"
//"font-size: 12px;"
//"color: #777777;"
//"text-align: center;"
//"}\r\n"
//"</style>\r\n"
//"</head>\r\n"
//"<body>\r\n"
//"<div class=\"container\">\r\n"
//"<div class=\"header\">\r\n"
//"<h1>이메일 인증 코드</h1>"
//"</div>\r\n"
//"<div class=\"content\">\r\n"
//"<p>안녕하세요,</p>\r\n"
//"<p>아래의 인증 코드를 입력하여 이메일 인증을 완료해주세요:</p>\r\n"
//"<div class=\"code\" th:text=\"123456\">789012</div>\r\n"
//"<p>이 코드는 5분간 유효합니다.</p>\r\n"
//"</div>\r\n"
//"<hr>\r\n"
//"<div class=\"footer\">\r\n"
//"<p>이 메일은 자동으로 발송된 메일입니다. 회신하지 마세요.</p>\r\n"
//"</div>\r\n"
//"</div>\r\n"
//"</body>\r\n"
//"</html>\r\n";

void AuthManager::AddAuthWaiter(const string& email, const string& password)
{
    WRITE_LOCK;
    AuthInfo info;
    info.password = password;
    info.authNo = GenerateVerificationCode();
    _authWaiters.insert(make_pair(email, info));
}

void AuthManager::CheckAuthWaiter(const string& email, const string& authNum)
{
	WRITE_LOCK;
	auto authUser = _authWaiters.find(email);
	if (authUser != _authWaiters.end())
	{
		if (authUser->second.authNo == stoi(authNum))
		{
            // TODO : 인증 성공 처리
			_authWaiters.erase(authUser);
		}
	}
}

int AuthManager::GenerateVerificationCode()
{
    // 난수 생성기 초기화
    random_device rd; // 실제 하드웨어 엔트로피 소스를 이용한 랜덤 시드
    mt19937 generator(rd()); // Mersenne Twister 난수 생성기
    uniform_int_distribution<int> dist(0, 999999); // 6자리 난수 범위 (0 ~ 999999)

    // 6자리 인증 코드 생성
    int code = dist(generator);

    return code;
}

void AuthManager::SendMail()
{
    //while (true)
    {
        map<string, AuthInfo> authWaitersCopy;

        {
            authWaitersCopy = _authWaiters;
        }

        for(const auto& [toAddr, info] : authWaitersCopy)
		{
			Email email(toAddr, FROM_ADDR, "MiniDungeon", "이메일 인증 코드", info.authNo);
			email.Send();
		}
    }
}

Email::Email(const string& toAddr, const string& fromAddr, const string& nameFrom, const string& subject,int authNum, const string& ccAddr)
{
    _toAddr = toAddr;
    _fromAddr = fromAddr;
    _nameFrom = nameFrom;
    _subject = subject;
    _ccAddr = ccAddr.empty() ? toAddr : ccAddr;
    _authNum = authNum;
    _htmlFormat = "<!DOCTYPE html>\r\n"
                    "<html xmlns:th=\"http://www.thymeleaf.org\">\r\n"
                    "<head>\r\n"
                    "<meta charset=\"UTF - 8\">\r\n"
                    "<title>이메일 인증 코드</title>\r\n"
                    "<style>\r\n"
                    "body {"
                    "font-family: Arial, sans-serif;"
                    "background-color: #f4f4f4;"
                    "margin: 0;"
                    "padding: 0;"
                    "}\r\n"
                    ".container {"
                    "max-width: 600px;"
                    "margin: 0 auto;"
                    "background-color: #ffffff;"
                    "padding: 20px;"
                    "border: 1px solid #dddddd;"
                    "}\r\n"
                    ".header {"
                    "text-align: center;"
                    "background-color: #3498db;"
                    "padding: 20px;"
                    "color: #ffffff;"
                    "}\r\n"
                    ".content {"
                    "margin-top: 20px;"
                    "font-size: 16px;"
                    "line-height: 1.6;"
                    "}\r\n"
                    ".code {"
                    "background-color: #f1f1f1;"
                    "padding: 10px;"
                    "font-size: 20px;"
                    "text-align: center;"
                    "font-weight: bold;"
                    "margin: 20px 0;"
                    "letter-spacing: 2px;"
                    "}\r\n"
                    ".footer {"
                    "margin-top: 20px;"
                    "font-size: 12px;"
                    "color: #777777;"
                    "text-align: center;"
                    "}\r\n"
                    "</style>\r\n"
                    "</head>\r\n"
                    "<body>\r\n"
                    "<div class=\"container\">\r\n"
                    "<div class=\"header\">\r\n"
                    "<h1>이메일 인증 코드</h1>"
                    "</div>\r\n"
                    "<div class=\"content\">\r\n"
                    "<p>안녕하세요,</p>\r\n"
                    "<p>아래의 인증 코드를 입력하여 이메일 인증을 완료해주세요:</p>\r\n"
                    "<div class=\"code\">" + to_string(_authNum) + "</div>\r\n"
                    "<p>이 코드는 5분간 유효합니다.</p>\r\n"
                    "</div>\r\n"
                    "<hr>\r\n"
                    "<div class=\"footer\">\r\n"
                    "<p>이 메일은 자동으로 발송된 메일입니다. 회신하지 마세요.</p>\r\n"
                    "</div>\r\n"
                    "</div>\r\n"
                    "</body>\r\n"
                    "</html>\r\n";
}

string Email::DateTimeNow() const
{
    const int RFC5322_TIME_LEN = 32;

    std::string ret;
    ret.resize(RFC5322_TIME_LEN);

    time_t tt;

#ifdef _MSC_VER
    // MSVC 환경에서는 localtime_s 사용
    time(&tt);
    tm t;
    localtime_s(&t, &tt);
#else
    tm tv, * t = &tv;
    tt = time(&tt);
    localtime_r(&tt, t);
#endif

    strftime(&ret[0], RFC5322_TIME_LEN, "%a, %d %b %Y %H:%M:%S %z", &t);

    return ret;
}

CURLcode Email::Send()
{
    CURLcode ret = CURLE_OK;

    struct curl_slist* recipients = NULL;

    CURL* curl = curl_easy_init();

    StringData textData{ SetPayloadText() };

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_USERNAME, "hans4809@gmail.com");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "ekyz edrh nphx kvge");
        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");

        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        //curl_easy_setopt(curl, CURLOPT_CAINFO, "/path/to/certificate.pem");

        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, ("<" + _fromAddr + ">").c_str());
        recipients = curl_slist_append(recipients, ("<" + _toAddr + ">").c_str());
        recipients = curl_slist_append(recipients, ("<" + _ccAddr + ">").c_str());

        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &textData);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        ret = curl_easy_perform(curl);

        if (ret != CURLE_OK)
        {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(ret) << "\n";
        }

        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }

    return ret;
}

string Email::SetPayloadText()
{
    string payloadText = "Date: " + DateTimeNow() + "\r\n"
        "To: <" + _toAddr + ">\r\n"
        "From: \"" + _nameFrom + "\" <" + _fromAddr + ">\r\n";

    if (_ccAddr.length()) 
    {
        payloadText += "Cc: <" + _ccAddr + ">\r\n";
    }
    payloadText += "Message-ID: <" + GenerateMessageID() + "@" + _fromAddr.substr(_fromAddr.find('@') + 1) + ">\r\n"
        "Subject: " + _subject + "\r\n"
        "MIME-Version: 1.0\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "\r\n"
        + _htmlFormat + "\r\n"
        "\r\n";
    return payloadText;
}

string Email::GenerateMessageID() const
{
    const size_t MESSAGE_ID_LEN = 37;
    tm t;
    time_t tt;
    time(&tt);
#ifdef _MSC_VER
    gmtime_s(&t, &tt);
#else
    gmtime_r(&tt, &t);
#endif
    std::string ret;
    ret.resize(MESSAGE_ID_LEN);
    size_t datelen = std::strftime(&ret[0], MESSAGE_ID_LEN, "%Y%m%d%H%M%S", &t);
    static const std::string alphaNum{
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz" };
    std::mt19937 gen;
    std::uniform_int_distribution<> dis(0, alphaNum.length() - 1);
    std::generate_n(ret.begin() + datelen, MESSAGE_ID_LEN - datelen, [&]() { return alphaNum[dis(gen)]; });
    return ret;
}

size_t Email::payload_source(void* ptr, size_t size, size_t nmemb, void* userp)
{
    StringData* text = reinterpret_cast<StringData*>(userp);

    if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1) || (text->bytesleft == 0)) {
        return 0;
    }

    if ((nmemb * size) >= text->msg.size()) {
        text->bytesleft = 0;
        return text->msg.copy(reinterpret_cast<char*>(ptr), text->msg.size());
    }

    return 0;
}
