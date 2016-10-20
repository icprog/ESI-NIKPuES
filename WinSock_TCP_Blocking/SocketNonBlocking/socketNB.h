#pragma once
#include "stdafx.h"
int sendNB(SOCKET socket, char* buffer, int bufferLength);
int SEND(SOCKET socket, char* buffer);
int recieveNB(SOCKET socket, char* buffer, int bufferLength);
int RECEIVE(SOCKET socket, char* buffer);