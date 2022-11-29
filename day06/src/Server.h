#pragma once

class EventLoop;//声明
class Socket;//声明
class Server
{
private:
    EventLoop *loop;
public:
    Server(EventLoop*);
    ~Server();

    void handleReadEvent(int);
    void newConnection(Socket *serv_sock);
};

