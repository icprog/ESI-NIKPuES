#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#include "../SocketNB/socketNB.h"
#include "../Thread/Util.h";
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 27016

// Initializes WinSock2 library
// Returns true if succeeded, false otherwise.
bool InitializeWindowsSockets();

typedef struct sendThreadParam {
	SOCKET *acceptedSocket;
	SRWLOCK *srwLock;
} SendThreadParam;
/* Funkcija programske niti zaduzene za cekanje na klijenta.*/
DWORD WINAPI sendThreadFunc(LPVOID param) {
	printf("\n Usao sam u thread za obradu!\n");
	int iResult = 0;
	int type = *(int *)param;

	SOCKET connectSocket = INVALID_SOCKET;
	// variable used to store function return value
	// message to send
	char *messageToSend = "this is a test";
	/////////////////////////////////////////////////////////////////////////////////
	char *data = (char *)malloc(sizeof(char) * 26 + 1);
	memset(data, 0, 26);
	createMessage(data, 26, "RED1", 5, "Saljem na red1", 's');


	char *data1 = (char *)malloc(sizeof(char) * 26 + 1);
	memset(data1, 0, 26);
	createMessage(data1, 26, "RED11", 5, "Hocu sve sa reda1", 'c');


	///////////////////////////////////////////////////////////////////////////////////


	if (InitializeWindowsSockets() == false)
	{
		// we won't log anything since it will be logged
		// by InitializeWindowsSockets() function
		return 1;
	}

	// create a socket
	connectSocket = socket(AF_INET,
		SOCK_STREAM,
		IPPROTO_TCP);

	if (connectSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// create and initialize address structure
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAddress.sin_port = htons(DEFAULT_PORT);
	// connect to server specified in serverAddress and socket connectSocket
	if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		printf("Unable to connect to server.\n");
		closesocket(connectSocket);
		WSACleanup();
	}

	// Set socket to nonblocking mode
	unsigned long int nonBlockingMode = 1;
	iResult = ioctlsocket(connectSocket, FIONBIO, &nonBlockingMode);

	if(type == 0)
	// Send an prepared message with null terminator included
		iResult = SEND(&connectSocket, data);
	else
		iResult = SEND(&connectSocket, data1);

	if (iResult == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %ld\n", iResult);

	select(&connectSocket, 0);
	



	// cleanup
	closesocket(connectSocket);
	WSACleanup();

	//free(data);   ////////////////////////////////////////////////// FREE

	return 0;
}

int __cdecl main(int argc, char **argv)
{
	HANDLE sendThread[2];
	DWORD sendThreadID;
	int type = 0;
	sendThread[0] = CreateThread(0, 0, &sendThreadFunc, &type, 0, &sendThreadID);
	//type = 1;
	//sendThread[1] = CreateThread(0, 0, &sendThreadFunc, &type, 0, &sendThreadID);
	WaitForSingleObject(sendThread[0], INFINITE);
	/*
	// socket used to communicate with server
	for (int i = 0; i < 1; i++) {
		printf("\n Okidam nit za obradu: %d\n", i);
		sendThread[i] = CreateThread(0, 0, &sendThreadFunc, 0, 0, &sendThreadID);
	}
	for (int i = 0; i < 1; i++) {
		printf("Radi mi nit za obradu: %d", i);
		WaitForSingleObject(sendThread[i], INFINITE);
		CloseHandle(sendThread[i]);;
	}
	*/
	Sleep(1000);

	return 0;
}

bool InitializeWindowsSockets()
{
	WSADATA wsaData;
	// Initialize windows sockets library for this process
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return false;
	}
	return true;
}