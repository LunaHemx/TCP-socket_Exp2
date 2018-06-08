#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/shm.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<fcntl.h>

#define buffersize 1024
const int port=8080;
const char* ip="127.0.0.1";

int main()
{
    int sock_cli=socket(AF_INET,SOCK_STREAM,0);
    if(sock_cli<0)
    {
        perror("socket");  //创建socket失败
        exit(1);
    }

    struct sockaddr_in servaddr;
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(port);  //server端所监听的端口
    servaddr.sin_addr.s_addr=inet_addr(ip);  //server端的ip地址

    /*client端需要调用connect()来连接server端，bind()与connect()不同的地方在于bind()的参数是自己的地址，connect()的参数是对方的地址*/
    /*客户端不需要listen()函数*/
    printf("Connect %s:%d\n",ip,port);
    if(connect(sock_cli,(struct sockaddr *)&servaddr,sizeof(servaddr))<0)  
    {
        perror("connect");
        exit(1);
    }
    printf("Server has been successfully connected!\n");

    char buffer[buffersize];//缓存区数组

    while(1)
    {
        memset(buffer,'\0',sizeof(buffer));//初始化缓存区数组
        printf("Client please enter: ");
        fflush(stdout);
        ssize_t len=read(0,buffer,sizeof(buffer)-1);//read()把输入的字符串读取出来并存到buffer里
        if(len>0)
        {
            buffer[len-1]='\0';
        }
        else if(len==0)
        {
            printf("Read is done...");
            break;
        }
        else
        {
            perror("read");
            break;
        }
        write(sock_cli,buffer,strlen(buffer));//把buffer里存的字符串通过write()写入sock_cli
        len=read(sock_cli,buffer,sizeof(buffer));  //读取sock_cli发来的字符串并存到buffer里
        if(len>0)
        {
            buffer[len]='\0';
        }
        else if(len==0)
        {
            printf("Read is done...");
            break;
        }
        else
        {
            perror("read");
            break;
        }
        printf("Server#: %s\n",buffer);//打印出server端发送来的字符串
    }
    close(sock_cli);//关闭sock_cli套接字
    return 0;
}
