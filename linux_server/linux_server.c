#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define port 4000
#define buffersize 1024

/*linux_server.c实现的是以linux为server，windows为client幷传输字符串的tcp通信功能，
client只负责输入字符串，server只负责接受从发送来的数据字符串*/

int main(int argc, char **argv)
{
    struct sockaddr_in server_addr; //给server_addr赋值
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port); //server和client的端口号要相同
    server_addr.sin_addr.s_addr=htons(INADDR_ANY);

    int sock_serv;
    sock_serv=socket(AF_INET,SOCK_STREAM,0); //创建socket幷返回sock_serv这个文件描述符
    if(sock_serv<0)
    {
        perror("socket");
        exit(1);
    }

    int opt=1;
    setsockopt(sock_serv,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    if(bind(sock_serv,(struct sockaddr *)&server_addr,sizeof(server_addr)))//将sock_serv套接字绑定ip地址与端口号
    {
        perror("bind");
        exit(1);
    }

    if(listen(sock_serv,20))  //使sock_serv处于监听状态
    {
        perror("listen");
        exit(1);
    }

    while(1)
    {
        struct sockaddr_in cli_addr;
        int sock_cli;
        socklen_t length;
        char buffer[buffersize];//缓冲区数组

        length=sizeof(cli_addr);
        sock_cli=accept(sock_serv,(struct sockaddr *)&cli_addr,&length);//等待幷接受客户的请求幷返回标识符sock_cli
        if(sock_cli<0)
        {
            perror("accept");
            break;
        }

        while(1)
        {
            bzero(buffer,buffersize);//初始化buffer
            length=recv(sock_cli,buffer,buffersize,0);//把从sock_cli的内核接受缓存区里的数据拷贝到buffer里面幷返回接受数据的字节数length
            if(length<0)
            {
                printf("Server receive data failed...\n");
                break;
            }

            if('e'==buffer[0])//如果接收到e则为客户端中断与服务器的连接
            {
                printf("Client Exit!\n");
                break;
            }
            printf("Receive data from Client: %s\n",buffer);//打印出从client接受来的字符串，即buffer中存储的字符串
        }
        close(sock_cli);//关闭sock_cli套接字
    }
    close(sock_serv);//关闭sock_serv套接字
    return 0;
}
