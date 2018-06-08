// Windows_client.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include<stdio.h>
#include<Windows.h>
#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

#define port 4000
#define ip "192.168.40.135"//���ip��ַ���������ip��ַ
#define buffersize 1024


int main(int argc, char *argv[])
{
	WSADATA ws; //��Linux��socket������в�ͬ�ĵط���WSADATA�Ǳ������洢��WSAStartup�������ú󷵻ص�Windows Sockets����

	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)//ʹ��Winsock�⺯��֮ǰ������ã�WSAStartup�����ʼ����̬���ӿ�Ws2_32.dll
	{
		printf("Init Windows Socket Failed: %d\n", GetLastError());
		return -1;
	}

	SOCKET sock_cli;
	sock_cli = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//����socket
	if (sock_cli == INVALID_SOCKET)
	{
		printf("Create Socket Failed: %d\n", GetLastError());
		return -1;
	}

	struct sockaddr_in cli_addr; //��cli_addr��ֵ
	cli_addr.sin_family = AF_INET;
	cli_addr.sin_port = htons(port);
	cli_addr.sin_addr.s_addr = inet_addr(ip);//���ַ���ת��Ϊ32λ�����ֽ����IPv4��ַ
	memset(cli_addr.sin_zero, 0x00, 8);

	int con = 0;
	con = connect(sock_cli, (struct sockaddr *)&cli_addr, sizeof(cli_addr));//��sock_cli�׽�����Linux�ϵķ������˽�����������
	if (con == SOCKET_ERROR)
	{
		printf("Connect Error: %d\n", GetLastError());
		return -1;
	}
	else
	{
		printf("Successfully Connected!\n");
	}

	char buffer[buffersize];//����������
	int ret = 0;
	while (1)
	{
		memset(buffer, 0,buffersize);//��ʼ��buffer
		scanf("%s", buffer); //��ʾSuccessfully Connected!֮��ֱ�������ַ���
		ret = send(sock_cli, buffer, (int)strlen(buffer), 0);//��buffer�������ͨ��send()������sock_cli���ں˷��ͻ�������
		if (ret == SOCKET_ERROR)
		{
			printf("Send Error: %d\n", GetLastError());
			break;
		}
		
		if ('e' == buffer[0]) //��'e'�˳�
		{
			printf("Exit\n");
			break;
		}
	}

	closesocket(sock_cli);//�ر�sock_cli�׽���
	WSACleanup();//�����Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ
	system("pause");

    return 0;
}

