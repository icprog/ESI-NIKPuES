#pragma once
#include "stdafx.h"


typedef struct mySocket {
	SOCKET socket;
	char *bufferName;
} MySocket;

typedef struct socketArray {
	MySocket *sockets;
	int count;
	int size;
	CRITICAL_SECTION cs;
}SocketArray;

void addSocket(SocketArray *socketArray, MySocket *socket);          //queue je niz, a buffer je pokazivac na buffer
void removeSocket(SocketArray *socketArray, MySocket *socket);
void clearSockets(SocketArray *socketArray);
void expandSockets(SocketArray *socketArray);
void initializeSockets(SocketArray *socketArray, int size, CRITICAL_SECTION *cs);