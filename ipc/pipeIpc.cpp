#include "pipeIpc.h"

using namespace lkup69;

PipeIpc::~PipeIpc()
{
        if (m_fd[0] != -1)
                close(m_fd[0]);
        if (m_fd[1] != -1)
                close(m_fd[1]);
}

int PipeIpc::Init(void)
{
        return pipe(m_fd);
}

void PipeIpc::BeReader(void)
{
        if (m_fd[1] != -1) {
                close(m_fd[1]);
                m_fd[1] = -1;
        }
}

void PipeIpc::BeWriter(void)
{
        if (m_fd[0] != -1) {
                close(m_fd[0]);
                m_fd[0] = -1;
        }
}

int PipeIpc::Read(char *buf, size_t size)
{
        return read(m_fd[0], buf, size);
}

int PipeIpc::Write(char *buf, size_t size)
{
        return write(m_fd[1], buf, size);
}

int PipeIpc::DupToStdout()
{
        //return dup2(STDOUT_FILENO, m_fd[1]);
        return dup2(m_fd[1], STDOUT_FILENO);
}

#ifdef UNIT_TEST
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// g++ -std=c++11 -DUNIT_TEST pipeIpc pipeIpc.cpp
int main()
{
        PipeIpc ipc;
        pid_t   pid;

        if (ipc.Init() < 0) {
                printf("ipc.Init() ... fail\n");
                exit(0);
        }

        pid = fork();
        if (pid == 0) {
                ipc.BeReader();

                for (int i = 0; i < 5; ++i) {
                        char buf[128] = { 0 };

                        ipc.Read(buf, sizeof(buf));
                        printf(">>>Reader: %s\n", buf);
                }
        }
        if (pid > 0) {
                ipc.BeWriter();

                for (int i = 0; i < 5; ++i) {
                        char buf[128] = { 0 };

                        sprintf(buf, " Writer:%d", i);
                        ipc.Write(buf, strlen(buf));
                        sleep(1);
                }
        }
        sleep(3);

        return 0;
}
#endif
