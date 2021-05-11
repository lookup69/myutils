#pragma once
#include "socket.h"

namespace lkup69
{
template <typename SOCKET_TYPE>
class UnixSocketClient : public SOCKET_TYPE
{
        UnixSocketClient(const UnixSocketClient &) = delete;
        UnixSocketClient &operator=(const UnixSocketClient &) = delete;
public:
        UnixSocketClient() = default;
        virtual ~UnixSocketClient() = default;

public:
        int Connect(const std::string &addr, int port = 0)
        {
                int    sd;
                size_t len;

                assert(addr.size() > 0);

                if ((sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
                        return -1;

                memset(&m_srvAddr, 0, sizeof(m_srvAddr));
                m_srvAddr.sun_family = AF_UNIX;
                len = (addr.size() > (sizeof(m_srvAddr.sun_path) - 2)) ? (sizeof(m_srvAddr.sun_path) - 2) : addr.size();
                memcpy(m_srvAddr.sun_path, addr.c_str(),  len);

                if (connect(sd, (struct sockaddr *) &m_srvAddr, sizeof(struct sockaddr_un)) == -1) {
                        return -1;
                }

                SOCKET_TYPE::m_socket = sd;

                return 0;

        }

private:
        struct sockaddr_un  m_srvAddr;
};
}