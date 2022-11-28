#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include "util.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd){
    //根据文件描述词来操作文件的特性。
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    errif(bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket bind error");

    errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");
    
    int epfd = epoll_create1(0);//创建一个多路复用的实例,生成用于处理accept的epoll专用的文件描述符
    errif(epfd == -1, "epoll create error");

    struct epoll_event events[MAX_EVENTS], ev;//声明epoll_event结构体的变量,ev用于注册事件,数组用于回传要处理的事件
    bzero(&events, sizeof(events));

    bzero(&ev, sizeof(ev));
    ev.data.fd = sockfd;////设置与要处理的事件相关的文件描述符
    ev.events = EPOLLIN | EPOLLET;//设置要处理的事件类型,边沿触发
    setnonblocking(sockfd);// //把socket设置为非阻塞方式
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);//注册epoll事件,操作一个多路复用的文件描述符

    while(true){
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);//等待一个epoll队列中的文件描述符的I/O事件发生,该函数返回需要处理的事件数目，如返回0表示已超时
        errif(nfds == -1, "epoll wait error");
        for(int i = 0; i < nfds; ++i){               //处理所发生的所有事件
            if(events[i].data.fd == sockfd){    //如果新监测到一个SOCKET用户连接到了绑定的SOCKET端口，建立新的连接。    //新客户端连接
                struct sockaddr_in clnt_addr;
                bzero(&clnt_addr, sizeof(clnt_addr));
                socklen_t clnt_addr_len = sizeof(clnt_addr);

                int clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len);////允许连接
                errif(clnt_sockfd == -1, "socket accept error");
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

                bzero(&ev, sizeof(ev));
                ev.data.fd = clnt_sockfd;
                ev.events = EPOLLIN | EPOLLET;
                setnonblocking(clnt_sockfd);
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sockfd, &ev);
            } else if(events[i].events & EPOLLIN){      //可读事件,读取消息
                char buf[READ_BUFFER];
                while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
                    bzero(&buf, sizeof(buf));
                    ssize_t bytes_read = read(events[i].data.fd, buf, sizeof(buf));//进行数据交换
                    if(bytes_read > 0){
                        printf("message from client fd %d: %s\n", events[i].data.fd, buf);
                        write(events[i].data.fd, buf, sizeof(buf));
                    } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
                        printf("continue reading");
                        continue;
                    } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
                        printf("finish reading once, errno: %d\n", errno);
                        break;
                    } else if(bytes_read == 0){  //EOF，客户端断开连接
                        printf("EOF, client fd %d disconnected\n", events[i].data.fd);
                        close(events[i].data.fd);   //关闭socket会自动将文件描述符从epoll树上移除
                        break;
                    }
                }
            } else{         //其他事件，之后的版本实现
                printf("something else happened\n");
            }
        }
    }
    close(sockfd);
    return 0;
}
