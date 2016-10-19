#include "util.h"
#include <stdlib.h>
#include <string.h>
#include "stdafx.h"

int dataSize(char * data) {
	//int dataSize=0;
	return *((int*)data);
	//return dataSize;
}


/* CIRCULAR BUFFER INTERFACE */
typedef struct buffer {
	char *name;
	char *data;
	int pushIdx;
	int popIdx;
	int count;
	int size;
} Buffer;

void expand(Buffer * buffer)
{
	// TODO 1: proveriti da li je bafer prazan


	char* newData;
	int newSize;
	//int velicina = dataSize(buffer->data);

	newSize = buffer->size * 2;
	newData = (char *)malloc(sizeof(char)*newSize);
	memset(newData, 0, newSize);
	if (buffer->pushIdx < buffer->popIdx) { // ako je push manji od pop indeksa, onda je data iz dva dela pa radimo 2 mem kopija
		int rest = buffer->size - buffer->popIdx;
		memcpy(newData, buffer->data + buffer->popIdx, rest); //kopiraj kraj starog bufera u novi 
		memcpy(newData + rest, buffer->data, buffer->pushIdx); //nastavi na novi data onaj data sa pocetka starog
	}
	else {
		memcpy(newData, buffer->data + buffer->popIdx, buffer->count);
	}

	//kada povecamo moramo da premestimo podatke sa kraja starog buffer-a na pocetak novog buffer-a

	/*for (int i = 0; i < buffer->count; i++) {
	if (buffer->popIdx == buffer->size)
	buffer->popIdx = 0;

	newData[i] = buffer->data[i];
	}*/

	free(buffer->data);  
	buffer->data = newData;
	buffer->size = newSize;
	buffer->pushIdx = buffer->count;
	buffer->popIdx = 0;


}


int add(Buffer *buffer, char * data)
{
	int sizeOfData = dataSize(data);

	// ako je bafer vec pun count == size, radi prosirivanje, ali prvo utvrdi za koliko puta
	// ili ako je velicina podataka veca od velicine ostatka slobodnog prostora u baferu
	if ((buffer->count == buffer->size) || ((buffer->size - buffer->count) < sizeOfData)) {
		if (sizeOfData > buffer->size * 2) {
			for (int i = 0; i < sizeOfData / buffer->size + 1; i++)
				expand(buffer);
		}
		else{
			expand(buffer);
		}

	}
	int rest = buffer->size - buffer->pushIdx + 1;
	if (sizeOfData > rest) {  
		//buffer->popIdx = buffer->pushIdx;
		memcpy(buffer->data + buffer->pushIdx, data, rest);
		memcpy(buffer->data, data + rest - 1, sizeOfData - rest);
		buffer->pushIdx = sizeOfData - rest;
		buffer->count += sizeOfData;
	}
	else {
		//buffer->popIdx = buffer->count;
		memcpy(buffer->data + buffer->pushIdx, data, sizeOfData);
		buffer->count += sizeOfData;
		buffer->pushIdx = buffer->count;
	}
	/*
	for (int i = 0; i < dataSize; i++) {
		buffer->data[buffer->pushIdx] = data[i];
		buffer->pushIdx++;

		if (buffer->pushIdx == buffer->size)
			buffer->pushIdx = 0;
	}
	*/
	
	/*debug output*/
	printf("\nSadrzaj bafera: ");
	for (int i = 0; i < buffer->size; i++) {
		printf("%c", buffer->data[i]);
	}
	printf("\nOstatak: \n");
	printf("PopIdx: %d\n", buffer->popIdx);
	printf("PushIdx: %d\n", buffer->pushIdx);
	printf("Count: %d\n", buffer->count);
	printf("Size: %d\n", buffer->size);
	/*end of debut output*/
	

	return 0;
}

void shrink(Buffer * buffer)
{

	int rest = buffer->size - buffer->popIdx;
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
		memset(newData, 0, newSize);
		
		if (buffer->pushIdx < buffer->popIdx) {
			int rest = buffer->size - buffer->popIdx;
			memcpy(newData, buffer->data + buffer->popIdx, rest);
			memcpy(newData + rest, buffer->data, buffer->pushIdx);
		}
		else {
			memcpy(newData, buffer->data+buffer->popIdx, buffer->count);
		}
		// prepisi podatke
		/*
		for (int i = 0; i < buffer->count; i++) {
			// ako je pokazivac na prvi za citanje ujedno i poslednja lokacija, postavi ga na 0
			if (buffer->popIdx == buffer->size)
				buffer->popIdx = 0;

			newData[i] = buffer->data[i];
		}
		*/

		free(buffer->data);
		buffer->data = newData;
		buffer->size = newSize;
		buffer->pushIdx = buffer->count;
		buffer->popIdx = 0;



	}

}

void destroyBuffer(Buffer * buffer)
{
	free(buffer->data);
	buffer->popIdx = -1;
	buffer->pushIdx = -1;
	buffer->count = -1;
	buffer->size = -1;
	buffer = NULL;
}

int remove(Buffer * buffer, char * data)
{
	//ako se poklapaju pop i pushIdx, nemamo podataka
	/*if (buffer->popIdx == buffer->pushIdx) {
		return -1; //vrati gresku
	}*/

	int velicina = dataSize(data);
	
	if (buffer->count == 0) {
		return 0;
	}

	//prvi slucaj da je data negde u sredini i onda je pomeramo na pocetak
	if (buffer->popIdx < buffer->pushIdx) {
		memcpy(data, buffer->data + buffer->popIdx, velicina); //kopiraj data na pocetak buffer-a
		memset(buffer->data + buffer->popIdx, 0, velicina);
		buffer->popIdx += velicina;
	}
	else {
	//iz dva dela, prvo kopiraj kraj data na pocetak, pa onda sa pocetka pomeri
		int rest = buffer->size - buffer->popIdx;
		memcpy(data,buffer->data+buffer->popIdx,rest);
		memcpy(data, buffer->data,velicina-rest);
		memset(buffer->data + buffer->popIdx, 0, rest);
		memset(buffer->data,  0, velicina - rest);
		buffer->popIdx = velicina - rest;
	}

	/*for (int i = 0; i < velicina; i++) {

		data[i] = buffer->data[buffer->popIdx];

		buffer->popIdx ++;

		if (buffer->popIdx == buffer->size) {
			buffer->popIdx = 0;
		}
	}*/

	buffer->count -= velicina;
	/*debug output*/
	printf("\nSadrzaj bafera: ");
	for (int i = 0; i < buffer->size; i++) {
		printf("%c", buffer->data[i]);
	}
	printf("\nOstatak: \n");
	printf("PopIdx: %d\n", buffer->popIdx);
	printf("PushIdx: %d\n", buffer->pushIdx);
	printf("Count: %d\n", buffer->count);
	printf("Size: %d\n", buffer->size);
	/*end of debut output*/
	return 0;
}

