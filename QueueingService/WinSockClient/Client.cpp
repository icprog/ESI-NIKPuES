#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#include "../SocketNB/socketNB.h"
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


	SOCKET connectSocket = INVALID_SOCKET;
	// variable used to store function return value
	// message to send
	char *messageToSend = "this is a test";
	/////////////////////////////////////////////////////////////////////////////////
	char *data = (char *)malloc(sizeof(char) * 26);
	memset(data, 0, 26);
	data[0] = 26;

	*(char*)((int *)data + 1) = 4;
	char *ime = "RED1";
	char *message = "ovo je poruka";

	memcpy(data + 8, ime, 4);
	memcpy(data + 12, message, 14);



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

	// Send an prepared message with null terminator included
	iResult = SEND(&connectSocket, data);

	if (iResult == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %ld\n", iResult);

	// cleanup
	closesocket(connectSocket);
	WSACleanup();

	free(data);   ////////////////////////////////////////////////// FREE

	return 0;
}

int __cdecl main(int argc, char **argv)
{
	HANDLE sendThread[1];
	DWORD sendThreadID;

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