#pragma once
#include "stdafx.h"
/* QUEUE INTERFACE */
typedef struct queue {
	Buffer *buffer;
	int count;
	int size;
	CRITICAL_SECTION cs;
}Queue;

void addBuffer(Queue *queue, Buffer *buffer);          //queue je niz, a buffer je pokazivac na buffer
void removeBuffer(Queue *queue, Buffer *buffer);
void clearQueue(Queue *queue);
void expandQueue(Queue *queuek);
void initializeQueue(Queue *queue, int size, CRITICAL_SECTION *cs/*SRWLOCK *srwLock*/);
Buffer * findBuffer(Queue * queue, char * name);