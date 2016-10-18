#include "util.h"
#include <stdlib.h>
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

int add(Buffer *buffer, char * data, int dataSize)
{
	// ako je bafer vec pun count == size, radi prosirivanje, ali prvo utvrdi za koliko puta
	// ili ako je velicina podataka veca od velicine ostatka slobodnog prostora u baferu
	if ((buffer->count >= buffer->size) || ((buffer->size - buffer->count) < dataSize)) {
		if (dataSize > buffer->size * 2) {
			//expand(buffer, dataSize/ buffer->size + 1);
		}
		else
			//expand(buffer, 0);
			printf("EXPAND");

	}
	
	for (int i = 0; i < dataSize; i++) {
		buffer->data[buffer->pushIdx] = data[i];
		buffer->pushIdx++;

		if (buffer->pushIdx == buffer->size)
			buffer->pushIdx = 0;
	}
	
	buffer->count += dataSize;

	return 0;
}

void shrink(Buffer * buffer)
{
	double fullness = buffer->count / buffer->size;

	// ako je bafer popunjen manje od jedne cetvrtine smanji ga za pola
	if (fullness <= 0.25) {
		char *newData;
		int newSize = 0;

		// za slucaj da bafer nije bio povecavan uvek za 2 puta
		if (buffer->size % 2 == 0)
			newSize = buffer->size / 2;
		else
			newSize = buffer->size / 2 + 2;

		// malloc new array
		newData = (char *)malloc(sizeof(char) * newSize);      // allocate 50 ints

		
		// prepisi podatke
		for (int i = 0; i < buffer->count; i++) {
			// ako je pokazivac na prvi za citanje ujedno i poslednja lokacija, postavi ga na 0
			if (buffer->popIdx == buffer->size)
				buffer->popIdx = 0;

			newData[i] = buffer->data[i];
		}

		buffer->data = newData;
		buffer->size = newSize;
		buffer->pushIdx = buffer->count;
		buffer->popIdx = 0;



	}
}