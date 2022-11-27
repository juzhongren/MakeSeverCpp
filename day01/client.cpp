/**
 * 这个是客户端的第一个程序
*/
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 8888

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);// 创建套接字：int socket(int family,int type,int protocol);

    struct sockaddr_in serv_addr;//定义结构体sockaddr_in,最后转换为内存一致的sockaddr
    memset(&serv_addr,0,sizeof(serv_addr));//数据初始化为0

    serv_addr.sin_family = AF_INET;//ipv4协议
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//IP地址
    serv_addr.sin_port = htons(PORT);//端口号

    //bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)); 客户端不进行bind操作

    connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));//客户端向服务端发起连接请求,(sockaddr*)&serv_addr强制转换为sockaddr
    
    return 0;
}
