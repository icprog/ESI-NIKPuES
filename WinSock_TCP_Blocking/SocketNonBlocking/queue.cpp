#include "queue.h"
#include "stdafx.h"

void removeBuffer(Queue * queue, Buffer * buffer)
{
	//smanji count i prolazi kroz sve elemente niza i obrisi bafer sa yadatim imenom
	queue->count--;

	for (int i = 0; i < queue->count; i++) {
		if (strcmp(queue->buffer[i]->name, buffer->name) == 0) {
			queue->buffer[i] = NULL;
			destroyBuffer(queue->buffer[i]);
			break;
		}	
	}
}

void clearQueue(Queue * queue)
{ 
	//prodji kroz sve elemente reda i obrisi ih
	for (int i = 0; i < queue->count; i++) {
		queue->buffer[i] = NULL;
		destroyBuffer(queue->buffer[i]);
		break;
	}
}


