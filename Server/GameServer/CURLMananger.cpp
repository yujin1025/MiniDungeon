#include "pch.h"
#include "CURLMananger.h"
#include <fstream>
#include <curl/curl.h>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <random>

#define FROM_ADDR "hans4809@gmail.com"// �߽����� �̸��� �ּ�
#define TO_ADDR "hondaestudy@gmail.com"// �������� �̸��� �ּ�
#define PASSWORD "ekyz edrh nphx kvge"  // ���ۿ��� ������ �� ��й�ȣ

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
    "<title>�̸��� ���� �ڵ�</title>\r\n"
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
    "<h1>�̸��� ���� �ڵ�</h1>"
    "</div>\r\n"
    "<div class=\"content\">\r\n"
    "<p>�ȳ��ϼ���,</p>\r\n"
    "<p>�Ʒ��� ���� �ڵ带 �Է��Ͽ� �̸��� ������ �Ϸ����ּ���:</p>\r\n"
    "<div class=\"code\" th:text=\"123456\">789012</div>\r\n"
    "<p>�� �ڵ�� 5�а� ��ȿ�մϴ�.</p>\r\n"
    "</div>\r\n"
    "<hr>\r\n"
    "<div class=\"footer\">\r\n"
    "<p>�� ������ �ڵ����� �߼۵� �����Դϴ�. ȸ������ ������.</p>\r\n"
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


string CURLMananger::GenerateMessageID(const string& domain)
{
    //time_t now = time(nullptr);
    //tm now_tm;

    //// gmtime_s ����Ͽ� �ð� ��ȯ (������ ����)
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

    //// ���� ������ ����
    //random_device rd;
    //mt19937 generator(rd());
    //uniform_int_distribution<int> dist(1000, 9999);
    //int randomValue = dist(generator);

    // Message-ID ���ڿ� ����
    ostringstream messageID;
    messageID << "dcd7cb36-11db-487a-9f3a-e652a9458efd" << "@" << domain << ">";

    return messageID.str();
}

string CURLMananger::SetPayLoadText()
{
    string return_string = "To: ";
    //_payload_string = "To: ";
    return_string.append(_toAddr.c_str());
    return_string.append("\r\n");

    return_string.append("From: ");
    return_string.append(FROM_ADDR);
    return_string.append("\r\n");

    return_string.append("Meesage-ID: ");
    _messageID = GenerateMessageID("rfcpedant.gmail.com");
    return_string.append(_messageID.c_str());
    return_string.append("\r\n");

    return_string.append("Subject: �̸��� ���� �ڵ�\r\n");
    return_string.append("MIME-Version: 1.0\r\n");
    return_string.append("Content-Type: text/html; charset=UTF-8\r\n");
    return_string.append("\r\n");

    return_string.append("<!DOCTYPE html>\r\n");
    return_string.append("<html xmlns:th=\"http://www.thymeleaf.org\">");

    return_string.append("<head>\r\n");
    return_string.append("<meta charset=\"UTF-8\">\r\n");
    return_string.append("<title>�̸��� ���� �ڵ�</title>\r\n");
    return_string.append("<style>\r\n");
    return_string.append("body {");
    return_string.append("font-family: Arial, sans-serif;");
    return_string.append("background-color: #f4f4f4;");
    return_string.append("margin: 0;");
    return_string.append("padding: 0;}\r\n");

    return_string.append(".container {");
    return_string.append("max-width: 600px;");
    return_string.append("margin: 0 auto;");
    return_string.append("background-color: #ffffff;");
    return_string.append("padding: 20px;");
    return_string.append("border: 1px solid #dddddd;}\r\n");

    return_string.append(".header {");
    return_string.append("text-align: center;");
    return_string.append("background-color: #3498db;");
    return_string.append("padding: 20px;");
    return_string.append("color: #ffffff;}\r\n");

    return_string.append(".content {");
    return_string.append("margin-top: 20px;");
    return_string.append("font-size: 16px;");
    return_string.append("line-height: 1.6;}\r\n");

    return_string.append(".code {");
    return_string.append("background-color: #f1f1f1;");
    return_string.append("padding: 10px;");
    return_string.append("font-size: 20px;");
    return_string.append("text-align: center;");
    return_string.append("font-weight: bold;");
    return_string.append("margin: 20px 0;");
    return_string.append("letter-spacing: 2px;}\r\n");

    return_string.append(".footer {");
    return_string.append("margin-top: 20px;");
    return_string.append("font-size: 12px;");
    return_string.append("color: #777777;");
    return_string.append("text-align: center;}\r\n");

    return_string.append("</style>\r\n");
    return_string.append("</head>\r\n");

    return_string.append("<body>\r\n");
    return_string.append("<div class=\"container\">\r\n");
    return_string.append("<div class=\"header\">\r\n");
    return_string.append("<h1>�̸��� ���� �ڵ�</h1>\r\n");
    return_string.append("</div>\r\n");

    return_string.append("<div class=\"content\">");
    return_string.append("<p>�Ʒ��� ���� �ڵ带 �Է����ּ���.</p>\r\n");
    return_string.append("<div class=\"code\">");
    return_string.append(GenerateVerificationCode());
    return_string.append("</div>\r\n");
    return_string.append("<p>�� �ڵ�� 5�а� ��ȿ�մϴ�.</p>\r\n");
    return_string.append("<p>�����մϴ�.</p>\r\n");
    return_string.append("</div>\r\n");
    return_string.append("<hr>\r\n");

    return_string.append("<div class=\"footer\">\r\n");
    return_string.append("<p>�� ������ �ڵ����� �߼۵� �����Դϴ�. ȸ������ ������.</p>\r\n");
    return_string.append("</div>\r\n");
    return_string.append("</div>\r\n");
    return_string.append("</body>\r\n");
    return_string.append("</html>\r\n");

    return return_string;
}

string CURLMananger::GenerateVerificationCode()
{
    // ���� ������ �ʱ�ȭ
    random_device rd; // ���� �ϵ���� ��Ʈ���� �ҽ��� �̿��� ���� �õ�
    mt19937 generator(rd()); // Mersenne Twister ���� ������
    uniform_int_distribution<int> dist(0, 999999); // 6�ڸ� ���� ���� (0 ~ 999999)

    // 6�ڸ� ���� �ڵ� ����
    int code = dist(generator);

    // 6�ڸ��� ������ �������� ���ڿ� ����
    ostringstream codeStream;
    codeStream << setw(6) << setfill('0') << code;

    return codeStream.str();
}

CURLMananger::CURLMananger()
{
    _toAddr = "hondaestudy@gmail.com";
    _messageID = GenerateMessageID("gmail.com");
}

CURLMananger::~CURLMananger()
{
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
        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465"); // ��Ʈ 465 ���
        // �⺻ �̸��� ����
        curl_easy_setopt(curl, CURLOPT_USERNAME, FROM_ADDR);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

        // �ʿ� �� ������ ���� ��Ȱ��ȭ (�׽�Ʈ ȯ�濡���� ���)
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        // �߽��� �̸��� ����
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "<hans4809@gmail.com>");

		// ������ �̸��� ����
        //string recipient = "<";
        //recipient.append(toAddr);
        //recipient.append(">");
        //recipients = curl_slist_append(recipients, recipient.c_str());
        recipients = curl_slist_append(recipients, "<hondaestudy@gmail.com>");
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        //string payload_text = SetPayLoadText();
        //string payload = payload_char;

        // �̸��� ���� ����
        //curl_easy_setopt(curl, CURLOPT_READDATA, &payload_text);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        
        res = curl_easy_perform(curl);

        // ��� Ȯ��
        if (res != CURLE_OK) 
        {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }
        else 
        {
            cout << "Email sent successfully!" << endl;
        }

        // �޸� ����
        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }
}