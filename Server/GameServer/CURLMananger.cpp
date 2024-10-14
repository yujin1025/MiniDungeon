#include "pch.h"
#include "CURLMananger.h"
#include <fstream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <random>
#include <cstring>

#define FROM_ADDR "hans4809@gmail.com"// 발신자의 이메일 주소
#define TO_ADDR "hondaestudy@gmail.com"// 수신자의 이메일 주소
#define PASSWORD "ekyz edrh nphx kvge"  // 구글에서 생성한 앱 비밀번호

static const char* payload_text =
    "To: " TO_ADDR  "\r\n"
    "From: " FROM_ADDR "\r\n"
    "Meesage-ID: <dcd7cb36-11db-487a-9f3a-e652a9458efd@"
    "rfcpedant.gmail.com>\r\n"
    "Subject: SMTP Email Test\r\n"
    "MIME-Version: 1.0\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "\r\n"
    "<!DOCTYPE html>\r\n"
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
    "<div class=\"code\" th:text=\"123456\">789012</div>\r\n"
    "<p>이 코드는 5분간 유효합니다.</p>\r\n"
    "</div>\r\n"
    "<hr>\r\n"
    "<div class=\"footer\">\r\n"
    "<p>이 메일은 자동으로 발송된 메일입니다. 회신하지 마세요.</p>\r\n"
    "</div>\r\n"
    "</div>\r\n"
    "</body>\r\n"
    "</html>\r\n";

struct upload_status 
{
    size_t bytes_read;
};

static size_t payload_source(char* ptr, size_t size, size_t nmemb, void* userp)
{
    struct upload_status* upload_ctx = (struct upload_status*)userp;
    const char* data;
    size_t room = size * nmemb;

    if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1))
    {
        return 0;
    }
    data = &payload_text[upload_ctx->bytes_read];

    if (data) {
        size_t len = strlen(data);
        if (room < len)
            len = room;
        memcpy(ptr, data, len);
        upload_ctx->bytes_read += len;

        return len;
    }

    return 0;
}


void CURLMananger::AddAuthWaiter(const string& email, const string& password)
{
    WRITE_LOCK;
	AuthInfo info;
	info.password = password;
	info.authNo = GenerateVerificationCode();
	_authWaiters.insert(make_pair(email, info));
}

string CURLMananger::GenerateMessageID(const string& domain)
{
    //time_t now = time(nullptr);
    //tm now_tm;

    //// gmtime_s 사용하여 시간 변환 (스레드 안전)
    //if (gmtime_s(&now_tm, &now) == 0) 
    //{
    //    cout << "Year: " << (now_tm.tm_year + 1900) << endl;
    //    cout << "Month: " << (now_tm.tm_mon + 1) << endl;
    //    cout << "Day: " << now_tm.tm_mday << endl;
    //}
    //else 
    //{
    //    cerr << "Failed to convert time to GMT" << endl;
    //}
    //ostringstream timeStream;
    //timeStream << put_time(&now_tm, "%Y%m%d%H%M%S");

    //// 난수 생성기 설정
    //random_device rd;
    //mt19937 generator(rd());
    //uniform_int_distribution<int> dist(1000, 9999);
    //int randomValue = dist(generator);

    // Message-ID 문자열 생성
    ostringstream messageID;
    messageID << "dcd7cb36-11db-487a-9f3a-e652a9458efd" << "@" << domain << ">";

    return messageID.str();
}

string CURLMananger::SetPayLoadText(const string& email, AuthInfo info)
{
    _payload = "To: ";
    _payload.append(email.c_str());
    _payload.append("\r\n");

    _payload.append("From: ");
    _payload.append(FROM_ADDR);
    _payload.append("\r\n");

    _payload.append("Meesage-ID: ");
    _messageID = GenerateMessageID("rfcpedant.gmail.com");
    _payload.append(_messageID.c_str());
    _payload.append("\r\n");

    _payload.append("Subject: 이메일 인증 코드\r\n");
    _payload.append("MIME-Version: 1.0\r\n");
    _payload.append("Content-Type: text/html; charset=UTF-8\r\n");
    _payload.append("\r\n");

    _payload.append("<!DOCTYPE html>\r\n");
    _payload.append("<html xmlns:th=\"http://www.thymeleaf.org\">");

    _payload.append("<head>\r\n");
    _payload.append("<meta charset=\"UTF-8\">\r\n");
    _payload.append("<title>이메일 인증 코드</title>\r\n");
    _payload.append("<style>\r\n");
    _payload.append("body {");
    _payload.append("font-family: Arial, sans-serif;");
    _payload.append("background-color: #f4f4f4;");
    _payload.append("margin: 0;");
    _payload.append("padding: 0;}\r\n");

    _payload.append(".container {");
    _payload.append("max-width: 600px;");
    _payload.append("margin: 0 auto;");
    _payload.append("background-color: #ffffff;");
    _payload.append("padding: 20px;");
    _payload.append("border: 1px solid #dddddd;}\r\n");

    _payload.append(".header {");
    _payload.append("text-align: center;");
    _payload.append("background-color: #3498db;");
    _payload.append("padding: 20px;");
    _payload.append("color: #ffffff;}\r\n");

    _payload.append(".content {");
    _payload.append("margin-top: 20px;");
    _payload.append("font-size: 16px;");
    _payload.append("line-height: 1.6;}\r\n");

    _payload.append(".code {");
    _payload.append("background-color: #f1f1f1;");
    _payload.append("padding: 10px;");
    _payload.append("font-size: 20px;");
    _payload.append("text-align: center;");
    _payload.append("font-weight: bold;");
    _payload.append("margin: 20px 0;");
    _payload.append("letter-spacing: 2px;}\r\n");

    _payload.append(".footer {");
    _payload.append("margin-top: 20px;");
    _payload.append("font-size: 12px;");
    _payload.append("color: #777777;");
    _payload.append("text-align: center;}\r\n");

    _payload.append("</style>\r\n");
    _payload.append("</head>\r\n");

    _payload.append("<body>\r\n");
    _payload.append("<div class=\"container\">\r\n");
    _payload.append("<div class=\"header\">\r\n");
    _payload.append("<h1>이메일 인증 코드</h1>\r\n");
    _payload.append("</div>\r\n");

    _payload.append("<div class=\"content\">");
    _payload.append("<p>아래의 인증 코드를 입력해주세요.</p>\r\n");
    _payload.append("<div class=\"code\">");
    _payload.append(to_string(info.authNo));
    _payload.append("</div>\r\n");
    _payload.append("<p>이 코드는 5분간 유효합니다.</p>\r\n");
    _payload.append("<p>감사합니다.</p>\r\n");
    _payload.append("</div>\r\n");
    _payload.append("<hr>\r\n");

    _payload.append("<div class=\"footer\">\r\n");
    _payload.append("<p>이 메일은 자동으로 발송된 메일입니다. 회신하지 마세요.</p>\r\n");
    _payload.append("</div>\r\n");
    _payload.append("</div>\r\n");
    _payload.append("</body>\r\n");
    _payload.append("</html>\r\n");
}

int CURLMananger::GenerateVerificationCode()
{
    // 난수 생성기 초기화
    random_device rd; // 실제 하드웨어 엔트로피 소스를 이용한 랜덤 시드
    mt19937 generator(rd()); // Mersenne Twister 난수 생성기
    uniform_int_distribution<int> dist(0, 999999); // 6자리 난수 범위 (0 ~ 999999)

    // 6자리 인증 코드 생성
    int code = dist(generator);

    return code;
}

CURLMananger::CURLMananger()
{
    _connection = curl_easy_init();
    curl_easy_setopt(_connection, CURLOPT_URL, "smtps://smtp.gmail.com:465"); // 포트 465 사용
    // 기본 이메일 설정
    curl_easy_setopt(_connection, CURLOPT_USERNAME, FROM_ADDR);
    curl_easy_setopt(_connection, CURLOPT_PASSWORD, PASSWORD);
    curl_easy_setopt(_connection, CURLOPT_USE_SSL, CURLUSESSL_ALL);

    // 필요 시 인증서 검증 비활성화 (테스트 환경에서만 사용)
    curl_easy_setopt(_connection, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(_connection, CURLOPT_SSL_VERIFYHOST, 0L);

    // 발신자 이메일 설정
    curl_easy_setopt(_connection, CURLOPT_MAIL_FROM, "<hans4809@gmail.com>");
}

CURLMananger::~CURLMananger()
{
    curl_easy_cleanup(_connection);
}

void CURLMananger::SendMail()
{
    ofstream report;
    CURL* curl;
    CURLcode res;

    struct curl_slist* recipients = NULL;
    struct upload_status upload_ctx = { 0 };

    curl = curl_easy_init();

    report.open("SMTP.txt");
    if (report.fail())
    {
        throw runtime_error("SMTP.txt failed.");
    }
    if(!report.is_open())
	{
        throw runtime_error("SMTP.txt can not open the file.");
	}

    if (curl) 
    {
        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465"); // 포트 465 사용
        // 기본 이메일 설정
        curl_easy_setopt(curl, CURLOPT_USERNAME, FROM_ADDR);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

        // 필요 시 인증서 검증 비활성화 (테스트 환경에서만 사용)
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        // 발신자 이메일 설정
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "<hans4809@gmail.com>");

		// 수신자 이메일 설정
        //string recipient = "<";
        //recipient.append(toAddr);
        //recipient.append(">");
        //recipients = curl_slist_append(recipients, recipient.c_str());
        recipients = curl_slist_append(recipients, "<hondaestudy@gmail.com>");
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        //string payload_text = SetPayLoadText();
        //string payload = payload_char;

        // 이메일 본문 설정
        //curl_easy_setopt(curl, CURLOPT_READDATA, &payload_text);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        
        res = curl_easy_perform(curl);

        // 결과 확인
        if (res != CURLE_OK) 
        {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }
        else 
        {
            cout << "Email sent successfully!" << endl;
        }

        // 메모리 해제
        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }
}

void CURLMananger::SendMail(const string& toAddr, const string& password)
{
    AddAuthWaiter(toAddr, password);

    if (_connection != nullptr)
    {
        WRITE_LOCK;

        CURLcode res;
        struct curl_slist* recipients = NULL;
        struct upload_status upload_ctx = { 0 };

        // 수신자 이메일 설정
        string recipient = "<";
        recipient.append(toAddr);
        recipient.append(">");
        recipients = curl_slist_append(recipients, recipient.c_str());
        curl_easy_setopt(_connection, CURLOPT_MAIL_RCPT, recipients);

        if(_authWaiters.find(toAddr) != _authWaiters.end())
		{
			AuthInfo info = _authWaiters[toAddr];
			SetPayLoadText(toAddr, info);

			// 이메일 본문 설정
			curl_easy_setopt(_connection, CURLOPT_READFUNCTION, payload_source);
			curl_easy_setopt(_connection, CURLOPT_READDATA, &upload_ctx);
			curl_easy_setopt(_connection, CURLOPT_UPLOAD, 1L);

			res = curl_easy_perform(_connection);

            //Stringdata textdata{ SetPayloadText };

			// 결과 확인
			if (res != CURLE_OK)
			{
				cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
			}
			else
			{
				cout << "Email sent successfully!" << endl;
			}

			// 메모리 해제
			curl_slist_free_all(recipients);
		}
    }
}

Email::Email(const string& toAddr, const string& fromAddr, const string& nameFrom, const string& subject, const string& message, const string& ccAddr)
{
    _toAddr = toAddr;
	_fromAddr = fromAddr;
	_nameFrom = nameFrom;
	_subject = subject;
	_message = message;
	_ccAddr = ccAddr.empty() ? toAddr : ccAddr;
}

string Email::DateTimeNow() const
{
    const int RFC5322_TIME_LEN = 32;

    std::string ret;
    ret.resize(RFC5322_TIME_LEN);

    time_t tt;

#ifdef _MSC_VER
    time(&tt);
    tm* t = localtime(&tt);
#else
    tm tv, * t = &tv;
    tt = time(&tt);
    localtime_r(&tt, t);
#endif

    strftime(&ret[0], RFC5322_TIME_LEN, "%a, %d %b %Y %H:%M:%S %z", t);

    return ret;
}

CURLcode Email::Send(const string& url, const string& userName, const string& password)
{
    CURLcode ret = CURLE_OK;

    struct curl_slist* recipients = NULL;

    CURL* curl = curl_easy_init();

    StringData textData{ SetPayloadText() };

    if (curl) 
    {
        curl_easy_setopt(curl, CURLOPT_USERNAME, userName.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

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
    if (_ccAddr.length()) {
        payloadText += "Cc: <" + _ccAddr + ">\r\n";
    }
    payloadText += "Message-ID: <" + GenerateMessageID() + "@" + _fromAddr.substr(_fromAddr.find('@') + 1) + ">\r\n"
        "Subject: " + _subject + "\r\n"
        "\r\n"
        + _message + "\r\n"
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
