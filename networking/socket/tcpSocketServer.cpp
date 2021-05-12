/*
 *
 * 2021-05-11
 *
 */

#include "tcpSocketServer.h"

//#include <stdio.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <assert.h>
//#include <string.h>
#include <memory>
#include "syncSocket.h"

using namespace lkup69;


#ifdef UNIT_TEST
// g++ -DUNIT_TEST -std=c++17 -o tcpSocketServer  tcpSocketServer.cpp syncSocket.cpp

int main()
{
#if 1 // case 1        
        std::unique_ptr<TcpSocketServer<SyncSocket>> srv;

        srv = std::make_unique<TcpSocketServer<SyncSocket>>(
                SocketServer::Builder<TcpSocketServer<SyncSocket>>().SetAddress("127.0.0.1")
                                                                    .SetPort(9981).Build());
        
        if(srv->Init() !=0) {
                printf("TcpSocketServer init() fail\n");
                exit(0);
        }
       
        while(1) {
                std::string str = "hello this is server";

                std::unique_ptr<Socket> s = srv->Accept();
                
                char buf[256] = {0};
                s->Read(buf, sizeof(buf));
                printf("C >>> S:%s\n", buf);

                s->Write(str.c_str(), str.size());
        }
#else
        TcpSocketServer<SyncSocket> srv = SocketServer::Builder<TcpSocketServer<SyncSocket>>().SetAddress("127.0.0.1")
                                                                                              .SetPort(9981).Build();
        
        if(srv.Init() !=0) {
                printf("TcpSocketServer init() fail\n");
                exit(0);
        }
       
        while(1) {
                std::string str = "hello this is server";

                std::unique_ptr<Socket> s = srv.Accept();
                
                char buf[256] = {0};
                s->Read(buf, sizeof(buf));
                printf("C >>> S:%s\n", buf);

                s->Write(str.c_str(), str.size());
        }

#endif
        return 0;
}
#endif
