/*
 *
 * 2021-05-11
 *
 */

#pragma once

#include <string>

#include "socket.h"

namespace lkup69 {
class SyncSocket : public Socket
{
        SyncSocket(const Socket &) = delete;
        SyncSocket &operator=(const SyncSocket &) = delete;
public:
        SyncSocket(void) = default;
        SyncSocket(int s);
        virtual ~SyncSocket() override;

public:
        virtual int GetSocket(void) override;
        virtual void Close() override;
        virtual int Read(char *buf, size_t len) override;
        virtual int Write(const char *buf, size_t len) override;

protected:
        int m_socket = -1;
};

} // namespace