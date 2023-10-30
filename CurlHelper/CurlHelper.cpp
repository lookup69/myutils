#include "CurlHelper.hpp"

/*
        2023-10-23

        UNIT_TEST:
                g++ -DUNIT_TEST -o CurlHelper CurlHelper.cpp -lcurl

*/

using namespace lkup69;
using namespace std;

CurlHelper::CurlHelper()
        : m_curlPtr(curl_easy_init(), curl_easy_cleanup)
{
}

CurlHelper::~CurlHelper()
{
        if (m_headerListPtr)
                curl_slist_free_all(m_headerListPtr);
}

int CurlHelper::SetUrl(const string& url)
{
        if (!m_curlPtr)
                return -1;

        if (curl_easy_setopt(m_curlPtr.get(), CURLOPT_URL, url.c_str()) != CURLE_OK)
                return -1;

        return 0;
}

int CurlHelper::SetTimeout(int sec)
{
        if (!m_curlPtr)
                return -1;

        if (curl_easy_setopt(m_curlPtr.get(), CURLOPT_TIMEOUT, sec) != CURLE_OK)
                return -1;

        return 0;
}

int CurlHelper::AppendHeader(const std::string& header)
{
        m_headerListPtr = curl_slist_append(m_headerListPtr, header.c_str());

        if (!m_headerListPtr)
                return -1;

        return 0;
}

void CurlHelper::ResetHeader(void)
{
        if (m_headerListPtr) {
                curl_slist_free_all(m_headerListPtr);
                m_headerListPtr = nullptr;
        }
}

CURLcode CurlHelper::Get(long& httpCode)
{
        CURLcode res;

        if (m_headerListPtr)
                curl_easy_setopt(m_curlPtr.get(), CURLOPT_HTTPHEADER, m_headerListPtr);

        curl_easy_setopt(m_curlPtr.get(), CURLOPT_FOLLOWLOCATION, 1L);

        res = curl_easy_perform(m_curlPtr.get());
        if (res != CURLE_OK)
                return res;

        return curl_easy_getinfo(m_curlPtr.get(), CURLINFO_RESPONSE_CODE, &httpCode);
}

CURLcode CurlHelper::Post(const string& data, long& httpCode)
{
        CURLcode res;

        curl_easy_setopt(m_curlPtr.get(), CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(m_curlPtr.get(), CURLOPT_POST, 1L);
        curl_easy_setopt(m_curlPtr.get(), CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(m_curlPtr.get(), CURLOPT_POSTFIELDSIZE, data.size());

        res = curl_easy_perform(m_curlPtr.get());
        if (res != CURLE_OK)
                return res;

        return curl_easy_getinfo(m_curlPtr.get(), CURLINFO_RESPONSE_CODE, &httpCode);
}

// g++ -DUNIT_TEST -o CurlHelper CurlHelper.cpp -lcurl
#ifdef UNIT_TEST
#include <stdio.h>

#include <fstream>
class TestRead
{
public:
        size_t Read(void* ptr, size_t size, size_t nmemb)
        {
                size_t real_size = size * nmemb;

                printf(">>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n", __PRETTY_FUNCTION__);
                printf("%s", static_cast<char*>(ptr));
                printf(">>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n", __PRETTY_FUNCTION__);
                printf("\n");

                return real_size;
        }
};

static size_t test_read(void* ptr, size_t size, size_t nmemb, void* userdata)
{
        size_t real_size = size * nmemb;

        printf(">>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n", __PRETTY_FUNCTION__);
        printf("%s", static_cast<char*>(ptr));
        printf(">>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n", __PRETTY_FUNCTION__);
        printf("\n");

        return real_size;
}

static size_t test_read_1(void* ptr, size_t size, size_t nmemb, void* userdata)
{
        size_t real_size = size * nmemb;

        printf(">>>>>>>>>>>>>>>>>>>>> %s(%s) <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n", __PRETTY_FUNCTION__, (char*)userdata);
        printf("%s", static_cast<char*>(ptr));
        printf(">>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n", __PRETTY_FUNCTION__);
        printf("\n");

        return real_size;
}

static size_t test_read_2(void* ptr, size_t size, size_t nmemb, void* userdata)
{
        string* str       = (string*)userdata;
        size_t  real_size = size * nmemb;

        printf(">>>>>>>>>>>>>>>>>>>>> %s(%s) <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n", __PRETTY_FUNCTION__, str->c_str());
        printf("%s", static_cast<char*>(ptr));
        printf(">>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n", __PRETTY_FUNCTION__);
        printf("\n");

        return real_size;
}

int main()
{
#if 0        
        {
                // nornaml function
                CurlHelper CurlHelper;
                long      httpCode;

                CurlHelper.SetUrl("example.com");
                CurlHelper.SetTimeout(10);

                // if not SetWriteFunction will out the respond to the stdout
                // the curl original behavior
                CurlHelper.SetWriteFunction(test_read);
                if (CurlHelper.Get(httpCode) == CURLE_OK)
                        printf("httpCode:%ld\n", httpCode);
        }

        {
                // nornaml function
                CurlHelper CurlHelper;
                long      httpCode;
                char      data[] = "normal function test";

                CurlHelper.SetUrl("example.com");
                CurlHelper.SetTimeout(10);

                // if not SetWriteFunction will out the respond to the stdout
                // the curl original behavior
                CurlHelper.SetWriteFunction(test_read_1, data);
                if (CurlHelper.Get(httpCode) == CURLE_OK)
                        printf("httpCode:%ld\n", httpCode);
        }

        {
                // nornaml function
                CurlHelper CurlHelper;
                long      httpCode;
                string    data = "normal function test";

                CurlHelper.SetUrl("example.com");
                CurlHelper.SetTimeout(10);

                // if not SetWriteFunction will out the respond to the stdout
                // the curl original behavior
                CurlHelper.SetWriteFunction(test_read_2, &data);
                if (CurlHelper.Get(httpCode) == CURLE_OK)
                        printf("httpCode:%ld\n", httpCode);
        }

        {
                // member function
                CurlHelper CurlHelper;
                TestRead  testRead;
                long      httpCode;

                CurlHelper.SetUrl("example.com");
                CurlHelper.SetTimeout(10);

                // if not SetWriteFunction will out the respond to the stdout
                // the curl original behavior
                CurlHelper.SetWriteFunction(testRead, &TestRead::Read);
                if (CurlHelper.Get(httpCode) == CURLE_OK)
                        printf("httpCode:%ld\n", httpCode);
        }

        {
                // lambda function
                auto read__ = [](void* ptr, size_t size, size_t nmemb) -> size_t {
                        size_t real_size = size * nmemb;

                        printf(">>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n", __PRETTY_FUNCTION__);
                        printf("%s", static_cast<char*>(ptr));
                        printf(">>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n", __PRETTY_FUNCTION__);
                        printf("\n");

                        return real_size;
                };
                CurlHelper CurlHelper;
                long      httpCode;

                CurlHelper.SetUrl("example.com");
                CurlHelper.SetTimeout(10);

                // if not SetWriteFunction will out the respond to the stdout
                // the curl original behavior
                CurlHelper.SetWriteFunction(read__);
                if (CurlHelper.Get(httpCode) == CURLE_OK)
                        printf("httpCode:%ld\n", httpCode);
        }
#endif

        // download file
        {
                ofstream ofile;

                // lambda function
                auto download_ = [&ofile](void* ptr, size_t size, size_t nmemb) -> size_t {
                        size_t real_size = size * nmemb;

                        printf(">>>>>>>>>>>>>>>>>>>>> %s  size:%ld <<<<<<<<<<<<<<<<<<<<<<<<<<<\n", __PRETTY_FUNCTION__, real_size);
                        ofile.write(static_cast<const char*>(ptr), real_size);

                        return real_size;
                };

                CurlHelper CurlHelper;
                long       httpCode;

                CurlHelper.SetUrl("https://github.com/lookup69/socketcpp/archive/refs/heads/main.zip");
                CurlHelper.SetTimeout(10);

                ofile.open("test.zip", ios::binary);
                if (ofile.is_open()) {
                        // if not SetWriteFunction will out the respond to the stdout
                        // the curl original behavior
                        CurlHelper.SetWriteFunction(download_);
                        if (CurlHelper.Get(httpCode) == CURLE_OK)
                                printf("httpCode:%ld\n", httpCode);
                }
        }

#if 0        
        {
                string data = "Lorem ipsum dolor sit amet, consectetur adipiscing "
                              "elit. Sed vel urna neque. Ut quis leo metus. Quisque eleifend, ex at "
                              "laoreet rhoncus, odio ipsum semper metus, at tempus ante urna in mauris. "
                              "Suspendisse ornare tempor venenatis. Ut dui neque, pellentesque a varius "
                              "eget, mattis vitae ligula. Fusce ut pharetra est. Ut ullamcorper mi ac "
                              "sollicitudin semper. Praesent sit amet tellus varius, posuere nulla non, "
                              "rhoncus ipsum.";
                // nornaml function
                CurlHelper CurlHelper;
                long       httpCode;

                CurlHelper.SetUrl("https://example.com/index.cgi");
                CurlHelper.SetTimeout(10);

                // if not SetWriteFunction will out the respond to the stdout
                // the curl original behavior
                CurlHelper.SetWriteFunction(test_read);
                if (CurlHelper.Post(data, httpCode) == CURLE_OK)
                        printf("httpCode:%ld\n", httpCode);
        }
#endif

        return 0;
}
#endif
