#pragma once
#include "stdafx.h"
#include "util.h"

void add(Buffer *queue, Buffer *buffer);          //queue je niz, a buffer je pokazivac na buffer
void remove(Buffer *queue, Buffer *buffer);
void clear(Buffer *queue);
void expand(Buffer *queue);