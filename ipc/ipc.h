#pragma once

namespace lkup69 {

class Ipc
{
public:
        virtual ~Ipc() {};
        virtual int Read(char *buf, size_t size) = 0;
        virtual int Write(char *buf, size_t size) = 0;
};
}