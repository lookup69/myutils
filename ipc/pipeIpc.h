#pragma once

namespace lkup69 {
#include <unistd.h>

class PipeIpc
{
        PipeIpc(const PipeIpc &) = delete;
        PipeIpc &operator=(const PipeIpc &) = delete;
public:
        PipeIpc() = default;
        ~PipeIpc();

        int Init(void);
        void BeWriter(void);
        void BeReader(void);
        int Read(char *buf, size_t size);
        int Write(char *buf, size_t size);
private:
        int m_fd[2] = {-1, -1};
};
}