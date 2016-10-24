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
#define DEFAULT_PORT "27017"
#define DEFAULT_SOCARRLEN 10
#define DEFAULT_THREADARRLEN 10
#define INITIAL_QUEUE_SIZE 10
void createQueue(Queue *queue);
void createSocketArray(SocketArray *socketArray);
void createThreadArray(ThreadArray *threadArray, SocketArray *socketArray, Queue *queue, SOCKET *serviceSocket);
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
	SOCKET *serviceSocket;
	if (odgovor == 1) {
		printf("\n Ovaj servis je server.\n Cekam klijenta da inicira komunikaciju...");

		// Socket used for listening for new clients 
		SOCKET listenSocket = INVALID_SOCKET;
		// Socket used for communication with client
		SOCKET acceptedSocket = INVALID_SOCKET;
		// variable used to store function return value
		int iResult;
		// Buffer used for storing incoming data
		char recvbuf[DEFAULT_BUFLEN];

		if (InitializeWindowsSockets() == false)
		{
			// we won't log anything since it will be logged
			// by InitializeWindowsSockets() function
			return 1;
		}

		// Prepare address information structures
		addrinfo *resultingAddress = NULL;
		addrinfo hints;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;       // IPv4 address
		hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
		hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
		hints.ai_flags = AI_PASSIVE;     // 

										 // Resolve the server address and port
		iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &resultingAddress);
		if (iResult != 0)
		{
			printf("getaddrinfo failed with error: %d\n", iResult);
			WSACleanup();
			return 1;
		}

		// Create a SOCKET for connecting to server
		listenSocket = socket(AF_INET,      // IPv4 address famly
			SOCK_STREAM,  // stream socket
			IPPROTO_TCP); // TCP

		if (listenSocket == INVALID_SOCKET)
		{
			printf("socket failed with error: %ld\n", WSAGetLastError());
			freeaddrinfo(resultingAddress);
			WSACleanup();
			return 1;
		}

		// Setup the TCP listening socket - bind port number and local address 
		// to socket
		iResult = bind(listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			printf("bind failed with error: %d\n", WSAGetLastError());
			freeaddrinfo(resultingAddress);
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}

		// Since we don't need resultingAddress any more, free it
		freeaddrinfo(resultingAddress);
		unsigned long int nonBlockingMode = 1;
		iResult = ioctlsocket(listenSocket, FIONBIO, &nonBlockingMode);
		// Set listenSocket in listening mode
		iResult = listen(listenSocket, SOMAXCONN);
		if (iResult == SOCKET_ERROR)
		{
			printf("listen failed with error: %d\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}

		printf("Server initialized, waiting for clients.\n");

		do
		{
			iResult = ioctlsocket(acceptedSocket, FIONBIO, &nonBlockingMode);
			FD_SET set;
			timeval timeVal;

			do {
				iResult = 0;
				FD_ZERO(&set);
				// Add socket we will wait to read from
				FD_SET(listenSocket, &set);

				// Set timeouts to zero since we want select to return
				// instantaneously
				timeVal.tv_sec = 0;
				timeVal.tv_usec = 0;
				iResult = select(0 /* ignored */, &set, NULL, NULL, &timeVal);

				// lets check if there was an error during select
				if (iResult == SOCKET_ERROR)
				{
					fprintf(stderr, "select failed with error: %ld\n", WSAGetLastError());
					return -1; //error code: -1
				}

				// now, lets check if there are any sockets ready
				if (iResult == 0)
				{
					//printf("\n SPAVAM! ");
					// there are no ready sockets, sleep for a while and check again
					Sleep(200);
				}

			} while (iResult == 0);
			// Wait for clients and accept client connections.
			// Returning value is acceptedSocket used for further
			// Client<->Server communication. This version of
			// server will handle only one client.
			acceptedSocket = accept(listenSocket, NULL, NULL);

			if (acceptedSocket == INVALID_SOCKET)
			{
				printf("accept failed with error: %d\n", WSAGetLastError());
				closesocket(listenSocket);
				WSACleanup();
				return 1;
			}

			do
			{
				// Receive data until the client shuts down the connection
				iResult = RECEIVE(&acceptedSocket, recvbuf);
				if (iResult > 0)
				{
					printf("Message received from client: %s.\n", recvbuf);

					// Send an prepared message with null terminator included
					char *messageToSend = " Uspostavljena konekcija sa klijentom...";
					char *data = (char *)malloc(sizeof(char) * 160);
					memset(data, 0, 160);
					data[0] = 160;

					*(char*)((int *)data + 1) = 4;
					char *ime = "RED1";

					memcpy(data + 8, ime, 4);
					memcpy(data + 12, messageToSend, 160);
					iResult = SEND(&acceptedSocket, data);

					if (iResult == SOCKET_ERROR)
					{
						printf("send failed with error: %d\n", WSAGetLastError());
						closesocket(acceptedSocket);
						WSACleanup();
						return 1;
					}

					// TODO: Naci gde zatvoriti ovaj accepted socekt
					serviceSocket = &acceptedSocket;


				}
				else if (iResult == 0)
				{
					// connection was closed gracefully
					printf("Connection with client closed.\n");
					closesocket(acceptedSocket);
				}
				else
				{
					// there was an error during recv
					printf("recv failed with error: %d\n", WSAGetLastError());
					closesocket(acceptedSocket);
				}
			} while (iResult > 0);

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

		// socket used to communicate with server
		SOCKET connectSocket = INVALID_SOCKET;
		// variable used to store function return value
		int iResult;
		// message to send
		char *messageToSend = "Uspostavljena konekcija sa serverom..";
		/////////////////////////////////////////////////////////////////////////////////
		char *data = (char *)malloc(sizeof(char) * 160);
		memset(data, 0, 160);
		data[0] = 160;

		*(char*)((int *)data + 1) = 4;
		char *ime = "RED1";

		memcpy(data + 8, ime, 4);
		memcpy(data + 12, messageToSend, 160);
		

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
		serverAddress.sin_addr.s_addr = inet_addr("192.168.101.110");
		serverAddress.sin_port = htons(27017);
		// connect to server specified in serverAddress and socket connectSocket
		if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
		{
			printf("Unable to connect to server.\n");
			closesocket(connectSocket);
			WSACleanup();
		}

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
		// Buffer used for storing incoming data
		char recvbuf[DEFAULT_BUFLEN];
		do
		{
			// Receive data until the client shuts down the connection
			iResult = RECEIVE(&connectSocket, recvbuf);
			if (iResult > 0)
			{
				printf("Message received from client: %s.\n", recvbuf+12);
				// TODO: Naci gde zatvoriti ovaj accepted socekt
				serviceSocket = &connectSocket;
			}
			else if (iResult == 0)
			{
				// connection was closed gracefully
				printf("Connection with client closed.\n");
				closesocket(connectSocket);
			}
			else
			{
				// there was an error during recv
				printf("recv failed with error: %d\n", WSAGetLastError());
				closesocket(connectSocket);
			}
		} while (iResult > 0);

		// cleanup
		//closesocket(connectSocket);
		//WSACleanup();
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
	createThreadArray(&threadArray, &sockets, &queue, serviceSocket);


	Sleep(INFINITE);
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

void createThreadArray(ThreadArray *threadArray, SocketArray *socketArray, Queue *queue, SOCKET *serviceSocket) {
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
	csParams.serviceSocket = serviceSocket;
	//threadArray->threads[1] = CreateThread(NULL, 0, &ServerServerThread, &csParams/* IZMENITI */, 0, &serviceID);

	threadArray->threads[0] = CreateThread(NULL, 0, &ClientServerThread, &csParams, 0, &clientID);
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
