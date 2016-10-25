// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define _CRT_SECURE_NO_WARNINGS


// TODO: reference additional headers your program requires here
#include "../Buffer/Buffer.h";
#include "../Queue/Queue.h";
#include "../SocketNB/SocketNB.h";
#include "../SocketArray/SocketArray.h";
#include "../ThreadArray/ThreadArray.h";
#include <ws2tcpip.h>