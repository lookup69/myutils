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
#include "socketServer.h"

namespace lkup69
{

template <typename S>
class TcpSocketServer : public SocketServer
{
        typedef S SOCKET_TYPE;
TcpSocketServer &operator=(const TcpSocketServer &) = delete;
TcpSocketServer &operator=(TcpSocketServer &&) = delete;

        
public:
        TcpSocketServer(const TcpSocketServer &) = default;
        TcpSocketServer(const std::string &addr, int port, int maxConnection) :
                m_addr(addr),
                m_port(port),
                m_maxConnection(maxConnection)
        {
        }

        ~TcpSocketServer() override
        {
                if (m_socket != -1)
                        close(m_socket);
        }

public:
        int Init(void) override
        {
                int    flags;
                size_t len;

                memset(&m_srvAddr, 0, sizeof(m_srvAddr));
                m_srvAddr.sin_family = AF_INET;

                if(m_addr.size()) 
                        m_srvAddr.sin_addr.s_addr = inet_addr(m_addr.c_str());
                else
                        m_srvAddr.sin_addr.s_addr = INADDR_ANY;

                m_srvAddr.sin_port = htons(m_port);

                m_socket = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
                if (m_socket < 0) 
                        return -1;

                flags = 1;
                setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof(flags));

                if (bind(m_socket, (struct sockaddr *) &m_srvAddr, sizeof(m_srvAddr)) == -1) {
                        close(m_socket);
                        m_socket = -1;
                        return -1;
                }

                return listen(m_socket, m_maxConnection);
        }

        std::unique_ptr<Socket> Accept(void) override
        {
                int sd;

                if ((sd = accept(m_socket, NULL, 0)) == -1)
                        return nullptr;

                return std::move(std::unique_ptr<Socket>(new SOCKET_TYPE(sd)));
        }

private:
        struct sockaddr_in  m_srvAddr;
        std::string         m_addr;
        int                 m_port;
        int                 m_socket = -1;
        int                 m_maxConnection = -1;
};
}

