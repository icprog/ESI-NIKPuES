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

