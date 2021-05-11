#pragma once

#include <assert.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <string>
namespace lkup69 {
class Socket
{
        Socket(const Socket &) = delete;
        Socket &operator=(const Socket &) = delete;
public:
        Socket(void) = default;
        virtual ~Socket()
        {
        }
public:
        virtual void Close() = 0;
        virtual int GetSocket(void) = 0;
        virtual int Read(char *buf, size_t len) = 0;
        virtual int Write(const char *buf, size_t len) = 0;
};

} // namespace