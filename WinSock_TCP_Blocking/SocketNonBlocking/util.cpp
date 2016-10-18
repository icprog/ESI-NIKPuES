#include "util.h"
#include <stdlib.h>
#include <string.h>

int dataSize(char * data) {
	//int dataSize=0;
	return *((int*)data);
	//return dataSize;
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
		memcpy(data, buffer->data + velicina - 1, velicina); //kopiraj data na pocetak buffer-a
		buffer->popIdx += velicina;
	}
	else {
	//iz dva dela, prvo kopiraj kraj data na pocetak, pa onda sa pocetka pomeri
		int rest = buffer->size - buffer->popIdx;
		memcpy(data,buffer->data+buffer->popIdx,rest);
		memcpy(data, buffer->data,velicina-rest);
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

}

void expand(Buffer * buffer )
{
	char* newData;
	int newSize;
	//int velicina = dataSize(buffer->data);
	
		newSize = buffer->size * 2; 
		newData = (char *)malloc(sizeof(char)*newSize);

	if (buffer->pushIdx < buffer->popIdx) { // ako je push manji od pop indeksa, onda je data iz dva dela pa radimo 2 mem kopija
		int rest = buffer->size - buffer->popIdx; 
		memcpy(newData, buffer->data + buffer->popIdx, rest); //kopiraj kraj starog bufera u novi 
		memcpy(newData+rest,buffer->data,buffer->pushIdx); //nastavi na novi data onaj data sa pocetka starog
	}
	else {
		memcpy(newData, buffer->data+buffer->popIdx, buffer->count);
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
