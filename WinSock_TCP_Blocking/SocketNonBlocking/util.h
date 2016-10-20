#pragma once
#define BUFF_SIZE 1024
#include "stdafx.h"
/* CIRCULAR BUFFER INTERFACE */
typedef struct buffer {
	char *name;
	char *data;
	int pushIdx;
	int popIdx;
	int count;
	int size;
} Buffer;

void expand(Buffer *buffer);					          // expand the buffer size
int add(Buffer *buffer, char *data, SRWLOCK *srwLock);					  // adding data to the buffer
int remove(Buffer *buffer, char *data, SRWLOCK *srwLock);					  // removing data from the buffer
void shrink(Buffer *buffer, SRWLOCK *srwLock);							  // shrink the buffer size when it's fulness is less or equal 25%
void createBuffer(Buffer *buffer, char *name, int bufferLength, SRWLOCK *srwLock);		  // create buffer
void destroyBuffer(Buffer *buffer, SRWLOCK *srwLock);						  // destroy buffe
int DataNameSize(char * data);
char* parseMessage(char *data);						  //parse Mesage, get buffer name
/* CIRCULAR BUFFER INTERFACE */


/* QUEUE INTERFACE */
typedef struct queue {
	Buffer *buffer;
	int count;
	int size;
}Queue;

void addBuffer(Queue *queue, Buffer *buffer, SRWLOCK *srwLock);          //queue je niz, a buffer je pokazivac na buffer
void removeBuffer(Queue *queue, Buffer *buffer, SRWLOCK *srwLock);
void clearQueue(Queue *queue, SRWLOCK *srwLock);
void expandQueue(Queue *queuek);
void initializeQueue(Queue *queue, int size, SRWLOCK *srwLock);
void findBuffer(Queue * queue, Buffer * buffer, char * name);
/* QUEUE INTERFACE */
