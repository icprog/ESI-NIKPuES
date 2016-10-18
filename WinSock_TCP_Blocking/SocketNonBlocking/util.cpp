#include "util.h"
#include <stdlib.h>


int remove(Buffer * buffer, char * data, int dataSize)
{
	//ako se poklapaju pop i pushIdx, nemamo podataka
	/*if (buffer->popIdx == buffer->pushIdx) {
		return -1; //vrati gresku
	}*/


	if (buffer->count == 0) {
		return false;
	}

	for (int i = 0; i < dataSize; i++) {

		data[i] = buffer->data[buffer->popIdx];

		buffer->popIdx ++;

		if (buffer->popIdx == buffer->size) {
			buffer->popIdx = 0;
		}
	}

	buffer->count -= dataSize;

}

void expand(Buffer * buffer, int howMuch)
{
	char* newData;
	int newSize;

	if (howMuch == 0) { 
		newSize = buffer->size * 2; 
		newData = (char *)malloc(sizeof(char)*newSize);
	}
	else {
		newSize = buffer->size * howMuch;
		newData = (char *)malloc(sizeof(char)*newSize);
	}

	//kada povecamo moramo da premestimo podatke sa kraja starog buffer-a na pocetak novog buffer-a
	for (int i = 0; i < buffer->count; i++) {
		if (buffer->popIdx == buffer->size)
			buffer->popIdx = 0;

		newData[i] = buffer->data[i];
	}

	buffer->data = newData;
	buffer->size = newSize;
	buffer->pushIdx = buffer->count;
	buffer->popIdx = 0;

}
