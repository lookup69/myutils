#include "socketIpc.h"

using namespace lkup69;

SocketIpc::~SocketIpc()
{
        if(m_sv[0] != -1)
                close(m_sv[0]);

        if(m_sv[1] != -1)
                close(m_sv[1]);
}

int SocketIpc::Init(void)
{
        return socketpair(AF_UNIX, SOCK_SEQPACKET | SOCK_CLOEXEC, 0, m_sv);
}

void SocketIpc::BeMaster(void)
{
        if(m_sv[1] != -1) {
                close(m_sv[1]);
                m_sv[1] = -1;
        }
}

void SocketIpc::BeSlave(void)
{
        if(m_sv[0] != -1) {
                close(m_sv[0]);
                m_sv[0] = -1;
        }
}

int SocketIpc::Read(char *buf, size_t size)
{
        int ret = 0;

        if(m_sv[0] != -1)
                ret = read(m_sv[0], buf, size);
        else if(m_sv[1] != -1)
                ret = read(m_sv[1], buf, size);
        else
                ret = -1;
        
        return ret;
}


int SocketIpc::Write(char *buf, size_t size)
{
        int ret = 0;

        if(m_sv[0] != -1)
                ret = write(m_sv[0], buf, size);
        else if(m_sv[1] != -1)
                ret = write(m_sv[1], buf, size);
        else
                ret = -1;
        
        return ret;
}

#ifdef UNIT_TEST
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// g++ -std=c++11 -DUNIT_TEST socketIpc socketIpc.cpp
int main()
{
        SocketIpc ipc;
        pid_t     pid;

        if(ipc.Init() < 0) {
                printf("ipc.Init() ... fail\n");
                exit(0);
        }

        pid = fork();
        if(pid == 0) {
                ipc.BeSlave();
                for(int i = 0; i < 5; ++i) {
                        char buf[128] = {0};

                        sprintf(buf, ">>>slave Write:%d", i);
                        ipc.Write(buf, strlen(buf));
                }

                for(int i = 0; i < 5; ++i) {
                        char buf[128] = {0};

                        ipc.Read(buf, sizeof(buf));
                        printf(">>>slave Read: %s\n", buf);
                }
        }
        if(pid > 0) {
                ipc.BeMaster();

                for(int i = 0; i < 5; ++i) {
                        char buf[128] = {0};

                        sprintf(buf, ">>> master Write:%d", i);
                        ipc.Write(buf, strlen(buf));
                }
                for(int i = 0; i < 5; ++i) {
                        char buf[128] = {0};

                        ipc.Read(buf, sizeof(buf));
                        printf(">>> master Read: %s\n", buf);
                }
        }
        sleep(3);

        return 0;

}
#endif