#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define port 1122
#define buffersize 4096
#define max 512

/*linux_server_file.c实现的是以linux为server，windows为client幷传输文件的tcp通信功能，
client只负责接收文件，server只负责发送需要传输的文件名*/

int main(int argc,char **argv[])
{
    char filename[max];//filename数组用于存储输入的文件名
    bzero(filename,max);//初始化filename数组
    printf("Please input the name of the file you want to send: ");
    scanf("%s",filename);
    getchar();

    struct sockaddr_in serv_addr; //给serv_addr赋值
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(port);
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);

    int sock_id;
    sock_id=socket(AF_INET,SOCK_STREAM,0);//创建socket幷返回文件描述符sock_id
    if(sock_id<0)
    {
        perror("socket");
        exit(1);
    }

    if(bind(sock_id,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)//将sock_id绑定ip地址与端口号
    {
        perror("error");
        exit(1);
    }

    if(listen(sock_id,20)<0)//使sock_id处于监听状态
    {
        perror("listen");
        exit(1);
    }

    struct sockaddr_in cli_addr;
    int con;
    while(1)
    {
        socklen_t length=sizeof(cli_addr);
        con=accept(sock_id,(struct sockaddr *)&cli_addr,&length);//等待并接受客户请求幷返回标识符con
        if(con<0)
        {
            perror("accept");
            break;
        }

        char buffer[buffersize];
        bzero(buffer,buffersize);
        strcpy(buffer,filename);//将filename里存储的文件名复制到buffer数组里

        int count;
        count=send(con,buffer,buffersize,0);//将存储在buffer数组里的文件名拷贝给与服务器连接的con的内核发送缓存区
        if(count<0)
        {
            printf("Send file failed...\n");
            break;
        }

        FILE *fp=fopen(filename,"r+");//以可读写方式打开指定文件
        if(fp==NULL)
        {
            printf("File %s not found...\n",filename);
        }
        else
        {
            bzero(buffer,buffersize); //清空buffer数组
            int file_length=0;
            while((file_length=fread(buffer,sizeof(char),buffersize,fp))>0)//读取指定文件的内容
            {
                printf("file_length=%d\n",file_length);//显示出文件内容的字节数
                printf("file content: %s\n",buffer);//显示文件内容
                if(send(con,buffer,file_length,0)<0)//把存储在buffer数组里面的内容拷贝进con内核里的发送缓冲区
                {
                    perror("send");
                    break;
                }
                bzero(buffer,buffersize);
            }
            fclose(fp);//关闭文件指针
            printf("Transfer file %s finished!\n",filename);
        }
        close(con);//关闭连接
    }
    close(sock_id);//关闭sock_id套接字
    return 0;
}
