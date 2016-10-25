#include "Buffer.h"
#include "stdafx.h"
typedef struct buffer {
	char *name;
	char *data;
	int pushIdx;
	int popIdx;
	int count;
	int size;
	CRITICAL_SECTION cs;
} Buffer;

int dataSize(char * data) {
	//int dataSize=0;
	return *((int*)data);
	//return dataSize;
}

int DataNameSize(char * data) {
	return *((int*)data + 1);
}
char getCharacter(char * data, CRITICAL_SECTION *cs)
{
	EnterCriticalSection(cs);
	char c;
	c = *data;
	LeaveCriticalSection(cs);
	return c;

	//oslobodi niz
}

void parseMessage(char * name, int nameSize, char * data, CRITICAL_SECTION *cs)
{
	EnterCriticalSection(cs);
	
	memset(name, 0, nameSize + 1);
	memcpy(name, data + 8, nameSize); //kopiraj kraj starog bufera u novi 
	LeaveCriticalSection(cs);
	//return name;
	
	//oslobodi niz
}

void createBuffer(Buffer * buffer, char * name, int bufferLength, CRITICAL_SECTION *cs)
{

	EnterCriticalSection(cs);
	buffer->name = name;
	buffer->count = 0;
	buffer->popIdx = 0;
	buffer->pushIdx = 0;
	buffer->size = bufferLength;
	buffer->data = (char *)malloc(sizeof(char) * bufferLength + 1);
	memset(buffer->data, 0, bufferLength);
	CRITICAL_SECTION bufferCS;
	InitializeCriticalSection(&bufferCS);
	buffer->cs = bufferCS;
	LeaveCriticalSection(cs);
}

void destroyBuffer(Buffer * buffer)
{

	EnterCriticalSection(&buffer->cs);
	free(buffer->data);
	buffer->popIdx = -1;
	buffer->pushIdx = -1;
	buffer->count = -1;
	buffer->size = -1;
	LeaveCriticalSection(&buffer->cs);
	DeleteCriticalSection(&buffer->cs);
	buffer = NULL;
}

void expand(Buffer * buffer)
{
	// TODO 1: proveriti da li je bafer prazan

	char* newData;
	int newSize;

	newSize = buffer->size * 2;
	newData = (char *)malloc(sizeof(char)*newSize + 1);
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

	free(buffer->data);  //MEMORY LEAK
	buffer->data = newData;
	buffer->size = newSize;
	buffer->pushIdx = buffer->count;
	buffer->popIdx = 0;

}

void shrink(Buffer * buffer)
{
	EnterCriticalSection(&buffer->cs);
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
		newData = (char *)malloc(sizeof(char) * newSize+1);      // allocate 50 ints
		memset(newData, 0, newSize);

		if (buffer->pushIdx < buffer->popIdx) {
			int rest = buffer->size - buffer->popIdx;
			memcpy(newData, buffer->data + buffer->popIdx, rest);
			memcpy(newData + rest, buffer->data, buffer->pushIdx);
		}
		else {
			memcpy(newData, buffer->data + buffer->popIdx, buffer->count);
		}
		// prepisi podatke
		free(buffer->data);
		buffer->data = newData;
		buffer->size = newSize;
		buffer->pushIdx = buffer->count;
		buffer->popIdx = 0;

		LeaveCriticalSection(&buffer->cs);
	}

}

int push(Buffer *buffer, char * data)
{

	EnterCriticalSection(&buffer->cs);

	int sizeOfData = dataSize(data);

	// ako je bafer vec pun count == size, radi prosirivanje, ali prvo utvrdi za koliko puta
	// ili ako je velicina podataka veca od velicine ostatka slobodnog prostora u baferu
	if ((buffer->count == buffer->size) || ((buffer->size - buffer->count) < sizeOfData)) {
		if (sizeOfData > buffer->size * 2) {
			for (int i = 0; i < sizeOfData / buffer->size + 1; i++)
				expand(buffer);
		}
		else {
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

	LeaveCriticalSection(&buffer->cs);
	return 0;
}

int pop(Buffer * buffer, char * data)
{
	EnterCriticalSection(&buffer->cs);

	int velicina = dataSize(buffer->data);

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
		memcpy(data, buffer->data + buffer->popIdx, rest);
		memcpy(data, buffer->data, velicina - rest);
		memset(buffer->data + buffer->popIdx, 0, rest);
		memset(buffer->data, 0, velicina - rest);
		buffer->popIdx = velicina - rest;
	}

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

	LeaveCriticalSection(&buffer->cs);
	return 0;
}