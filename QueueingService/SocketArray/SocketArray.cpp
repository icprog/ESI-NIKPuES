#include "stdafx.h"
#include "SocketArray.h"

void expandSockets(SocketArray * socketArray)
{
	MySocket *newArray;
	newArray = (MySocket *)malloc(sizeof(MySocket) * socketArray->size * 2);
	socketArray->size *= 2;
	memcpy(newArray, socketArray->sockets, socketArray->size);
	free(socketArray->sockets);
	socketArray->sockets = newArray;
}

void addSocket(SocketArray * socketArray, MySocket *socket)
{

	EnterCriticalSection(&socketArray->cs);
	if (socketArray->count == socketArray->size)
		expandSockets(socketArray);

	for (int i = 0; i < socketArray->size; i++) {
		if (socketArray->sockets[i].socket == INVALID_SOCKET) {
			socketArray->sockets[i].socket = socket->socket;
			socketArray->sockets[i].bufferName = socket->bufferName;
			break;
		}
	}

	socketArray->count++;
	LeaveCriticalSection(&socketArray->cs);

}

void removeSocket(SocketArray * socketArray, MySocket *socket)
{

	EnterCriticalSection(&socketArray->cs);
	//smanji count i prolazi kroz sve elemente niza i obrisi bafer sa yadatim imenom
	socketArray->count--;

	for (int i = 0; i < socketArray->count; i++) {
		if (socketArray->sockets[i].socket == socket->socket) {
			closesocket(socketArray->sockets[i].socket);
			socketArray->sockets[i].socket = INVALID_SOCKET;
			socketArray->sockets[i].bufferName = NULL;
			break;
		}
	}
	LeaveCriticalSection(&socketArray->cs);

}

void clearSockets(SocketArray * socketArray)
{

	EnterCriticalSection(&socketArray->cs);

	for (int i = 0; i < socketArray->count; i++) {
		closesocket(socketArray->sockets[i].socket);
		socketArray->sockets[i].socket = INVALID_SOCKET;
		socketArray->sockets[i].bufferName = NULL;
		break;
	}
	free(socketArray->sockets);
	LeaveCriticalSection(&socketArray->cs);
	DeleteCriticalSection(&socketArray->cs);

}

void initializeSockets(SocketArray * socketArray, int size, CRITICAL_SECTION *cs)
{

	EnterCriticalSection(cs);
	socketArray->count = 0;
	socketArray->size = size;
	socketArray->sockets = (MySocket *)malloc(sizeof(MySocket) * size);
	for (int i = 0; i < size; i++) {
		socketArray->sockets[i].bufferName = NULL;
		socketArray->sockets[i].socket = INVALID_SOCKET;
	}
	CRITICAL_SECTION queueCS;
	InitializeCriticalSection(&queueCS);
	socketArray->cs = queueCS;
	LeaveCriticalSection(cs);

}
