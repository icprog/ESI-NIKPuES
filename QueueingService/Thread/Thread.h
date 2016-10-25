#pragma once
#include "stdafx.h"

/*
	Struktura za parametre ClientServerThread.
*/
typedef struct csParams {
	ThreadArray *threadArray = NULL;
	SocketArray *socketArray = NULL;
	Queue *queue = NULL;
	SOCKET *serviceSocket = NULL;
	int odgovor = -1;
}CSParams;
/*
Struktura za parametre PopFromService.
*/
typedef struct ppsParamas {
	SocketArray *socketArray = NULL;
	Queue *queue = NULL;
	SOCKET *serviceSocket = NULL;
	int type = -1;                            //0 za klijente, 1 za servere
}PPSParams;
typedef struct ssParams {
	SOCKET *serviceSocket = NULL;
	int odgovor = -1;
}SSParams;

/*
Struktura za parametre PushInService
*/
typedef struct pushParams {
	MySocket *socket;
	Queue *queue;
	CRITICAL_SECTION *cs;
	int initializator; // kome treba poslati odgovor : 0-Server, 1-Klijent
}PushParams;

/*
	Glavna serverska nit. Cilj joj je da omoguci uspostavu veze sa klijentima. 
	Kreira se u okviru main() funkcije servera.
*/
DWORD WINAPI ClientServerThread(LPVOID lpParam);

/*
	Ovo je funkcija za nuti koje kreira glavna nit servera.
	Funkcija ima zadatak da procesira poruku koju klijent posalje na servis i stavi je u odgovarajuci red.
*/
DWORD WINAPI PushInService(LPVOID lpParam);          

/*
	Ovo je funkcija za nit koju kreira glava nit servera, a koja radi proveru na svakih 200ms.
	Funkcija ima zadatak da svakih 200ms proveri da li postoji poruka u nekom redu, i ako postoji, da je skine sa tog reda,
	i posalje odgovarajucem klijentu ili serveru koji se prijavio da ceka poruku sa tog reda. 
	Jedan od parametara je ste da li ovo zovo klijent ili server.
*/
DWORD WINAPI PopFromService(LPVOID lpParam);

/*
	Ova funkcija se koristi u okviru niti kreirane iz main() funkcije servera. 
	Funkcija ima zadatak da na svakih 200ms vrsi proveru aktivnih niti, te ukoliko pronadje one koje vise nisu aktivne, a kreirane su,
	zatvori na odgovarajuci nacin i time spreci curenje resursa.
*/
DWORD WINAPI GarbageCollector(LPVOID lpParam);

