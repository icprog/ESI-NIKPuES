#include "queue.h"
#include "stdafx.h"


typedef struct buffer {
	char *name;
	char *data;
	int pushIdx;
	int popIdx;
	int count;
	int size;
} Buffer;

typedef struct queue {
	Buffer *buffer;
	int count;
	int size;
} Queue;

void removeBuffer(Queue * queue, Buffer * buffer)
{
	//smanji count i prolazi kroz sve elemente niza i obrisi bafer sa yadatim imenom
	queue->count--;

	for (int i = 0; i < queue->count; i++) {
		if (strcmp(queue->buffer[i].name, buffer->name) == 0) {
			destroyBuffer(&(queue->buffer[i]));
			break;
		}	
	}
}

void clearQueue(Queue * queue)
{ 
	//prodji kroz sve elemente reda i obrisi ih
	for (int i = 0; i < queue->count; i++) {
		destroyBuffer(&(queue->buffer[i]));
		break;
	}
}


void expandQueue(Queue *queue) {
	Buffer *newArray;
	newArray = (Buffer *)malloc(sizeof(Buffer) * queue->size * 2 );
	queue->size *= 2;
	free(queue->buffer);
	queue->buffer = newArray;
}

void initializeQueue(Queue * queue, int size)
{
	queue->count = 0;
	queue->size = 0;
	queue->buffer = (Buffer *)malloc(sizeof(Buffer) * size);
	memset(queue->buffer, NULL, size);

}

void addBuffer(Queue * queue, Buffer * buffer)
{
	if (queue->count == queue->size)
		expandQueue(queue);

	queue->buffer[queue->count] = *buffer;
	
	queue->count++;

}
