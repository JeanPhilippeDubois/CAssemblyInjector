#pragma once
#include <windows.h>
#include <iostream>
#include<fstream>
#include<TlHelp32.h>

using LibrairieLoadeeA = HINSTANCE(WINAPI*) ( const char * librairieNomFichier );
using ObtenirAddresseProc = UINT_PTR(WINAPI*)(HINSTANCE HMODULE, const char* librairieNomProc);
using ENTREE_DU_DLL = BOOL(WINAPI*)(void* pourDll, DWORD dwReason, void* bReserver);

struct MAPPING_MANUEL_DONNE
{
	LibrairieLoadeeA plibrairieloadeeA;
	ObtenirAddresseProc pobteniradresseproc;
	HINSTANCE jpMod;
};

bool MonMappingManuel(HANDLE leProc, const char* poidDLLFile);