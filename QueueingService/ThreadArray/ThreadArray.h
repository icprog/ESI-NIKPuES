#pragma once
#include "stdafx.h"

typedef struct threadArray {
	HANDLE *threads;
	int count;
	int size;
	CRITICAL_SECTION cs;
}ThreadArray;

void addThread(ThreadArray *threadArray, HANDLE *thread);          //queue je niz, a buffer je pokazivac na buffer
void removeThread(ThreadArray *threadArray, HANDLE *thread);
void clearThreads(ThreadArray *threadArray);
void expandThreads(ThreadArray *threadArray);
void initializeThreads(ThreadArray *threadArray, int size, CRITICAL_SECTION *cs);
