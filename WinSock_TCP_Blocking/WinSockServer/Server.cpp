#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#include "C:/Users/ra64-2012/Desktop/Blok1/ESI-NIKPuES/WinSock_TCP_Blocking/SocketNonBlocking/socketNB.h" //davor
#include "C:/Users/RA4-2012/Documents/ESI-NIKPuES/WinSock_TCP_Blocking/SocketNonBlocking/socketNB.h"
#include "C:/Users/RA4-2012/Documents/ESI-NIKPuES/WinSock_TCP_Blocking/SocketNonBlocking/util.h"


#define DEFAULT_BUFLEN 512
#define INITIAL_QUEUE_SIZE 10
#define DEFAULT_PORT "27016"
#define SOCKET_ARRAY_INITIAL_SIZE 10
#define THREAD_ARRAY_SIZE 6
// struct for parameters of clientWaitingThreadFunc

typedef struct clientWaitingParams {
	Queue *queue;

} CL_PARAMS;


typedef struct receiveThreadParam {
	SOCKET *acceptedSocket;
	SRWLOCK *srwLock;
	Queue *queue;
} ReceiveThreadParam;

bool InitializeWindowsSockets();


DWORD WINAPI ReceiveThread(LPVOID lpParam) {
	printf("\n Usao sam u thread za obradu!\n");
	int iResult = 0;
	SOCKET acceptedSocket = *((ReceiveThreadParam*)lpParam)->acceptedSocket;
	Queue queue = *((ReceiveThreadParam*)lpParam)->queue;
	SRWLOCK srwLock = *((ReceiveThreadParam*)lpParam)->srwLock;
	char recvbuf[DEFAULT_BUFLEN];
	memset(recvbuf, 0, DEFAULT_BUFLEN);
	printf("\n Pokusacu da obradim zahtev!\n");
	do
	{
		// Receive data until the client shuts down the connection
		//iResult = recv(acceptedSocket, recvbuf, DEFAULT_BUFLEN, 0);
		iResult = RECEIVE(acceptedSocket, recvbuf);
		//recvbuf je data!!!
		printf("\nIresult = %d!\n", iResult);
		if (iResult > 0)
		{
			//uzmemo ime iz poruke
			char * name;
			name = parseMessage(recvbuf);
			printf("\niResult je veci od nule, ocigledno!");
			//pronadjemo bufer koji nam treba
			Buffer * buffer=findBuffer(&queue, name);
			

			add(buffer, recvbuf, &srwLock); //punimo bafer

			printf("Message received from client: %s.\n", recvbuf);

		}
		else if (iResult == 0)
		{
			// connection was closed gracefully
			printf("Connection with client closed.\n");
			//closesocket(acceptedSocket);
		}
		else
		{
			// there was an error during recv
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(acceptedSocket);
		}
	} while (iResult > 0);

	// here is where server shutdown loguc could be placed

	return 0;
}


/* Funkcija programske niti zaduzene za cekanje na klijenta.*/
DWORD WINAPI clientWaitingThreadFunc(LPVOID param) {
	CL_PARAMS* clParams = (CL_PARAMS*)param;
	SRWLOCK srwLock;
	//InitializeSRWLock(&srwLock);
	//ReleaseSRWLockExclusive(&srwLock);
	// ARRAY OF THREAD HANDLES
	//HANDLE threadArray[THREAD_ARRAY_SIZE];
	//memset(threadArray, NULL, clParams->queue->count);

	// Socket used for listening for new clients 
	SOCKET listenSocket = INVALID_SOCKET;
	// Socket used for communication with client
	SOCKET socketArray[SOCKET_ARRAY_INITIAL_SIZE];
	memset(socketArray, INVALID_SOCKET, SOCKET_ARRAY_INITIAL_SIZE);

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

	// SET LISTEN SOCKET TO NON BLOCKING
	unsigned long int nonBlockingMode = 1;
	iResult = ioctlsocket(listenSocket, FIONBIO, &nonBlockingMode);

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

	// Set socket to nonblocking mode
	//unsigned long int nonBlockingMode = 1;
	//iResult = ioctlsocket(acceptedSocket, FIONBIO, &nonBlockingMode);

	// Since we don't need resultingAddress any more, free it
	freeaddrinfo(resultingAddress);

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
		// Wait for clients and accept client connections.
		// Returning value is acceptedSocket used for further
		// Client<->Server communication. This version of
		// server will handle only one client.
		bool socketArrayFull = 1;                             // da li su svi soketi u upotrebi?
		for (int i = 0; i < SOCKET_ARRAY_INITIAL_SIZE; i++) {
			if (socketArray[i] == INVALID_SOCKET) {
				socketArrayFull = 0;

				iResult = ioctlsocket(socketArray[i], FIONBIO, &nonBlockingMode);
				FD_SET set;
				timeval timeVal;
				
				do{
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
						printf("\n SPAVAM! ");
						// there are no ready sockets, sleep for a while and check again
						Sleep(200);
					}

				} while (iResult == 0);
				
				socketArray[i] = accept(listenSocket, NULL, NULL); /////////////////

				if (socketArray[i] == INVALID_SOCKET)
				{
					printf("accept failed with error: %d\n", WSAGetLastError());
					closesocket(listenSocket);
					WSACleanup();
					return 1;
				}
				printf("\n KLIJENT JE PRIHVACEN!\n");
				// OKIDA THREAD ZA RECEIVE
				//ReleaseSRWLockExclusive(&srwLock);
				ReceiveThreadParam rtParam;
				rtParam.acceptedSocket = &socketArray[i];
				rtParam.queue = clParams->queue;
				rtParam.srwLock = &srwLock;
				HANDLE receiveThread;
				DWORD receiveThreadID;
				printf("\n Okidam nit za obradu!\n");
				receiveThread = CreateThread(0, 0, &ReceiveThread, &rtParam, 0, &receiveThreadID);
				WaitForSingleObject(receiveThread, INFINITE);
				printf("\n Zahtev obradjen!\n");
				// shutdown the connection since we're done
				//iResult = shutdown(socketArray[i], SD_SEND);
			//	if (iResult == SOCKET_ERROR)
			//	{
			//		printf("shutdown failed with error: %d\n", WSAGetLastError());
			//		closesocket(socketArray[i]);
					//WSACleanup();
					//return 1;
			//	}
				//closesocket(socketArray[i]);
				CloseHandle(receiveThread);
				socketArray[i] = INVALID_SOCKET;
				break;

			}
		}
		//Ako nijedan socket iz niza nije slobodan uspavaj nit na neko vreme
		if (socketArrayFull)
			Sleep(200);


	} while (1);



	// cleanup
	closesocket(listenSocket);
	WSACleanup();
}



int  main(void) 
{
	enum CONNECTION { CLIENT = 1, SERVER=2};
	int answer = -1;
	char *serviceIp = (char *)malloc(sizeof(char)*32); //other service ip
	memset(serviceIp, 0, 32);
	int servicePort = -1; // other service port
	SRWLOCK srwLock;
	InitializeSRWLock(&srwLock);
	do {
		printf("Aplikacija se inicijalno ponasa kao: ");
		printf("\n\t1) KLIJENT");
		printf("\n\t2) SERVER");
		printf("\n>  ");
		scanf("%d", &answer);
		printf("\nOdgovor: %d", answer);
	} while (answer < CLIENT && answer > SERVER);

	printf("klijent. Pokusace se inicijalizacija  i komunikacija sa serverom.");
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

	createBuffer(&red1, imered1, DEFAULT_BUFLEN, &srwLock);
	createBuffer(&red11, imered11, DEFAULT_BUFLEN, &srwLock);
	createBuffer(&red2, imered2, DEFAULT_BUFLEN, &srwLock);
	createBuffer(&red21, imered21, DEFAULT_BUFLEN, &srwLock);
	createBuffer(&red3, imered3, DEFAULT_BUFLEN, &srwLock);
	createBuffer(&red31, imered31, DEFAULT_BUFLEN, &srwLock);

	Buffer *bufferArray = (Buffer *)malloc(sizeof(Buffer)*INITIAL_QUEUE_SIZE);
	bufferArray[0] = red1;
	bufferArray[1] = red11;
	bufferArray[2] = red2;
	bufferArray[3] = red21;
	bufferArray[4] = red3;
	bufferArray[5] = red31;
	Queue queue;
	initializeQueue(&queue, INITIAL_QUEUE_SIZE, &srwLock);
	queue.buffer = bufferArray;
	queue.count = 6;


	printf("\nOva aplikacija ce se ponasati kao: ");
	if (answer == CLIENT){
		printf("klijent. Pokusavam da komuniciram sa drugim serverom.\n");
		// socket used to communicate with server
		SOCKET connectSocket = INVALID_SOCKET;
		// variable used to store function return value
		int iResult;
		// message to send
		char *messageToSend = "Pozdrav sa servisa koji se ponasa kao klijent!";
		printf("Unesite IP adresu drugog servisa: \n");
		scanf("%s", &serviceIp);
		printf("Unesite port: \n");
		scanf("%d", &servicePort);


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
		serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
		serverAddress.sin_port = htons(27016);
		// connect to server specified in serverAddress and socket connectSocket
		while (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
		{
			printf("Unable to connect to server.\n");
			closesocket(connectSocket);
			WSACleanup();
		}
		printf("Uspeo sam da se konektujem na drugi servis! \n");

		// Set socket to nonblocking mode
		unsigned long int nonBlockingMode = 1;
		iResult = ioctlsocket(connectSocket, FIONBIO, &nonBlockingMode);

		// Send an prepared message with null terminator included
		iResult = SEND(connectSocket, messageToSend);

		if (iResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return 1;
		}

		printf("Bytes Sent: %ld\n", iResult);

		// cleanup
		closesocket(connectSocket);
		WSACleanup();

		return 0;
	}
	else{
		printf("server. Cekam drugi server da inicira komunikaciju.\n");
	}


	HANDLE clienteWaitingThread;
	DWORD clienteWaitingThreadID;

	CL_PARAMS clParams;
	clParams.queue = &queue;

	clienteWaitingThread = CreateThread(NULL, 0, &clientWaitingThreadFunc, &clParams, 0, &clienteWaitingThreadID);
	WaitForSingleObject(clienteWaitingThread, INFINITE);


    return 0;
}

bool InitializeWindowsSockets()
{
    WSADATA wsaData;
	// Initialize windows sockets library for this process
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return false;
    }
	return true;
}
