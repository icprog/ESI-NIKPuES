#pragma once
#include "stdafx.h"
#include "util.h"

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

