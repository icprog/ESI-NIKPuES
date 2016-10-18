#include "util.h"
#include <stdlib.h>
<<<<<<< HEAD
#include "stdafx.h"
=======
>>>>>>> f3330c768d4b2e7678329bb45d83b70299369c5d

/* CIRCULAR BUFFER INTERFACE */
typedef struct buffer {
	char *name;
	char *data;
	int pushIdx;
	int popIdx;
	int count;
	int size;
} Buffer;

<<<<<<< HEAD
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
=======
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
>>>>>>> f3330c768d4b2e7678329bb45d83b70299369c5d
