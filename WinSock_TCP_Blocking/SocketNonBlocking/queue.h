#pragma once
#include "stdafx.h"
#include "util.h"

typedef struct queue {
	Buffer *buffer;
	int count;
	int size;
} Queue;

void initializeQueue(Queue *queue, int size);
void addBuffer(Queue *queue, Buffer *buffer);     
void removeBuffer(Queue *queue, Buffer *buffer);
void clearQueue(Queue *queue);
void expandQueue(Queue *queue);