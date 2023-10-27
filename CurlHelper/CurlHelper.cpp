#include "CurlHelper.hpp"

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

struct WriteThis {
        const char* readptr;
        size_t      sizeleft;
};
#if 0
static size_t read_callback(char* dest, size_t size, size_t nmemb, void* userp)
{
        struct WriteThis* wt          = (struct WriteThis*)userp;
        size_t            buffer_size = size * nmemb;

        if (wt->sizeleft) {
                /* copy as much as possible from the source to the destination */
                size_t copy_this_much = wt->sizeleft;
                if (copy_this_much > buffer_size)
                        copy_this_much = buffer_size;
                memcpy(dest, wt->readptr, copy_this_much);

                wt->readptr += copy_this_much;
                wt->sizeleft -= copy_this_much;
                return copy_this_much; /* we copied this many bytes */
        }

        return 0; /* no more data left to deliver */
}
#endif

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
                long      httpCode;

                CurlHelper.SetUrl("https://example.com/index.cgi");
                CurlHelper.SetTimeout(10);

                // if not SetWriteFunction will out the respond to the stdout
                // the curl original behavior
                CurlHelper.SetWriteFunction(test_read);
                if (CurlHelper.Post(data, httpCode) == CURLE_OK)
                        printf("httpCode:%ld\n", httpCode);
        }
        return 0;
}
#endif
