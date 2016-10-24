#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "../Buffer/Buffer.h";
#include "../Queue/Queue.h";
#include "../SocketNB/SocketNB.h";
#include "../SocketArray/SocketArray.h";
#include "../Thread/Thread.h";
#include "../ThreadArray/ThreadArray.h";

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27016"
#define DEFAULT_SOCARRLEN 10
#define DEFAULT_THREADARRLEN 10
#define INITIAL_QUEUE_SIZE 10
void createQueue(Queue *queue);
void createSocketArray(SocketArray *socketArray);
void createThreadArray(ThreadArray *threadArray, SocketArray *socketArray, Queue *queue);
bool InitializeWindowsSockets();
int  main(void) 
{
	if (InitializeWindowsSockets() == false)
	{
		// we won't log anything since it will be logged
		// by InitializeWindowsSockets() function
		return 1;
	}
	/*
	Kreiranje redova( buffera) i niza redova(queuea )
	*/
	Queue queue;
	createQueue(&queue);
	/*
		Kreiranje niza soketa za komunikaciju sa klijentima. 
		Inicijalizacija na pocetne vrednosti.

	*/
	SocketArray sockets;
	createSocketArray(&sockets);

	/*
		Kreiranje niza svih tredova koji se koriste u programu.
		0: Nit za komunikaciju sa klijentima
		1: Nit za komunikaciju sa drugim servisom
		2: Nit koja periodicno proverava da li je neki tread zavrsio sa radom i zatvara handle na njega
	*/
	ThreadArray threadArray;
	createThreadArray(&threadArray, &sockets, &queue);


    return 0;
}


void createQueue(Queue *queue) {
	char *imered1 = "RED1";
	char *imered11 = "RED11";
	char *imered2 = "RED2";
	char *imered21 = "RED21";
	char *imered3 = "RED3";
	char *imered31 = "RED31";

	Buffer red1;
	Buffer red11;
	Buffer red2;
	Buffer red21;
	Buffer red3;
	Buffer red31;
	CRITICAL_SECTION cs[6];
	for (int i = 0; i < 6; i++) {
		InitializeCriticalSection(&cs[i]);
	}
	createBuffer(&red1, imered1, DEFAULT_BUFLEN, &cs[0]);
	createBuffer(&red11, imered11, DEFAULT_BUFLEN, &cs[1]);
	createBuffer(&red2, imered2, DEFAULT_BUFLEN, &cs[2]);
	createBuffer(&red21, imered21, DEFAULT_BUFLEN, &cs[3]);
	createBuffer(&red3, imered3, DEFAULT_BUFLEN, &cs[4]);
	createBuffer(&red31, imered31, DEFAULT_BUFLEN, &cs[5]);

	Buffer *bufferArray = (Buffer *)malloc(sizeof(Buffer)*INITIAL_QUEUE_SIZE);
	bufferArray[0] = red1;
	bufferArray[1] = red11;
	bufferArray[2] = red2;
	bufferArray[3] = red21;
	bufferArray[4] = red3;
	bufferArray[5] = red31;

	// Da li je cs potrebna?
	CRITICAL_SECTION quecs;
	InitializeCriticalSection(&quecs);
	initializeQueue(queue, INITIAL_QUEUE_SIZE, &quecs);
	queue->buffer = bufferArray;
	queue->count = 6;
}

void createSocketArray(SocketArray *socketArray) {
	CRITICAL_SECTION sockets_cs;
	InitializeCriticalSection(&sockets_cs);
	initializeSockets(socketArray, DEFAULT_SOCARRLEN, &sockets_cs);
	DeleteCriticalSection(&sockets_cs);
}

void createThreadArray(ThreadArray *threadArray, SocketArray *socketArray, Queue *queue) {
	CRITICAL_SECTION thread_cs;
	InitializeCriticalSection(&thread_cs);
	initializeThreads(threadArray, DEFAULT_THREADARRLEN, &thread_cs);
	threadArray->count = 3;
	DeleteCriticalSection(&thread_cs);

	DWORD clientID, serviceID, gcID;

	CSParams csParams;
	csParams.queue = queue;
	csParams.socketArray = socketArray;
	csParams.threadArray = threadArray;
	threadArray->threads[1] = CreateThread(NULL, 0, &ServerServerThread, &csParams/* IZMENITI */, 0, &serviceID);

	threadArray->threads[0] = CreateThread(NULL, 0, &ClientServerThread, &csParams, 0, &clientID);
	//WaitForSingleObject(threadArray->threads[0], INFINITE);
	Sleep(INFINITE);
	/*
	while (1) {
		threadArray->threads[2] = CreateThread(NULL, 0, &GarbageCollector, &csParams/* IZMENITI , 0, &gcID);
	}
*/
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
