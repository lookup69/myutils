#include "CurlAgent.hpp"

using namespace lkup69;
using namespace std;

CurlAgent::CurlAgent()
        : m_curlPtr(curl_easy_init(), curl_easy_cleanup)
{
}

CurlAgent::~CurlAgent()
{
        if (m_headerListPtr)
                curl_slist_free_all(m_headerListPtr);
}

int CurlAgent::SetUrl(const string& url)
{
        if (!m_curlPtr)
                return -1;

        if (curl_easy_setopt(m_curlPtr.get(), CURLOPT_URL, url.c_str()) != CURLE_OK)
                return -1;

        return 0;
}

int CurlAgent::SetTimeout(int sec)
{
        if (!m_curlPtr)
                return -1;

        if (curl_easy_setopt(m_curlPtr.get(), CURLOPT_TIMEOUT, sec) != CURLE_OK)
                return -1;

        return 0;
}

int CurlAgent::AppendHeader(const std::string& header)
{
        m_headerListPtr = curl_slist_append(m_headerListPtr, header.c_str());

        if (!m_headerListPtr)
                return -1;

        return 0;
}

void CurlAgent::ResetHeader(void)
{
        if (m_headerListPtr) {
                curl_slist_free_all(m_headerListPtr);
                m_headerListPtr = nullptr;
        }
}

CURLcode CurlAgent::Get(long& httpCode)
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

CURLcode CurlAgent::Post(const string& data, long& httpCode)
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
                CurlAgent curlAgent;
                long      httpCode;

                curlAgent.SetUrl("example.com");
                curlAgent.SetTimeout(10);

                // if not SetWriteFunction will out the respond to the stdout
                // the curl original behavior
                curlAgent.SetWriteFunction(test_read);
                if (curlAgent.Get(httpCode) == CURLE_OK)
                        printf("httpCode:%ld\n", httpCode);
        }

        {
                // nornaml function
                CurlAgent curlAgent;
                long      httpCode;
                char      data[] = "normal function test";

                curlAgent.SetUrl("example.com");
                curlAgent.SetTimeout(10);

                // if not SetWriteFunction will out the respond to the stdout
                // the curl original behavior
                curlAgent.SetWriteFunction(test_read_1, data);
                if (curlAgent.Get(httpCode) == CURLE_OK)
                        printf("httpCode:%ld\n", httpCode);
        }

        {
                // nornaml function
                CurlAgent curlAgent;
                long      httpCode;
                string    data = "normal function test";

                curlAgent.SetUrl("example.com");
                curlAgent.SetTimeout(10);

                // if not SetWriteFunction will out the respond to the stdout
                // the curl original behavior
                curlAgent.SetWriteFunction(test_read_2, &data);
                if (curlAgent.Get(httpCode) == CURLE_OK)
                        printf("httpCode:%ld\n", httpCode);
        }

        {
                // member function
                CurlAgent curlAgent;
                TestRead  testRead;
                long      httpCode;

                curlAgent.SetUrl("example.com");
                curlAgent.SetTimeout(10);

                // if not SetWriteFunction will out the respond to the stdout
                // the curl original behavior
                curlAgent.SetWriteFunction(testRead, &TestRead::Read);
                if (curlAgent.Get(httpCode) == CURLE_OK)
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
                CurlAgent curlAgent;
                long      httpCode;

                curlAgent.SetUrl("example.com");
                curlAgent.SetTimeout(10);

                // if not SetWriteFunction will out the respond to the stdout
                // the curl original behavior
                curlAgent.SetWriteFunction(read__);
                if (curlAgent.Get(httpCode) == CURLE_OK)
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
                CurlAgent curlAgent;
                long      httpCode;

                curlAgent.SetUrl("https://example.com/index.cgi");
                curlAgent.SetTimeout(10);

                // if not SetWriteFunction will out the respond to the stdout
                // the curl original behavior
                curlAgent.SetWriteFunction(test_read);
                if (curlAgent.Post(data, httpCode) == CURLE_OK)
                        printf("httpCode:%ld\n", httpCode);
        }
        return 0;
}
#endif
