#pragma once
#include <sys/epoll.h>

class Epoll;
class Channel
{
private:
    Epoll *ep;//每个文件描述符会被分发到一个Epoll类，用一个ep指针来指向
    int fd;
    uint32_t events;//events表示希望监听这个文件描述符的哪些事件
    uint32_t revents;//revents表示在epoll返回该Channel时文件描述符正在发生的事件
    bool inEpoll;//inEpoll表示当前Channel是否已经在epoll红黑树中，为了注册Channel的时候方便区分使用EPOLL_CTL_ADD还是EPOLL_CTL_MOD
public:
    Channel(Epoll *_ep, int _fd);
    ~Channel();

    void enableReading();

    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    bool getInEpoll();
    void setInEpoll();

    // void setEvents(uint32_t);
    void setRevents(uint32_t);
};

