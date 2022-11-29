事件驱动
_______________
首先我们将整个服务器抽象成一个Server类，
里面的核心是一个EventLoop，这是一个事件循环，
我们添加需要监听的事务到这个事件循环内，每次有事件发生时就会通知（在程序中返回给我们Channel），
然后根据不同的描述符、事件类型进行处理（以回调函数的方式）

这个版本服务器内只有一个EventLoop，当其中有可读事件发生时，我们可以拿到该描述符对应的Channel。<br>
```c++
void EventLoop::loop(){
          while(!quit){
          std::vector<Channel*> chs;
              chs = ep->poll();
              for(auto it = chs.begin(); it != chs.end(); ++it){
                  (*it)->handleEvent();
              }
          }
}

void Channel::handleEvent(){
    callback();
}
```
在新建Channel时，根据Channel描述符的不同分别绑定了两个回调函数，newConnection()函数被绑定到服务器socket上，handlrReadEvent()被绑定到新接受的客户端socket上。
这样如果服务器socket有可读事件，Channel里的handleEvent()函数实际上会调用Server类的newConnection()新建连接。
如果客户端socket有可读事件，Channel里的handleEvent()函数实际上会调用Server类的handlrReadEvent()响应客户端请求。

至此，我们已经抽象出了EventLoop和Channel，构成了事件驱动模型。这两个类和服务器核心Server已经没有任何关系，经过完善后可以被任何程序复用，达到了事件驱动的设计思想，现在我们的服务器也可以看成一个最简易的Reactor模式服务器。

当然，这个Reactor模式并不是一个完整的Reactor模式，如处理事件请求仍然在事件驱动的线程里，这显然违背了Reactor的概念。我们还需要做很多工作，在接下来几天的教程里会进一步完善。
