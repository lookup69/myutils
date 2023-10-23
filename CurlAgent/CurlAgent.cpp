#include "CurlAgent.hpp"

using namespace lkup69;
using namespace std;

CurlAgent::CurlAgent()
{
        m_curlPtr = unique_ptr<CURL, CurlDeleter>(curl_easy_init());
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

int main()
{
        {
                // nornaml function
                CurlAgent curlAgent;

                curlAgent.SetUrl("example.com");
                curlAgent.SetTimeout(10);
                curlAgent.Execute(test_read);
        }

        {
                // member function
                CurlAgent curlAgent;
                TestRead  testRead;

                curlAgent.SetUrl("example.com");
                curlAgent.SetTimeout(10);
                // curlAgent.Execute(&TestRead::Read, testRead);
                curlAgent.Execute(testRead, &TestRead::Read);
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
                TestRead  testRead;

                curlAgent.SetUrl("example.com");
                curlAgent.SetTimeout(10);
                curlAgent.Execute(read__);
        }
}
#endif
