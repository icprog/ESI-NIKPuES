// BufferTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "C:/Users/RA64-2012/Desktop/ESI-NIKPuES/WinSock_TCP_Blocking/SocketNonBlocking/util.h"  //davor
//#include "C:/Users/RA4-2012/Documents/ESI-NIKPuES/WinSock_TCP_Blocking/SocketNonBlocking/util.h"

int main()
{
	Buffer myByffer;
	char *data = (char *)malloc(sizeof(char) * 25);
	memset(data, 0, 25);
	data[0] = 25;
	char *poruka = "ovo je poruka!";
	memcpy((int *)data +1, poruka, 14);

	char *data2 = (char *)malloc(sizeof(char) * 5);
	memset(data2, 0, 5);
	data2[0] = 5;
	poruka = "nije";
	memcpy((int *)data2 + 1, poruka,4);

	char *data3 = (char *)malloc(sizeof(char) * 15);
	memset(data3, 0, 15);
	data3[0] = 15;
	poruka = "mozda ovo";
	memcpy((int *)data3 + 1, poruka, 9);




	myByffer.count = 0;
	myByffer.popIdx = 0;
	myByffer.pushIdx = 0;
	myByffer.size = 35;
	myByffer.data = (char *)malloc(sizeof(char) * 35 + 1);
	memset(myByffer.data, 0, 35);

	add(&myByffer, data);
	add(&myByffer, data2);
	remove(&myByffer, data);
	add(&myByffer, data3);
	add(&myByffer, data);
	
	free(data);

	int *array;
	array = (int *)malloc(sizeof(int) * 15);
	int a = 5;
	array[0] = a;

	printf("%d", array[0]);
	 
    return 0;
}

