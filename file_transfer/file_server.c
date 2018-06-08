#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define buffersize 1024

/*运行时需要注意: ./file_server <端口号> <需要保存到的文件>,server只负责将上传文件的内容写入文件,运行成功的话显示接收到的上传文件内容与Finish receive*/
int main(int argc,char **argv)
{

    FILE *fp;

    if(argc!=3)
    {
        printf("Usage: %s portnum filename\n",argv[0]);
        exit(0);
    }

    int sock_id;
    sock_id=socket(AF_INET,SOCK_STREAM,0);//创建socket幷返回相应文件标识符sock_id
    if(sock_id<0)
    {
        perror("socket");
        exit(0);
    }

    struct sockaddr_in serv_addr;//给serv_addr赋值
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);


    if(bind(sock_id,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)//将sock_id绑定serv_addr的ip地址与端口号
    {
        perror("bind");
        exit(0);
    }

    if(listen(sock_id,10)==-1)//使sock_id处于监听状态
    {
        perror("listen");
        exit(0);
    }

    struct sockaddr_in cli_addr;
    char buffer[buffersize];//缓冲区数组

    while(1)
    {
        if((fp=fopen(argv[2],"a+"))==NULL)//以附加可读写方式打开要保存的文件，若文件不存在就创建文件，若文件存在就把写入的数据放到文件末尾且不会覆盖原来的数据
        {
            perror("Open file failed\n");
            exit(0);
        }

        int cli_addr_len;
        cli_addr_len=sizeof(cli_addr);

        int link_id;
        link_id=accept(sock_id,(struct sockaddr *)&cli_addr,&cli_addr_len);//采用默认的阻塞通信方式直到接收到客户的连接请求
        if(link_id==-1)
        {
            perror("accept");
            exit(0);
        }

        int recv_len;
        int write_len;
        bzero(buffer,buffersize);//初始化缓冲区数组
        while(recv_len=recv(link_id,buffer,buffersize,0))//把link_id里的接收缓冲区数据拷贝到buffer里
        {
            if(recv_len<0)
            {
                printf("Receive data from client failed...\n");
                break;
            }
            printf("%s\n",buffer);//打印接收到的数据
            write_len=fwrite(buffer,sizeof(char),recv_len,fp);//把buffer里的数据写入到指定需要保存的文件里
            if(write_len<recv_len)
            {
                printf("Write file failed...\n");
                break;
            }
            bzero(buffer,buffersize);//清空buffer数组
        }
        printf("\nFinish receive\n");
        fclose(fp);//关闭文件指针
        close(link_id);//关闭连接
    }
    close(sock_id);//关闭sock_id套接字
    return 0;
}
