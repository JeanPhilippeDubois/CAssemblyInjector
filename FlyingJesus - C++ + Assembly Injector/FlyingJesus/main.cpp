#include "Mirko.h"
//aussi le size
const char directionDeMonOuLeDLL[] = "MrJumbo.dll";
//aussi le size
const char maAppQueJeTarket[] = "EscapeFromTarkovBE_.exe";

int main()
{
	PROCESSENTRY32  monPE32{ 0 };
	monPE32.dwSize = sizeof(monPE32);
	HANDLE leSnapshut = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	DWORD lePIDdelapp = 0;
	if (leSnapshut == INVALID_HANDLE_VALUE)
	{
		DWORD derniereErreur = GetLastError();
		printf("CreateTool32 a Fail");
		system("PAUSE");
		return 0;
	}
	
	BOOL leRetour = Process32First(leSnapshut, &monPE32);
	while (leRetour)
	{
		if (!strcmp(maAppQueJeTarket, monPE32.szExeFile))
		{
			lePIDdelapp = monPE32.th32ProcessID;
			break;
		}
		leRetour = Process32Next(leSnapshut, &monPE32);
	}
	CloseHandle(leSnapshut);
	
	HANDLE monProcesss = OpenProcess(PROCESS_ALL_ACCESS, FALSE, lePIDdelapp);
	

		if (!monProcesss)
		{
			DWORD uneErreur = GetLastError();
			printf("Erreur lors du processus de l'ouverture du processus", uneErreur);
			system("PAUSE");
			return 0;
		}

		if (!MonMappingManuel(monProcesss, directionDeMonOuLeDLL))
		{
			CloseHandle(monProcesss);
			printf("Il y a une erreur mon petit gars");
			system("PAUSE");
			return 0;
		}


}