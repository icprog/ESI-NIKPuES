#include "stdafx.h"
#include "ThreadArray.h"


void expandThreads(ThreadArray * threadArray)
{
	HANDLE *newArray;
	newArray = (HANDLE *)malloc(sizeof(HANDLE) * threadArray->size * 2);
	threadArray->size *= 2;
	memcpy(newArray, threadArray->threads, threadArray->size);
	free(threadArray->threads);
	threadArray->threads = newArray;
}

void addThread(ThreadArray * threadArray, HANDLE *thread)
{

	EnterCriticalSection(&threadArray->cs);
	if (threadArray->count == threadArray->size)
		expandThreads(threadArray);

	for (int i = 0; i < threadArray->size; i++) {
		if (threadArray->threads[i] == NULL) {
			threadArray->threads[i] = *thread;
			break;
		}
	}

	threadArray->count++;
	LeaveCriticalSection(&threadArray->cs);

}

void removeThread(ThreadArray * threadArray, HANDLE *thread)
{

	EnterCriticalSection(&threadArray->cs);
	//smanji count i prolazi kroz sve elemente niza i obrisi bafer sa yadatim imenom
	threadArray->count--;

	for (int i = 0; i < threadArray->size; i++) {
		if (threadArray->threads[i] == *thread) {
			CloseHandle(threadArray->threads[i]);
			threadArray->threads[i] = NULL;
			break;
		}
	}
	LeaveCriticalSection(&threadArray->cs);

}

void clearThreads(ThreadArray * threadArray)
{

	EnterCriticalSection(&threadArray->cs);

	for (int i = 0; i < threadArray->size; i++) {
		CloseHandle(threadArray->threads[i]);
		//WSACleanup();
		threadArray->threads[i] = NULL;
		break;
	}
	free(threadArray->threads);
	LeaveCriticalSection(&threadArray->cs);
	DeleteCriticalSection(&threadArray->cs);

}

void initializeThreads(ThreadArray * threadArray, int size, CRITICAL_SECTION *cs)
{
	EnterCriticalSection(cs);
	threadArray->count = 0;
	threadArray->size = size;
	threadArray->threads = (HANDLE *)malloc(sizeof(HANDLE) * size);
	//memset(threadArray->threads, NULL, size);

	for (int i = 0; i < threadArray->size; i++) {
		threadArray->threads[i] = NULL;
	}
	CRITICAL_SECTION queueCS;
	InitializeCriticalSection(&queueCS);
	threadArray->cs = queueCS;
	LeaveCriticalSection(cs);

}
