#include "stdafx.h"
#include "socketNB.h"

#define SLEEP_TIME 50

enum codes { SUCCESS=0, SOCK_ERR=-1, SLEEP=-2, CONN_ERR=-3 };


int bufferLength(char* buffer){
	return *((int*)buffer);
}

int sendNB(SOCKET socket,  char* buffer, int bufferLength){
	int iResult, i = 0;
	int sockAddrLen = sizeof(struct sockaddr);
	FD_SET set;
	timeval timeVal;
	
	FD_ZERO(&set);
	// Add socket we will wait to read from
	FD_SET(socket, &set);

	// Set timeouts to zero since we want select to return
	// instantaneously
	timeVal.tv_sec = 0;
	timeVal.tv_usec = 0;

	iResult = select(0 /* ignored */,  NULL, &set, NULL, &timeVal);

	// lets check if there was an error during select
	if (iResult == SOCKET_ERROR)
	{
		fprintf(stderr, "select failed with error: %ld\n", WSAGetLastError());
		return SOCK_ERR; //error code: -1
	}

	// now, lets check if there are any sockets ready
	if (iResult == 0)
	{
		// there are no ready sockets, sleep for a while and check again
		Sleep(SLEEP_TIME);
		return SLEEP; // sleep code: 1
	}


	iResult = send(socket, buffer, bufferLength, 0);

	if (iResult == SOCKET_ERROR)
	{
		printf("sendto failed with error: %d\n", WSAGetLastError());
		closesocket(socket);
		WSACleanup();
		return CONN_ERR; // connection error code: 2
	}

	return iResult; // success code: 0;
}

int SEND(SOCKET socket, char* buffer){
	int i = 0;
	int len = 15;
	int iResult;
	while (i < len){
		do {
			iResult = sendNB(socket, buffer, len - i);
		} while (iResult == SLEEP);
		if (iResult == SOCKET_ERROR)
		{
			printf("sendto failed with error: %d\n", WSAGetLastError());
			closesocket(socket);
			WSACleanup();
			return CONN_ERR; // connection error code: 2
		}
		i += iResult;
	}
	return SUCCESS; // success code: 0
}


int recieveNB(SOCKET socket, char* buffer, int bufferLength){
	int iResult, i = 0;
	int sockAddrLen = sizeof(struct sockaddr);
	FD_SET set;
	timeval timeVal;

	FD_ZERO(&set);
	// Add socket we will wait to read from
	FD_SET(socket, &set);

	// Set timeouts to zero since we want select to return
	// instantaneously
	timeVal.tv_sec = 0;
	timeVal.tv_usec = 0;

	iResult = select(0 /* ignored */, &set, NULL, NULL, &timeVal);

	// lets check if there was an error during select
	if (iResult == SOCKET_ERROR)
	{
		fprintf(stderr, "select failed with error: %ld\n", WSAGetLastError());
		return SOCK_ERR; //error code: -1
	}

	// now, lets check if there are any sockets ready
	if (iResult == 0)
	{
		// there are no ready sockets, sleep for a while and check again
		Sleep(SLEEP_TIME);
		return SLEEP; // sleep code: 1
	}


	do
	{
		// Receive data until the client shuts down the connection
		iResult = recv(socket, buffer, bufferLength, 0);
		if (iResult > 0)
		{
			printf("Message received from client: %s.\n", buffer);
		}
		else if (iResult == 0)
		{
			// connection was closed gracefully
			printf("Connection with client closed.\n");
			closesocket(socket);
		}
		else
		{
			// there was an error during recv
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(socket);
		}
	} while (iResult > 0);

	return iResult; // success code: 0;

}