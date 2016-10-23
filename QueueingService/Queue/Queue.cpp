#include "stdafx.h"
#include "Queue.h"

void expandQueue(Queue * queue)
{
	Buffer *newArray;
	newArray = (Buffer *)malloc(sizeof(Buffer) * queue->size * 2);
	queue->size *= 2;
	memcpy(newArray, queue->buffer, queue->size);
	free(queue->buffer);
	queue->buffer = newArray;
}

void addBuffer(Queue * queue, Buffer * buffer)
{

	EnterCriticalSection(&queue->cs);
	if (queue->count == queue->size)
		expandQueue(queue);

	for (int i = 0; i < queue->size; i++) {
		if (queue->buffer[i].name == NULL) {          // ne ime
			queue->buffer[i] = *buffer;
			break;
		}
	}
	queue->buffer[queue->count] = *buffer;

	queue->count++;
	LeaveCriticalSection(&queue->cs);

}

void removeBuffer(Queue * queue, Buffer * buffer)
{

	EnterCriticalSection(&queue->cs);
	//smanji count i prolazi kroz sve elemente niza i obrisi bafer sa yadatim imenom
	queue->count--;

	for (int i = 0; i < queue->count; i++) {
		if (strcmp(queue->buffer[i].name, buffer->name) == 0) {
			destroyBuffer(&(queue->buffer[i]));
			break;
		}
	}
	LeaveCriticalSection(&queue->cs);

}

void clearQueue(Queue * queue)
{

	EnterCriticalSection(&queue->cs);

	for (int i = 0; i < queue->count; i++) {
		destroyBuffer(&(queue->buffer[i]));
		break;
	}
	free(queue->buffer);
	LeaveCriticalSection(&queue->cs);
	DeleteCriticalSection(&queue->cs);

}

void initializeQueue(Queue * queue, int size, CRITICAL_SECTION *cs)
{

	EnterCriticalSection(cs);
	queue->count = 0;
	queue->size = size;
	queue->buffer = (Buffer *)malloc(sizeof(Buffer) * size);
	for (int i = 0; i < size; i++) {
		queue->buffer[i].name = NULL;
	}
	CRITICAL_SECTION queueCS;
	InitializeCriticalSection(&queueCS);
	queue->cs = queueCS;
	LeaveCriticalSection(cs);

}

Buffer * findBuffer(Queue * queue, char * name)
{
	for (int i = 0; i < queue->count; i++) {
		if (strcmp(queue->buffer[i].name, name) == 0) {

			//free(name);		//oslobadjamo ime koje smo malloc u funkciji parseMessage
			// HEAP CORRUPTION .

			return &queue->buffer[i];
		}
	}
	return NULL;
}
