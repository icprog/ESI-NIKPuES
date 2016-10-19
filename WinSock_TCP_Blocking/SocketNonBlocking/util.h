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
int add(Buffer *buffer, char *data);					  // adding data to the buffer
int remove(Buffer *buffer, char *data);					  // removing data from the buffer
void shrink(Buffer *buffer);							  // shrink the buffer size when it's fulness is less or equal 25%
void createBuffer(Buffer *buffer, char *name, int bufferLength);		  // create buffer
void destroyBuffer(Buffer *buffer);						  // destroy buffe

/* CIRCULAR BUFFER INTERFACE */


/* QUEUE INTERFACE */
typedef struct queue {
	Buffer *buffer;
	int count;
	int size;
}Queue;

void addBuffer(Queue *queue, Buffer *buffer);          //queue je niz, a buffer je pokazivac na buffer
void removeBuffer(Queue *queue, Buffer *buffer);
void clearQueue(Queue *queue);
void expandQueue(Queue *queue);
void initializeQueue(Queue *queue, int size);
/* QUEUE INTERFACE */