#pragma once
#define BUFF_SIZE=1024

/* CIRCULAR BUFFER INTERFACE */
typedef struct buffer {
	char *name;
	char *data;
	int pushIdx;
	int popIdx;
} Buffer;

void add(Buffer buffer, char *data, int count);            // adding data to the buffer
void remove(Buffer buffer, char *data, int count);         // removing data from the buffer
void expand(Buffer buffer);					   // expand the buffer size
void shrink(Buffer buffer);                     // shrink the buffer size when it's fulness is less or equal 25%
int isEnough(int dataLength, int bufferLength);  //is enough place for data in buffer
int fullness(Buffer buffer);                    // returns fullnes in procents( 25 means 25%)
int size(Buffer buffer);                        // returns the size of the buffer
int count(Buffer buffer);                       // returns the number of elements currently in buffer
int pushIdx(Buffer buffer);                     // returns current position of pushIdx
int popIdx(Buffer buffer);                      // returns current position of popIdx
/* CIRCULAR BUFFER INTERFACE */