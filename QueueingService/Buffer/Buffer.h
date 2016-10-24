#pragma once
#include "stdafx.h"
/* CIRCULAR BUFFER INTERFACE */
typedef struct buffer {
	char *name;
	char *data;
	int pushIdx;
	int popIdx;
	int count;
	int size;
	CRITICAL_SECTION cs;
} Buffer;

void expand(Buffer *buffer);					          // expand the buffer size
int push(Buffer *buffer, char *data);					  // adding data to the buffer
int pop(Buffer *buffer, char *data);					  // removing data from the buffer
void shrink(Buffer *buffer);							  // shrink the buffer size when it's fulness is less or equal 25%
void createBuffer(Buffer *buffer, char *name, int bufferLength, CRITICAL_SECTION *cs);		  // create buffer
void destroyBuffer(Buffer *buffer);
int DataNameSize(char * data);
char* parseMessage(char *data, CRITICAL_SECTION *cs);						  //parse Mesage, get buffer name
char getCharacter(char * data, CRITICAL_SECTION *cs);