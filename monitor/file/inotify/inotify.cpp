/*
 * Licence BSD
 * 2021
 */

#include "inotify.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <limits.h>
#include <string.h>
#include <assert.h>

using namespace lkup69;


/* Supported events suitable for MASK parameter of INOTIFY_ADD_WATCH.  */
// IN_ACCESS             0x00000001     /* File was accessed.  */
// IN_MODIFY             0x00000002     /* File was modified.  */
// IN_ATTRIB             0x00000004     /* Metadata changed.  */
// IN_CLOSE_WRITE        0x00000008     /* Writtable file was closed.  */
// IN_CLOSE_NOWRITE      0x00000010  /* Unwrittable file closed.  */
// IN_CLOSE              (IN_CLOSE_WRITE | IN_CLOSE_NOWRITE) /* Close.  */
// IN_OPEN               0x00000020     /* File was opened.  */
// IN_MOVED_FROM         0x00000040     /* File was moved from X.  */
// IN_MOVED_TO           0x00000080  /* File was moved to Y.  */
// IN_MOVE               (IN_MOVED_FROM | IN_MOVED_TO) /* Moves.  */
// IN_CREATE             0x00000100     /* Subfile was created.  */
// IN_DELETE             0x00000200     /* Subfile was deleted.  */
// IN_DELETE_SELF        0x00000400     /* Self was deleted.  */
// IN_MOVE_SELF          0x00000800     /* Self was moved.  */

/* Events sent by the kernel.  */
// IN_UNMOUNT            0x00002000     /* Backing fs was unmounted.  */
// IN_Q_OVERFLOW         0x00004000     /* Event queued overflowed.  */
// IN_IGNORED            0x00008000     /* File was ignored.  */

/* Helper events.  */
// IN_CLOSE              (IN_CLOSE_WRITE | IN_CLOSE_NOWRITE)    /* Close.  */
// IN_MOVE               (IN_MOVED_FROM | IN_MOVED_TO)          /* Moves.  */

/* Special flags.  */
// IN_ONLYDIR            0x01000000     /* Only watch the path if it is a directory.  */
// IN_DONT_FOLLOW        0x02000000     /* Do not follow a sym link.  */
// IN_EXCL_UNLINK        0x04000000     /* Exclude events on unlinked objects.  */
// IN_MASK_CREATE        0x10000000     /* Only create watches.  */
// IN_MASK_ADD           0x20000000    /* Add to the mask of an already existing watch.  */
// IN_ISDIR              0x40000000 /* Event occurred against dir.  */
// IN_ONESHOT            0x80000000     /* Only send event once.  */

static void display_inotify_event(struct inotify_event *i)
{
        printf("    wd =%2d; ", i->wd);
        if (i->cookie > 0)
                printf("cookie =%4d; ", i->cookie);

        printf("mask = ");
        if (i->mask & IN_ACCESS)
                printf("IN_ACCESS ");
        if (i->mask & IN_ATTRIB)
                printf("IN_ATTRIB ");
        if (i->mask & IN_CLOSE_NOWRITE)
                printf("IN_CLOSE_NOWRITE ");
        if (i->mask & IN_CLOSE_WRITE)
                printf("IN_CLOSE_WRITE ");
        if (i->mask & IN_CREATE)
                printf("IN_CREATE ");
        if (i->mask & IN_DELETE)
                printf("IN_DELETE ");
        if (i->mask & IN_DELETE_SELF)
                printf("IN_DELETE_SELF ");
        if (i->mask & IN_IGNORED)
                printf("IN_IGNORED ");
        if (i->mask & IN_ISDIR)
                printf("IN_ISDIR ");
        if (i->mask & IN_MODIFY)
                printf("IN_MODIFY ");
        if (i->mask & IN_MOVE_SELF)
                printf("IN_MOVE_SELF ");
        if (i->mask & IN_MOVED_FROM)
                printf("IN_MOVED_FROM ");
        if (i->mask & IN_MOVED_TO)
                printf("IN_MOVED_TO ");
        if (i->mask & IN_OPEN)
                printf("IN_OPEN ");
        if (i->mask & IN_Q_OVERFLOW)
                printf("IN_Q_OVERFLOW ");
        if (i->mask & IN_UNMOUNT)
                printf("IN_UNMOUNT ");
        printf("\n");

        if (i->len > 0)
                printf("        name = %s\n", i->name);
}

Inotify::~Inotify()
{
        if(m_bRuning)
            m_bExit = true;

        while (m_bExit);

        if(m_inotifyFd != -1)
            close(m_inotifyFd);

        if(m_epollFd != -1)
            close(m_epollFd);            
}

int Inotify::Init()
{
        if ((m_inotifyFd = inotify_init()) < 0) {
                fprintf(stderr,
                        "[%s][%s][%d][err] initfy_init() \n",
                        __FILE__, __PRETTY_FUNCTION__, __LINE__);
                return -1;
        }

        if ((m_epollFd = epoll_create1(0)) < 0) {
                fprintf(stderr,
                        "[%s][%s][%d][err] epoll_create1() \n",
                        __FILE__, __PRETTY_FUNCTION__, __LINE__);
                if(m_inotifyFd != -1) {
                    close(m_inotifyFd);
                    m_inotifyFd = -1;
                }

                return -1;
        }
        return 0;
}

int Inotify::AddWatch(const std::string &path, uint32_t interesting)
{
        int wd;

        assert(m_inotifyFd != -1);

        if ((wd = inotify_add_watch(m_inotifyFd, path.c_str(), interesting)) == -1) {
                fprintf(stderr,
                        "[%s][%s][%d][err] inotify_add_watch(%d, %s) \n",
                        __FILE__, __PRETTY_FUNCTION__, __LINE__,
                        m_inotifyFd, path.c_str());

                return -1;
        }

        m_watchMap[wd] = path;

        return 0;
}

int Inotify::RemoveWatch(const std::string &path)
{
        std::map<int, std::string>::iterator it;

        assert(m_inotifyFd != -1);

        for (it = m_watchMap.begin(); it != m_watchMap.end(); ++it) {
                if (it->second.compare(path) == 0) {
                        if (inotify_rm_watch(m_inotifyFd, it->first) < 0) {
                                fprintf(stderr,
                                        "[%s][%s][%d][err] inotify_rm_watch(%d, %d) :%s \n",
                                        __FILE__, __PRETTY_FUNCTION__, __LINE__,
                                        m_inotifyFd, it->first, it->second.c_str());

                                return -1;
                        }
                        m_watchMap.erase(it);

                        return 0;
                }
        }
        fprintf(stderr,
                "[%s][%s][%d][err] not exist: %s \n",
                __FILE__, __PRETTY_FUNCTION__, __LINE__,
                path.c_str());

        return -1;
}

void Inotify::RegisterObserver(InotifyObserver *obs)
{
        m_observer = obs;
}

void Inotify::Start(void)
{
        assert(m_observer != nullptr);

        m_thread.reset(new std::thread(&Inotify::Watcher_, this));
        m_thread->detach();

}

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))
#define EPOLL_EVENT_LEN 1

void Inotify::Watcher_(void)
{
        struct inotify_event *event;
        struct epoll_event   ev; 
        struct epoll_event   events[EPOLL_EVENT_LEN];
        char                 *p;
        int                  numRead;
        int                  nfds;


        ev.events = EPOLLIN;
        ev.data.fd = m_inotifyFd;
        if (epoll_ctl(m_epollFd, EPOLL_CTL_ADD, m_inotifyFd, &ev) == -1) {
                fprintf(stderr,
                        "[%s][%s][%d][err] epoll_ctl() \n",
                        __FILE__, __PRETTY_FUNCTION__, __LINE__);
                return;
        }

        m_bRuning = true;
        while (!m_bExit) {
                char buf[BUF_LEN] = {0};

                nfds = epoll_wait(m_epollFd, events, 1, 1000);

                if((nfds < 0) || m_bExit)
                        continue;

                if(events[0].data.fd == m_inotifyFd) {
                    /* Read events forever */
                    numRead = read(m_inotifyFd, buf, BUF_LEN);

                    if (numRead == 0)
                            continue;

                    if (numRead == -1) {
                            fprintf(stderr,
                                    "[%s][%s][%d][err] read() \n",
                                    __FILE__, __PRETTY_FUNCTION__, __LINE__);
                    }

                    //printf("Read %ld bytes from inotify fd\n", (long)numRead);
                    for (p = buf; p < buf + numRead;) {
                            event = (struct inotify_event *)p;
                            if (m_observer)
                                    m_observer->OnNotify(this, event, m_watchMap[event->wd]);

                            p += sizeof(struct inotify_event) + event->len;
                    }
                }
        }

        m_bRuning = false;
        m_bExit = false;
}

// g++ -Wall -DUNIT_TEST -std=c++17 -o inotify inotify.cpp -pthread
#ifdef UNIT_TEST

class UnitTest : public InotifyObserver
{
public:
        int OnNotify(Inotify *inotify, struct inotify_event *event, const std::string &path) override
        {
                display_inotify_event(event);
                printf(">>>>>> %s\n", path.c_str());
                return 0;
        }
};

int main(int argc, char *argv[])
{
        UnitTest un;

        {
            std::unique_ptr<Inotify> in = std::make_unique<Inotify>();

            if (in->Init() == -1)
                    exit(0);


            for (int i = 1; i < argc; i++)
                    in->AddWatch(argv[i]);

            in->RegisterObserver(&un);
            in->Start();

            int cnt = 5;
            while (--cnt)
                    sleep(1);
        }
        return 0;
}
#endif
