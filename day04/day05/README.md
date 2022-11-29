1、以Channel的方式使用epoll： 新建一个Channel时，必须说明该Channel与哪个epoll和fd绑定：

    Channel *servChannel = new Channel(ep, serv_sock->getFd());
2、这时该Channel还没有被添加到epoll红黑树，因为events没有被设置，不会监听该Channel上的任何事件发生。如果我们希望监听该Channel上发生的读事件，需要调用一个enableReading函数：
    servChannel->enableReading();
    调用这个函数后，如Channel不在epoll红黑树中，则添加，否则直接更新Channel、打开允许读事件。enableReading函数如下：

    void Channel::enableReading(){
        events = EPOLLIN | EPOLLET;
        ep->updateChannel(this);
    }
    可以看到该函数做了两件事，将要监听的事件events设置为读事件并采用ET模式，然后在ep指针指向的Epoll红黑树中更新该Channel，updateChannel函数的实现如下：

    void Epoll::updateChannel(Channel *channel){
        int fd = channel->getFd();  //拿到Channel的文件描述符
        struct epoll_event ev;
        bzero(&ev, sizeof(ev));
        ev.data.ptr = channel;
        ev.events = channel->getEvents();   //拿到Channel希望监听的事件
        if(!channel->getInEpoll()){
            errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");//添加Channel中的fd到epoll
            channel->setInEpoll();
        } else{
            errif(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify error");//已存在，则修改
        }
    }
3、在使用时，我们可以通过Epoll类中的poll()函数获取当前有事件发生的Channel：

    while(true){
        vector<Channel*> activeChannels = ep->poll();
        // activeChannels是所有有事件发生的Channel
    }
