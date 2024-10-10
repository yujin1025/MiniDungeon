#include "pch.h"
#include "CURLMananger.h"
#include <fstream>
#include <curl/curl.h>

#define FROM_ADDR "hans4809@gmail.com"// 발신자의 이메일 주소
#define TO_ADDR "hondaestudy@gmail.com"// 수신자의 이메일 주소
#define PASSWORD "ekyz edrh nphx kvge"  // 구글에서 생성한 앱 비밀번호

static const char* payload_text =
    "Date: Mon, 29 Nov 2010 21:54:29 +1100\r\n"
    "To: " TO_ADDR "\r\n"
    "From: " FROM_ADDR "\r\n"
    "Meesage-ID: <dcd7cb36-11db-487a-9f3a-e652a9458efd@"
    "rfcpedant.gmail.com>\r\n"
    "Subject: SMTP Email Test\r\n"
    "\r\n"
    "This is a test email sent using libcurl.\r\n"
    "\r\n";

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

        recipients = curl_slist_append(recipients, "<hondaestudy@gmail.com>");
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        // 이메일 본문 설정
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