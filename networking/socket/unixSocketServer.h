#pragma once

#include "socket.h"
#include "socketServer.h"
//#include "syncSocket.h"
namespace lkup69
{

template <typename S>
class UnixSocketServer : public SocketServer
{
        typedef S SOCKET_TYPE;

        UnixSocketServer &operator=(const UnixSocketServer &) = delete;
public:
        UnixSocketServer(const UnixSocketServer &) = default;
        UnixSocketServer(const std::string &addr, int port, int maxConnection) :
                m_addr(addr),
                m_maxConnection(maxConnection)
        {
        }

        ~UnixSocketServer() override
        {
                if (m_socket != -1)
                        close(m_socket);
                remove(m_srvAddr.sun_path);
        }

public:
        int Init(void) override
        {
                size_t len;

                assert(m_addr.size() > 0);

                memset(&m_srvAddr, 0, sizeof(m_srvAddr));
                m_srvAddr.sun_family = AF_UNIX;

                //Abstract Socket Namespace
                len = (m_addr.size() > (sizeof(m_srvAddr.sun_path) - 2)) ? (sizeof(m_srvAddr.sun_path) - 2) : m_addr.size();
                memcpy(m_srvAddr.sun_path, m_addr.c_str(),  len);

                m_socket = socket(AF_UNIX, SOCK_STREAM, 0);
                if (m_socket < 0) {
                        return -1;
                }

                unlink(m_srvAddr.sun_path);
                if (bind(m_socket, (struct sockaddr *) &m_srvAddr, sizeof(struct sockaddr_un)) == -1) {
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

                //return std::move(std::unique_ptr<Socket>(new SOCKET_TYPE(sd)));
                return std::move(std::unique_ptr<Socket>(new SOCKET_TYPE(sd)));
        }

private:
        struct sockaddr_un  m_srvAddr;
        std::string         m_addr;
        int                 m_socket = -1;
        int                 m_maxConnection = -1;
};
}

