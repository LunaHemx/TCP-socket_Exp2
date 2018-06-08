#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<fcntl.h>

#define buffersize 1024
#define queue 20
const int my_port=8080;
const char* ip="127.0.0.1";


/*C语言进行socket编程时，常使用sockaddr和sockaddr_in两种数据结构是用来保存socket信息
struct sockaddr
{
    unsigned short sa_family; //地址族
    char sa_data[14];  //存放地址和端口
};

struct sockaddr_in
{
    short int sin_family;  //地址族
    unsigned short int sin_port; //端口号（使用网络字节序）
    struct in_addr sin_addr;  //地址
    unsigned char sin_zero[8];  //8字节数组全0，为保留空字节
};
*/

int main()
{
    int server_sockfd=socket(AF_INET,SOCK_STREAM,0);//使用socket()创建套接字server_sockfd,第一个参数是地址类型，第二个参数是套接字类型,成功则返回一个网络文件描述符
    if(server_sockfd<0)
    {
        perror("socket");  //创建socket失败，perror输出制定函数发生错误的原因
        exit(1);
    }

    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family=AF_INET;
    server_sockaddr.sin_port=htons(my_port);  //将定义的整型端口号转换为网络字节顺序
    server_sockaddr.sin_addr.s_addr=inet_addr(ip); 
    //inet_addr()将用点分十进制字符串表示的IPv4转化为用网络字节序整数表示的IPv4地址

    //服务端socket通过bind()给自己绑定IP地址与端口号，随后可在这个端口监听服务请求
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))<0)
    {
        perror("bind"); 
        exit(1);
    }

    //使得server_sockfd处于监听状态，queue为该socket建立的输入数据队列，将到达的服务器请求保存在队列里
    if(listen(server_sockfd,queue)<0)  
    {
        perror("listen");  //监听失败
        exit(1);
    }
    
    char buffer[buffersize];
    struct sockaddr_in client_addr;
    socklen_t length;

    printf("Waiting for the client to connect...\n");
    int conn=accept(server_sockfd,(struct sockaddr*)&client_addr,&length);
    /*accept()等待并请求客户连接，默认会阻塞进程，直到有一个客户端建立连接后返回，server通过accept返回成功的套接字来完成与客户的通信，
    系统调用从listen监听队列里接受accept一个连接，其中server_sockfd是执行过listen系统调用的监听socket，
    client_addr参数用来获取被接受连接的socket地址*/
    
    if(conn<0)
    {
        perror("accept");  //接受连接失败
        exit(1);
    }
    else
    {
        printf("Client has been successfully connected!\n");  //接受连接成功
        printf("Connected with ip: %s and port: %d\n",inet_ntop(AF_INET,&client_addr.sin_addr,buffer,1024),ntohs(client_addr.sin_port));
    }


    while(1)
    {
        memset(buffer,'\0',sizeof(buffer));  //初始化buffer数组
        ssize_t len=read(conn,buffer,sizeof(buffer)-1);  //读取连接成功的客户端所发送来的数据并存到buffer里
        if(len>0)
        {
            printf("Client#: %s\n",buffer); //显示client端发来的数据
        }
        else if(len==0)
        {
            printf("Read is done...\n");  //client端关闭写入
            break;
        }
        else
        {
            perror("read");
            break;
        }
        
        printf("Server please enter: ");
        fflush(stdout);
        len=read(0,buffer,sizeof(buffer)-1);//读取输入的字符串并存到buffer里
        if(len>0)
        {
            buffer[len-1]='\0';
        }
        else if(len==0)
        {
            printf("Read is done...\n");
            break;
        }
        else
        {
            perror("read");
            break;
        }
        write(conn,buffer,strlen(buffer));//把buffer里存的字符串通过write()写入conn
    }
    close(server_sockfd);//关闭server_sockfd套接字
    return 0;
}
