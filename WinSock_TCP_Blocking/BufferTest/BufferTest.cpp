// BufferTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "C:/Users/RA4-2012/Documents/ESI-NIKPuES/WinSock_TCP_Blocking/SocketNonBlocking/util.h"

int main()
{
	Buffer myByffer;
	char *data = (char *)malloc(sizeof(char) * 25);
	memset(data, 0, 25);
	data[0] = 25;
	char *poruka = "ovo je poruka!";
	memcpy((int *)data +1, poruka, 14);




	myByffer.count = 0;
	myByffer.popIdx = 0;
	myByffer.pushIdx = 0;
	myByffer.size = 35;
	myByffer.data = (char *)malloc(sizeof(char) * 35);
	memset(myByffer.data, 0, 35);
	add(&myByffer, data);
	add(&myByffer, data);
	remove(&myByffer, data);
	expand(&myByffer);
	expand(&myByffer);
	shrink(&myByffer);
    return 0;
}

