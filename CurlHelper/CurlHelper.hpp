#pragma once

#include <assert.h>
#include <curl/curl.h>

#include <string>
#include <memory>
#include <functional>

#include <stdio.h>
namespace lkup69
{

class CurlHelper
{
public:
        CurlHelper();
        ~CurlHelper();

        int  SetUrl(const std::string &url);
        int  SetTimeout(int sec);
        int  AppendHeader(const std::string &header);
        void ResetHeader(void);

        template <typename CLASS, typename MFUNC_OR_USERDATA = int>
        void SetWriteFunction(CLASS &klass, MFUNC_OR_USERDATA mfunc_or_userdata = 0)
        {
                // normal function
                if constexpr (std::is_function_v<CLASS>) {
                        curl_easy_setopt(m_curlPtr.get(), CURLOPT_WRITEFUNCTION, &klass);
                        if (!std::is_integral_v<MFUNC_OR_USERDATA>)
                                // std::__cxx11::basic_string<char>*
                                curl_easy_setopt(m_curlPtr.get(), CURLOPT_WRITEDATA, mfunc_or_userdata);
                } else if constexpr (std::is_class_v<CLASS>) {
                        if constexpr (!std::is_integral_v<MFUNC_OR_USERDATA>) {
                                // memver function
                                // the MFUNC not a int

                                m_callback = std::bind(mfunc_or_userdata, klass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
                        } else {
                                // lambda function
                                // CLASS is the default type INT

                                m_callback = std::bind(klass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
                        }

                        curl_easy_setopt(m_curlPtr.get(), CURLOPT_WRITEFUNCTION, &CurlHelper::WriteFunctionWrap_);
                        curl_easy_setopt(m_curlPtr.get(), CURLOPT_WRITEDATA, &m_callback);
                } else {
                        assert(1);
                }
        }

        template <typename CLASS, typename MFUNC_OR_USERDATA = int>
        void SetReadFunction(CLASS &klass, MFUNC_OR_USERDATA mfunc_or_userdata = 0)
        {
                // normal function
                if constexpr (std::is_function_v<CLASS>) {
                        curl_easy_setopt(m_curlPtr.get(), CURLOPT_WRITEFUNCTION, &klass);
                        if (!std::is_integral_v<MFUNC_OR_USERDATA>)
                                curl_easy_setopt(m_curlPtr.get(), CURLOPT_READDATA, mfunc_or_userdata);
                } else if constexpr (std::is_class_v<CLASS>) {
                        if constexpr (!std::is_integral_v<MFUNC_OR_USERDATA>) {
                                // memver function
                                // the MFUNC not a int

                                m_callback = std::bind(mfunc_or_userdata, klass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
                        } else {
                                // lambda function
                                // CLASS is the default type INT

                                m_callback = std::bind(klass, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
                        }

                        curl_easy_setopt(m_curlPtr.get(), CURLOPT_READFUNCTION, &CurlHelper::ReadFunctionWrap_);
                        curl_easy_setopt(m_curlPtr.get(), CURLOPT_READDATA, &m_callback);
                } else {
                        assert(1);
                }
        }

        CURLcode Get(long &httpCode);
        CURLcode Post(const std::string &data, long &httpCode);

private:
        static size_t WriteFunctionWrap_(void *ptr, size_t size, size_t nmemb, void *userp)
        {
                std::function<size_t(void *, size_t, size_t)> *callback = static_cast<std::function<size_t(void *, size_t, size_t)> *>(userp);

                return (*callback)(ptr, size, nmemb);
        }

        static size_t ReadFunctionWrap_(char *dest, size_t size, size_t nmemb, void *userp)
        {
                std::function<size_t(char *dest, size_t size, size_t nmemb)> *callback = static_cast<std::function<size_t(char *dest, size_t size, size_t nmemb)> *>(userp);

                return (*callback)(dest, size, nmemb);
        }

private:
        std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> m_curlPtr;
        std::function<size_t(void *, size_t, size_t)>       m_callback;
        struct curl_slist                                  *m_headerListPtr = nullptr;
        std::string                                         m_postData;
};
}  // namespace lkup69