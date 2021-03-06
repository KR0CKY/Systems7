
#include "pch.h"
#include < iostream >
#pragma comment( lib, "ws2_32.lib" )
#include < Windows.h>
#include < conio.h>


#define PORT 7
#define SERVERADDR "127.0.0.1"

char nick[100];
char str_array[50][100];
int length_mes = 0;

void recieveMessage(SOCKET ConnectSocket) {
	char buf[1024];
	for (;;)
	{
		memset(buf, 0, sizeof(buf));
		if (recv(ConnectSocket, buf, 1024, NULL))
		{
			printf(buf);
		}
	}
	delete(buf);
}

void sendMessage(SOCKET ConnectSocket)
{
	char buf[1024];
	char tempBuf[924];
	for (;;)
	{
		memset(buf, 0, sizeof(buf));
		fgets(tempBuf, 900, stdin);
		strcpy_s(buf, nick);
		strcat_s(buf, ": ");
		strcat_s(buf, tempBuf);
		if (strlen(buf) - strlen(nick) != 3)
			send(ConnectSocket, buf, (int)strlen(buf), NULL);
	}
}


int main(int argc, char* argv[])
{
	char buff[1024];
	printf("CLIENT\n");

	printf("Name: ");
	std::cin >> nick;
	// Шаг 1 - инициализация библиотеки Winsock
	if (WSAStartup(0x202, (WSADATA *)&buff[0]))
	{
		printf("WSAStart error %d\n", WSAGetLastError());
		return -1;
	}

	// Шаг 2 - создание сокета
	SOCKET my_sock;
	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_sock < 0)
	{
		printf("Socket() error %d\n", WSAGetLastError());
		return -1;
	}

	// Шаг 3 - установка соединения

	// заполнение структуры sockaddr_in
	// указание адреса и порта сервера
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	HOSTENT *hst;

	// преобразование IP адреса из символьного в сетевой формат
	if (inet_addr(SERVERADDR) != INADDR_NONE)
		dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	else
		// попытка получить IP адрес по доменному имени сервера
		if (hst = gethostbyname(SERVERADDR))
			// hst->h_addr_list массив указателей на адреса
			((unsigned long *)&dest_addr.sin_addr)[0] =
			((unsigned long **)hst->h_addr_list)[0][0];
		else
		{
			printf("Invalid address %s\n", SERVERADDR);
			closesocket(my_sock);
			WSACleanup();
			return -1;
		}

	// адрес сервера получен – пытаемся установить соединение 
	if (connect(my_sock, (sockaddr *)&dest_addr,
		sizeof(dest_addr)))
	{
		printf("Connect error %d\n", WSAGetLastError());
		return -1;
	}

	DWORD thID;

	// Шаг 4 - чтение и передача сообщений
	int nsize;

	HANDLE sendm;
	HANDLE recievem;

	recievem = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)recieveMessage, (LPVOID)(my_sock), NULL, NULL);
	sendm = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)sendMessage, (LPVOID)(my_sock), NULL, NULL);

	WaitForSingleObject(recievem, INFINITE);
	WaitForSingleObject(sendm, INFINITE);

	printf("Recv error %d\n", WSAGetLastError());
	closesocket(my_sock);
	WSACleanup();
	return -1;
}
