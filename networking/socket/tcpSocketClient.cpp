/*
 *
 * 2021-05-11
 *
 */

#include "tcpSocketClient.h"
#include "syncSocket.h"
using namespace lkup69;


#ifdef UNIT_TEST
// g++ -DUNIT_TEST -std=c++17 -o tcpSocketClient tcpSocketClient.cpp syncSocket.cpp
int main()
{
        std::string      msg = "hello this is client";
        TcpSocketClient<SyncSocket> usc;
#if 1        
        char             buf[256] = {0};

        if(usc.Connect("127.0.0.1", 9981) == -1) {
                printf("Connect fail\n");
                return 0;
        }

        usc.Write(msg.c_str(), msg.size());
        usc.Read(buf, sizeof(buf));
        printf("S >>> C:%s\n", buf);
#endif
        return 0;
}
#endif
