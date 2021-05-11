/*
 *
 * 2021-05-11
 *
 */
#pragma once

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#include "socket.h"

namespace lkup69
{
template <typename SOCKET_TYPE>
class TcpSocketClient : public SOCKET_TYPE
{
        TcpSocketClient(const TcpSocketClient &) = delete;
        TcpSocketClient &operator=(const TcpSocketClient &) = delete;
public:
        TcpSocketClient() = default;
        virtual ~TcpSocketClient() = default;

public:
        int Connect(const std::string &addr, int port = 0)
        {
                int    sd;

                assert(addr.size() > 0);

                if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
                        return -1;

                memset(&m_srvAddr, 0, sizeof(m_srvAddr));
                m_srvAddr.sin_family = AF_INET;
                m_srvAddr.sin_addr.s_addr = inet_addr(addr.c_str());
                m_srvAddr.sin_port = htons(port);

                if (connect(sd, (struct sockaddr *) &m_srvAddr, sizeof(m_srvAddr)) == -1) {
                        return -1;
                }

                SOCKET_TYPE::m_socket = sd;

                return 0;

        }

private:
        struct sockaddr_in  m_srvAddr;
};
}