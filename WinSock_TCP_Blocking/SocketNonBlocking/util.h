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

void expand(Buffer *buffer);					 // expand the buffer size
int add(Buffer *buffer, char *data);             // adding data to the buffer
int remove(Buffer *buffer, char *data);          // removing data from the buffer
void shrink(Buffer *buffer);                     // shrink the buffer size when it's fulness is less or equal 25%
Buffer *createBuffer(char *name, int bufferLength); // create buffer
void destroyBuffer(Buffer *buffer);              // destroy buffer


/* CIRCULAR BUFFER INTERFACE */