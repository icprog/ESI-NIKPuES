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

int add(Buffer *buffer, char *data, int dataSize);            // adding data to the buffer
<<<<<<< HEAD
void remove(Buffer *buffer, char *data, int dataSize);         // removing data from the buffer
=======
int remove(Buffer *buffer, char *data, int dataSize);         // removing data from the buffer
>>>>>>> f3330c768d4b2e7678329bb45d83b70299369c5d
void expand(Buffer *buffer, int howMuch);					   // expand the buffer size
void shrink(Buffer *buffer);                     // shrink the buffer size when it's fulness is less or equal 25%
int isEnough(int dataLength, int bufferLength);  //is enough place for data in buffer
int fullness(Buffer buffer);                    // returns fullnes in procents( 25 means 25%)

/* CIRCULAR BUFFER INTERFACE */