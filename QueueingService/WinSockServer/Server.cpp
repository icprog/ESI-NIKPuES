#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "../Buffer/Buffer.h";
#include "../Queue/Queue.h";
#include "../SocketNB/SocketNB.h";
#include "../SocketArray/SocketArray.h";
#include "../Thread/Thread.h";
#include "../ThreadArray/ThreadArray.h";
#include "../Thread/Util.h";

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27017"
#define DEFAULT_SOCARRLEN 10
#define DEFAULT_THREADARRLEN 10
#define INITIAL_QUEUE_SIZE 10
void createQueue(Queue *queue);
void createSocketArray(SocketArray *socketArray);
void createThreadArray(ThreadArray *threadArray, SocketArray *socketArray, Queue *queue, int odgovor);

bool InitializeWindowsSockets();
int  main(void) 
{
	if (InitializeWindowsSockets() == false)
	{
		// we won't log anything since it will be logged
		// by InitializeWindowsSockets() function
		return 1;
	}
	int odgovor = -1;
	
	do {
		printf("Da li se servis ponasa kao server? ");
		printf("\n\t1) DA");
		printf("\n\t2) NE");
		printf("\n>(Unesite 1 ili 2) ");
		scanf("%d", &odgovor);
	} while (odgovor != 1 && odgovor != 2);

	/*
	Kreiranje redova( buffera) i niza redova(queuea )
	*/
	// Resolve the server address and port

	
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
	createThreadArray(&threadArray, &sockets, &queue, odgovor);


	DWORD clientID, serviceID, gcID;
	SOCKET serviceSocket = INVALID_SOCKET;
	// Socket used for listening for new clients 
	SOCKET listenSocket = INVALID_SOCKET;
	SOCKET acceptedSocket = INVALID_SOCKET;
	char recvbuf[DEFAULT_BUFLEN];
	// variable used to store function return value
	int iResult;
	CSParams csParams;
	csParams.queue = &queue;
	csParams.socketArray = &sockets;
	csParams.threadArray = &threadArray;
	csParams.serviceSocket = &serviceSocket;
	csParams.odgovor = odgovor;
	unsigned long int nonBlockingMode = 1;


	if (odgovor == 1) {
		printf("\n Ovaj servis je server.\n Cekam klijenta da inicira komunikaciju...");


	
		iResult = listenSocketFunc(&listenSocket, DEFAULT_PORT);
		if (iResult == 1) {
			// Ako je ovde greska, kraj rada
			return 1;
		}
		do
		{
			iResult = ioctlsocket(acceptedSocket, FIONBIO, &nonBlockingMode);
			select(&listenSocket);
			// Wait for clients and accept client connections.
			// Returning value is acceptedSocket used for further
			// Client<->Server communication. This version of
			// server will handle only one client.
			iResult = accept(&acceptedSocket, &listenSocket);
			if (iResult == 1) {
				// Ako je ovde greska, kraj rada
				return 1;
			}
			receiveServerAsServer(&serviceSocket, &acceptedSocket, recvbuf);

			// here is where server shutdown loguc could be placed

		} while (1);

		// shutdown the connection since we're done
		//iResult = shutdown(acceptedSocket, SD_SEND);
		/*
		if (iResult == SOCKET_ERROR)
		{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(acceptedSocket);
		WSACleanup();
		return 1;
		}*/

		// cleanup
		//closesocket(listenSocket);
		//closesocket(acceptedSocket);
		//WSACleanup();

	}
	else {
		printf("\n Ovaj servis je klijent.\n Iniciram komunikaciju sa serverom...");



		char *message;
		createMessage(message, 160, "RED1", 4, "Uspostavljena konekcija sa serverom...", 's');
		// create a socket

		iResult = createSocket(&acceptedSocket, "127.0.0.1", 27017);
		if (iResult != 0) {
			WSACleanup();
			return 1;
		}

		// Send an prepared message with null terminator included

		iResult = sendMessage(&acceptedSocket, message);
		if (iResult != 0) {
			return 1;
		}


		receiveServerAsClient(&serviceSocket, &acceptedSocket, recvbuf);

		free(message);
		// cleanup
		//closesocket(connectSocket);
		//WSACleanup();
	}



	//Sleep(INFINITE);
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

void createThreadArray(ThreadArray *threadArray, SocketArray *socketArray, Queue *queue, int odgovor) {
	printf("\nCREATE THREAD");
	CRITICAL_SECTION thread_cs;
	InitializeCriticalSection(&thread_cs);
	initializeThreads(threadArray, DEFAULT_THREADARRLEN, &thread_cs);
	threadArray->count = 3;
	DeleteCriticalSection(&thread_cs);

	//threadArray->threads[1] = CreateThread(NULL, 0, &ServerServerThread, &csParams/* IZMENITI */, 0, &serviceID);

	//threadArray->threads[0] = CreateThread(NULL, 0, &ClientServerThread, &csParams, 0, &clientID);

	//WaitForSingleObject(serverToServer, INFINITE);

	//WaitForSingleObject(threadArray->threads[0], INFINITE);

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
