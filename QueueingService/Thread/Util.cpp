#include "Util.h"
#include "stdafx.h"

enum codes { SUCCESS = 0, SOCK_ERR = -1, SLEEP = -2, CONN_ERR = -3, SEND_ERR = -4, REC_ERR = -5 };

int listenSocketFunc(SOCKET * listenSocket, char * port)
{

	// variable used to store function return value
	int iResult;

	// Prepare address information structures
	addrinfo *resultingAddress = NULL;
	addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;       // IPv4 address
	hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
	hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
	hints.ai_flags = AI_PASSIVE;     // 

									 // Resolve the server address and port
	iResult = getaddrinfo(NULL, port, &hints, &resultingAddress);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	*listenSocket = socket(AF_INET,      // IPv4 address famly
		SOCK_STREAM,  // stream socket
		IPPROTO_TCP); // TCP

	if (*listenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(resultingAddress);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket - bind port number and local address 
	// to socket
	iResult = bind(*listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(resultingAddress);
		closesocket(*listenSocket);
		WSACleanup();
		return 1;
	}

	// Since we don't need resultingAddress any more, free it
	freeaddrinfo(resultingAddress);

	unsigned long int nonBlockingMode = 1;
	iResult = ioctlsocket(*listenSocket, FIONBIO, &nonBlockingMode);

	// Set listenSocket in listening mode
	iResult = listen(*listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(*listenSocket);
		WSACleanup();
		return 1;
	}

	printf("Server initialized, waiting for clients.\n");

	return iResult;
}

int select(SOCKET * socket)
{
	FD_SET set;
	timeval timeVal;
	int iResult;

	do {
		iResult = 0;
		FD_ZERO(&set);
		// Add socket we will wait to read from
		FD_SET(*socket, &set);

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

	return iResult;
}

int accept(SOCKET * acceptedSocket, SOCKET* listenSocket)
{
	*acceptedSocket = accept(*listenSocket, NULL, NULL);

	if (*acceptedSocket == INVALID_SOCKET)
	{
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(*listenSocket);
		WSACleanup();
		return 1;
	}
	return SUCCESS;
}
void createMessage(char * data, int length, char * name, int nameLength, char * message, char c)
{


	data[0] = length;

	*(char*)((int *)data + 1) = nameLength;
	*(data + 8) = c;

	memcpy(data + 9, name, nameLength);
	memcpy(data + nameLength, message, length);


}

int createSocket(SOCKET * ssocket, char * ip, int port)
{
	// create a socket
	*ssocket = socket(AF_INET,
		SOCK_STREAM,
		IPPROTO_TCP);

	if (*ssocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		//WSACleanup();
		return SOCK_ERR;
	}

	// create and initialize address structure
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(ip);
	serverAddress.sin_port = htons(port);
	// connect to server specified in serverAddress and socket connectSocket
	if (connect(*ssocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		printf("Unable to connect to server.\n");
		closesocket(*ssocket);
		//WSACleanup();
		return CONN_ERR;
	}

	return SUCCESS;
}

int sendMessage(SOCKET * socket, char * data)
{
	int iResult = -1;
	// Send an prepared message with null terminator included

	printf("\nSENDING MESSAGE: %s", data + 9);

	iResult = SEND(socket, data);

	if (iResult == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(*socket);
		//WSACleanup();
		return SEND_ERR;
	}

	printf("\nMESSAGE SENT! Bytes Sent: %ld\n", iResult);
	return SUCCESS;
}

int receiveServerAsClient(SOCKET* serviceSocket, SOCKET *acceptedSocket, char * message)
{
	int iResult = -1;


// Receive data until the client shuts down the connection
		iResult = RECEIVE(acceptedSocket, message);
		printf("%d", iResult);
		if (iResult > 0)
		{
			printf("Message received from server as a server: %s.\n", message + 9);
			// TODO: Naci gde zatvoriti ovaj accepted socekt
			serviceSocket = acceptedSocket;
			iResult = 0;
		}
		if (iResult == 0)
		{
			// connection was closed gracefully
			printf("Connection with server established.\n");
			//closesocket(*acceptedSocket);
			return SUCCESS;
		}
		else
		{
			// there was an error during recv
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(*acceptedSocket);
			return REC_ERR;
		}


	return SUCCESS;
}
int receiveServerFromServer( SOCKET *acceptedSocket, char * message)
{
	int iResult = -1;


	// Receive data until the client shuts down the connection
	iResult = RECEIVE(acceptedSocket, message);
	printf("%d", iResult);
	if (iResult > 0)
	{
		printf("Message received from server as a server: %s.\n", message + 9);
		// TODO: Naci gde zatvoriti ovaj accepted socekt
		iResult = 0;
	}
	if (iResult == 0)
	{
		//closesocket(*acceptedSocket);
		return SUCCESS;
	}
	else
	{
		// there was an error during recv
		printf("recv failed with error: %d\n", WSAGetLastError());
		closesocket(*acceptedSocket);
		return REC_ERR;
	}


	return SUCCESS;
}
int receiveServerAsServer(SOCKET* serviceSocket, SOCKET *acceptedSocket, char * message)
{
	// Receive data until the client shuts down the connection
	int iResult = RECEIVE(acceptedSocket, message);
	if (iResult > 0)
	{
		printf("Message received from another server: %s.\n", message +9 );

		// Send an prepared message with null terminator included
		char *messageToSend = " Uspostavljena konekcija sa klijentom...";
		char *data = (char *)malloc(sizeof(char) * 160 +1);
		memset(data, 0, 160);
		createMessage(data, 160, "RED1", 4, "Uspostavljena konekcija sa klijentom...", 's');

		iResult = sendMessage(acceptedSocket, data);

		//free(data); ////////////////////////////////////////////FREE


		if (iResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(*acceptedSocket);
			WSACleanup();
			return 1;
		}

		// TODO: Naci gde zatvoriti ovaj accepted socekt
		serviceSocket = acceptedSocket;
		iResult = 0;

	}
	if (iResult == 0)
	{
		// connection was closed gracefully
		printf("Uspostavljena veza sa klijentom.\n");
		//closesocket(acceptedSocket);
		serviceSocket = acceptedSocket;
		return SUCCESS;
	}
	else
	{
		// there was an error during recv
		printf("recv failed with error: %d\n", WSAGetLastError());
		closesocket(*acceptedSocket);
		return REC_ERR;
	}
	return SUCCESS;
}


