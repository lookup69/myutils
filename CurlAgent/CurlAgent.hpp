#pragma once

#include <curl/curl.h>

#include <string>
#include <memory>
#include <functional>

namespace lkup69
{

struct CurlDeleter {
        void operator()(CURL *ptr)
        {
                if (ptr)
                        curl_easy_cleanup(ptr);
        }
};

class CurlAgent
{
public:
        CurlAgent();
        ~CurlAgent() = default;

        int SetUrl(const std::string &url);
        int SetTimeout(int sec);

        template <typename CLASS, typename MFUNC = int>
        int Execute(CLASS &klass, MFUNC mfunc = 0)
        {
                if constexpr (std::is_function_v<CLASS>) {
                        curl_easy_setopt(m_curlPtr.get(), CURLOPT_WRITEFUNCTION, &klass);

                        CURLcode res = curl_easy_perform(m_curlPtr.get());
                        if (res != CURLE_OK)
                                return -1;

                        return 0;
                } else if constexpr (std::is_class_v<CLASS>) {
                        if constexpr (!std::is_integral_v<MFUNC>) {
                                std::function<size_t(void *, size_t, size_t)> func;

                                func = std::bind(mfunc, klass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
                                curl_easy_setopt(m_curlPtr.get(), CURLOPT_WRITEFUNCTION, &CurlAgent::WriteResponseWrap_);
                                curl_easy_setopt(m_curlPtr.get(), CURLOPT_WRITEDATA, &func);

                                CURLcode res = curl_easy_perform(m_curlPtr.get());
                                if (res != CURLE_OK)
                                        return -1;
                        } else {
                                std::function<size_t(void *, size_t, size_t)> func;

                                func = std::bind(klass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
                                curl_easy_setopt(m_curlPtr.get(), CURLOPT_WRITEFUNCTION, &CurlAgent::WriteResponseWrap_);
                                curl_easy_setopt(m_curlPtr.get(), CURLOPT_WRITEDATA, &func);

                                CURLcode res = curl_easy_perform(m_curlPtr.get());
                                if (res != CURLE_OK)
                                        return -1;
                        }

                        return 0;
                }

                return -1;
        }

private:
        static size_t WriteResponseWrap_(void *ptr, size_t size, size_t nmemb, void *userdata)
        {
                std::function<size_t(void *, size_t, size_t)> *callback = static_cast<std::function<size_t(void *, size_t, size_t)> *>(userdata);

                return (*callback)(ptr, size, nmemb);
        }

private:
        std::unique_ptr<CURL, CurlDeleter> m_curlPtr;
};
}  // namespace lkup69