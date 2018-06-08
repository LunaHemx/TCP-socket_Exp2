#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define buffersize 1024


/*运行时需要注意：./file_client <ip地址> <端口号> <要上传的文件名>,client只负责上传文件，
client端口号要与server端口号一致,运行成功的话只显示Send finished,同一文件夹下的file1.txt,file2.txt是测试文件*/
int main(int argc,char **argv)
{
    FILE *fp;

    if(argc!=4)
    {
        printf("Usage: %s ipaddr portnum filename\n",argv[0]);
        exit(0);
    }

    int sock_id;
    sock_id=socket(AF_INET,SOCK_STREAM,0);//创建socket幷返回相应文件标识符sock_id
    if(sock_id<0)
    {
        perror("socket");
        exit(0);
    }

    struct sockaddr_in serv_addr; //给serv_addr赋值
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(atoi(argv[2]));
    inet_pton(AF_INET,argv[1],&serv_addr.sin_addr);//inet_pton()把运行file_client时添加的ip地址从字符串转换到网络地址

    int con;
    con=connect(sock_id,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr));//将sock_id与服务器的ip地址与端口号建立连接
    if(con==-1)
    {
        perror("connect");
        return -1;
    }

    if((fp=fopen(argv[3],"r"))==NULL)//以只读方式打开指定上传的文件
    {
        perror("Open file failed");
        exit(0);
    }

    char buffer[buffersize];//缓冲区数组
    int read_len;
    int send_len;

    bzero(buffer,buffersize);//初始化缓冲区
    while((read_len=fread(buffer,sizeof(char),buffersize,fp))>0)//读取指定文件并将其内容存入buffer
    {
        send_len=send(sock_id,buffer,read_len,0);//将buffer里的内容拷贝到sock_id内核里的发送缓冲区里
        if(send_len<0)
        {
            perror("Send file failed");
            exit(0);
        }
        bzero(buffer,buffersize);//清空buffer
    }

    fclose(fp);//关闭文件指针
    close(sock_id);//关闭sock_id套接字
    printf("Send finished\n");
    return 0;
}
