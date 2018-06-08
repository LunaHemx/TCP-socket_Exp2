// Windowsclient_file.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>
#pragma warning(disable:4996)
#pragma comment(lib,"ws2_32.lib")

#define port 1122
#define ip "192.168.40.135"//���ip��ַ���������ip��ַ
#define buffersize 1024
#define max 512//�ļ����������󳤶�

int create_connect_socket(const char *host,int PORT)//���Զ��庯����client���׽��ֵĴ���������д��һ�𣬷����������
{
	WSADATA ws;//WSADATA�Ǳ������洢��WSAStartup�������ú󷵻ص�Windows Sockets����
	if (WSAStartup(MAKEWORD(2, 0), &ws) == SOCKET_ERROR)//ʹ��Winsock�⺯��֮ǰ������ã�WSAStartup�����ʼ����̬���ӿ�Ws2_32.dllS
	{
		printf("Init Windows Socket Failed: %d\n", GetLastError());
	}

	SOCKET sock_cli;
	sock_cli = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//����socket
	if (sock_cli == INVALID_SOCKET)
	{
		printf("Create Socket Failed: %d\n", GetLastError());
		WSACleanup();
	}

	struct sockaddr_in serv_addr; //��serv_addr��ֵ
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = inet_addr(ip);

	int con = 0;
	con = connect(sock_cli, (struct sockaddr *)&serv_addr, sizeof(serv_addr));//��sock_cli�׽�����Linux�ϵķ������˽�����������
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
	return sock_cli; //�����׽���
}

void receive_file(int soc) //���Զ��庯���������մ�Linux server�˷������ļ�
{
	char buffer[buffersize]; //����������
	char filename[max];//����ļ���������
	memset(buffer, 0, buffersize);//��ʼ��buffer

	int count;
	count = recv(soc, buffer, buffersize, 0);//�ѽ������ӵ��׽��ֵ��ں˽��ջ�����������buffer���ʱserver���Ǳߴ�������Ҫ�ϴ����ļ���
	if (count < 0)
	{
		perror("recv");
		exit(1);
	}
	strcpy(filename, buffer);//��buffer������ݸ��Ƶ�filename����
	printf("Preparing recv file %s\n", filename);

	FILE *fp = fopen(filename, "wb+"); //�Կɶ�д��ʽ��ָ���ļ�
	if (fp == NULL)
	{
		perror("file open");
		exit(1);
	}
	memset(buffer, 0, buffersize);//���buffer

	int length = 0;
	while (length = recv(soc, buffer, buffersize, 0))//�ѽ��������׽��ֵ��ں˽��ܻ�����������ݿ�����buffer����,��ʱserver���Ǳߴ�������Ҫ�ϴ��ļ�������
	{
		if (length < 0)
		{
			perror("recv");
			exit(1);
		}
		int write_length = fwrite(buffer, sizeof(char), length, fp);//��buffer�������д���ļ���
		if (write_length < length)
		{
			perror("write");
			exit(1);
		}
		memset(buffer, 0, buffersize);//���buffer
	}
	printf("Received file %s finished!\n", filename);
	fclose(fp);
}

int main()
{
	SOCKET sock_cli;
	char buffer[buffersize];
	int inputl;//���������ַ���������
	while (1)
	{
		printf("Client: ���س��������򰴡�e���˳�");
		inputl = 0;
		memset(buffer, 0, buffersize);
		while ((buffer[inputl++] = getchar()) != '\n') //����س���Ϊ������ʶ
		{
			;
		}
		if ('e' == buffer[0]) //����e���˳�
		{
			printf("Exit\n");
			break;
		}
		sock_cli = create_connect_socket(ip,port);//�����׽��ֲ������˽�������
		receive_file(sock_cli);//���մӷ���˷������ļ�

		closesocket(sock_cli);//�ر��׽���
		WSACleanup();//�����Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ
	}
	system("pause");
    return 0;
}

