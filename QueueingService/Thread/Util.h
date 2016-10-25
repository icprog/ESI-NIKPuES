#pragma once
#include "stdafx.h"

enum codes { SUCCESS = 0, SOCK_ERR = -1, SLEEP = -2, CONN_ERR = -3, SEND_ERR = -4, REC_ERR = -5 };

int listenSocketFunc(SOCKET* listenSocket, char* port);

int select(SOCKET* socket);

int accept(SOCKET* acceptedSocket, SOCKET* listenSocket);

void createMessage(char* data, int length, char* name, int nameLength, char* message, char c);

int createSocket(SOCKET* ssocket, char* ip, int port);

int sendMessage(SOCKET* socket, char* data);

int receiveServerAsClient(SOCKET* serviceSocket,SOCKET *acceptedSocket, char* message);

int receiveServerAsServer(SOCKET* serviceSocket, SOCKET *acceptedSocket, char* message);
