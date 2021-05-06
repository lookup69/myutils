#pragma once

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>

#include "ipc.h"

namespace lkup69 {
class SocketIpc : public Ipc
{
        SocketIpc(const SocketIpc &) = delete;
        SocketIpc &operator=(const SocketIpc &) = delete;
public:
        SocketIpc() = default;
        ~SocketIpc() override;

        int  Init(void);
        void BeMaster(void);
        void BeSlave(void);

        int Read(char *buf, size_t size) override;
        int Write(char *buf, size_t size) override;

private:
        int m_sv[2] = {-1, -1};
};
}