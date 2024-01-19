#pragma once

namespace lkup69 {
#include <unistd.h>

class PipeIpc
{
public:
        PipeIpc() = default;
        ~PipeIpc();
        PipeIpc(const PipeIpc &) = delete;
        PipeIpc &operator=(const PipeIpc &) = delete;

public:
        int Init(void);
        void BeWriter(void);
        void BeReader(void);
        int Read(char *buf, size_t size);
        int Write(char *buf, size_t size);
        int DupToStdout();

private:
        int m_fd[2] = {-1, -1};
};
}
