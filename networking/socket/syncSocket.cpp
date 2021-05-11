/*
 *
 * 2021-05-11
 *
 */

#include "syncSocket.h"

#include <assert.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <string>


using namespace lkup69;


SyncSocket::SyncSocket(int s) : m_socket(s)
{
}


SyncSocket::~SyncSocket()
{
        if (m_socket != -1)
                close(m_socket);
}

int SyncSocket::GetSocket(void)
{
        return m_socket;
}

void SyncSocket::Close()
{
        if (m_socket != -1)
                close(m_socket);

        m_socket = -1;
}

int SyncSocket::Read(char *buf, size_t len)
{
        assert(buf != nullptr);
        assert(len > 0);
        assert(m_socket != -1);

        return read(m_socket, buf, len);
}

int SyncSocket::Write(const char *buf, size_t len)
{
        assert(buf != nullptr);
        assert(len > 0);
        assert(m_socket != -1);

        return write(m_socket, buf, len);
}

