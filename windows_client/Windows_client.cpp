// Windows_client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<stdio.h>
#include<Windows.h>
#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

#define port 4000
#define ip "192.168.40.135"//这个ip地址是虚拟机的ip地址
#define buffersize 1024


int main(int argc, char *argv[])
{
	WSADATA ws; //与Linux中socket编程略有不同的地方，WSADATA是被用来存储被WSAStartup函数调用后返回的Windows Sockets数据

	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)//使用Winsock库函数之前必须调用，WSAStartup负责初始化动态连接库Ws2_32.dll
	{
		printf("Init Windows Socket Failed: %d\n", GetLastError());
		return -1;
	}

	SOCKET sock_cli;
	sock_cli = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//创建socket
	if (sock_cli == INVALID_SOCKET)
	{
		printf("Create Socket Failed: %d\n", GetLastError());
		return -1;
	}

	struct sockaddr_in cli_addr; //给cli_addr赋值
	cli_addr.sin_family = AF_INET;
	cli_addr.sin_port = htons(port);
	cli_addr.sin_addr.s_addr = inet_addr(ip);//将字符串转换为32位网络字节序的IPv4地址
	memset(cli_addr.sin_zero, 0x00, 8);

	int con = 0;
	con = connect(sock_cli, (struct sockaddr *)&cli_addr, sizeof(cli_addr));//将sock_cli套接字与Linux上的服务器端进行连接请求
	if (con == SOCKET_ERROR)
	{
		printf("Connect Error: %d\n", GetLastError());
		return -1;
	}
	else
	{
		printf("Successfully Connected!\n");
	}

	char buffer[buffersize];//缓冲区数组
	int ret = 0;
	while (1)
	{
		memset(buffer, 0,buffersize);//初始化buffer
		scanf("%s", buffer); //显示Successfully Connected!之后直接输入字符串
		ret = send(sock_cli, buffer, (int)strlen(buffer), 0);//将buffer里的内容通过send()拷贝到sock_cli的内核发送缓冲区里
		if (ret == SOCKET_ERROR)
		{
			printf("Send Error: %d\n", GetLastError());
			break;
		}
		
		if ('e' == buffer[0]) //按'e'退出
		{
			printf("Exit\n");
			break;
		}
	}

	closesocket(sock_cli);//关闭sock_cli套接字
	WSACleanup();//解除与Socket库的绑定并且释放Socket库所占用的系统资源
	system("pause");

    return 0;
}

