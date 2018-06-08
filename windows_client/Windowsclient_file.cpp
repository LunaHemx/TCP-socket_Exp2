// Windowsclient_file.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>
#pragma warning(disable:4996)
#pragma comment(lib,"ws2_32.lib")

#define port 1122
#define ip "192.168.40.135"//这个ip地址是虚拟机的ip地址
#define buffersize 1024
#define max 512//文件名数组的最大长度

int create_connect_socket(const char *host,int PORT)//本自定义函数将client端套接字的创建、连接写在一起，方便后续调用
{
	WSADATA ws;//WSADATA是被用来存储被WSAStartup函数调用后返回的Windows Sockets数据
	if (WSAStartup(MAKEWORD(2, 0), &ws) == SOCKET_ERROR)//使用Winsock库函数之前必须调用，WSAStartup负责初始化动态连接库Ws2_32.dllS
	{
		printf("Init Windows Socket Failed: %d\n", GetLastError());
	}

	SOCKET sock_cli;
	sock_cli = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//创建socket
	if (sock_cli == INVALID_SOCKET)
	{
		printf("Create Socket Failed: %d\n", GetLastError());
		WSACleanup();
	}

	struct sockaddr_in serv_addr; //给serv_addr赋值
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = inet_addr(ip);

	int con = 0;
	con = connect(sock_cli, (struct sockaddr *)&serv_addr, sizeof(serv_addr));//将sock_cli套接字与Linux上的服务器端进行连接请求
	if (con == SOCKET_ERROR)
	{
		printf("Connect Error: %d\n", GetLastError());
		closesocket(sock_cli);
		WSACleanup();
	}
	else
	{
		printf("Successfully Connected!\n");
	}
	return sock_cli; //返回套接字
}

void receive_file(int soc) //本自定义函数用来接收从Linux server端发来的文件
{
	char buffer[buffersize]; //缓冲区数组
	char filename[max];//存放文件名的数组
	memset(buffer, 0, buffersize);//初始化buffer

	int count;
	count = recv(soc, buffer, buffersize, 0);//把建立连接的套接字的内核接收缓冲区拷贝到buffer里，此时server端那边传来的是要上传的文件名
	if (count < 0)
	{
		perror("recv");
		exit(1);
	}
	strcpy(filename, buffer);//把buffer里的数据复制到filename里面
	printf("Preparing recv file %s\n", filename);

	FILE *fp = fopen(filename, "wb+"); //以可读写方式打开指定文件
	if (fp == NULL)
	{
		perror("file open");
		exit(1);
	}
	memset(buffer, 0, buffersize);//清空buffer

	int length = 0;
	while (length = recv(soc, buffer, buffersize, 0))//把建立连接套接字的内核接受缓存区里的数据拷贝到buffer里面,此时server端那边传来的是要上传文件的内容
	{
		if (length < 0)
		{
			perror("recv");
			exit(1);
		}
		int write_length = fwrite(buffer, sizeof(char), length, fp);//把buffer里的内容写入文件里
		if (write_length < length)
		{
			perror("write");
			exit(1);
		}
		memset(buffer, 0, buffersize);//清空buffer
	}
	printf("Received file %s finished!\n", filename);
	fclose(fp);
}

int main()
{
	SOCKET sock_cli;
	char buffer[buffersize];
	int inputl;//用于输入字符自增变量
	while (1)
	{
		printf("Client: 按回车键继续或按‘e’退出");
		inputl = 0;
		memset(buffer, 0, buffersize);
		while ((buffer[inputl++] = getchar()) != '\n') //输入回车键为结束标识
		{
			;
		}
		if ('e' == buffer[0]) //按“e”退出
		{
			printf("Exit\n");
			break;
		}
		sock_cli = create_connect_socket(ip,port);//创建套接字并与服务端进行连接
		receive_file(sock_cli);//接收从服务端发来的文件

		closesocket(sock_cli);//关闭套接字
		WSACleanup();//解除与Socket库的绑定并且释放Socket库所占用的系统资源
	}
	system("pause");
    return 0;
}

