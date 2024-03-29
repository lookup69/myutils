#pragma once

#include <stdint.h>
#include <sys/inotify.h>

#include <string>
#include <map>
#include <thread>
#include <memory>
#include <vector>

namespace lkup69
{

class Inotify;
class InotifyObserver
{
public:
        InotifyObserver()          = default;
        virtual ~InotifyObserver() = default;

        InotifyObserver(const InotifyObserver &)            = delete;
        InotifyObserver &operator=(const InotifyObserver &) = delete;

        virtual int OnInotifyEvent(Inotify *inotify, struct inotify_event *event, const std::string &path) = 0;
};

class Inotify
{
public:
        Inotify() = default;
        virtual ~Inotify();

        Inotify(const Inotify &)            = delete;
        Inotify &operator=(const Inotify &) = delete;

public:
        static void DumpInotifyEvent(struct inotify_event *i);

public:
        int  Init();
        int  AddWatch(const std::string &path, uint32_t interesting = IN_ALL_EVENTS);
        int  RemoveWatch(const std::string &path);
        void RegisterObserver(InotifyObserver *obs);
        void Start(void);

private:
        void Watcher_(void);

private:
        int                            m_inotifyFd = -1;
        int                            m_epollFd   = -1;
        volatile bool                  m_bExit     = false;
        volatile bool                  m_bRuning   = false;
        std::vector<InotifyObserver *> m_observerVec;
        std::map<int, std::string>     m_watchMap;
        std::unique_ptr<std::thread>   m_thread;
};
}  // namespace lkup69
