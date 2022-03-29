#include "Mirko.h"

void __stdcall MonPtiShell(MAPPING_MANUEL_DONNE * jpData);

bool MonMappingManuel(HANDLE leProc, const char* poidDLLFile)
{
	BYTE* donneeProcesseurData = nullptr;
	IMAGE_NT_HEADERS* lesHeadersduNT = nullptr;
	IMAGE_OPTIONAL_HEADER* leHeaderOptionelduProc = nullptr;
	IMAGE_FILE_HEADER* leVieuxFileHeader = nullptr;
	BYTE* leprocTargetBase;


	if (!GetFileAttributes(poidDLLFile))
	{
		printf("Aucun fichier mec");
		return false;
	}

	std::ifstream File(poidDLLFile, std::ios::binary | std::ios::ate);

	if (File.fail())
	{
		printf("erreur ouverture du fichier molo", (DWORD)File.rdstate());
		File.close();
		return false;
	}
	auto lePoidDeLaFichier = File.tellg();

	if (lePoidDeLaFichier < 0x1000)
	{
		printf("poidfichierInvalid");
		File.close();
		return false;
	}

	donneeProcesseurData = new BYTE[static_cast<UINT_PTR>(lePoidDeLaFichier)];
	if (!donneeProcesseurData)
	{
		printf("Memory allocating failed");
		File.close();
		return false;
	}

	File.seekg(0, std::ios::beg);
	File.read(reinterpret_cast<char*>(donneeProcesseurData), lePoidDeLaFichier);

	if (reinterpret_cast<IMAGE_DOS_HEADER*>(donneeProcesseurData)->e_magic != 0x5A4D)
	{
		printf("inalid fiile");
		delete[] donneeProcesseurData;
		return false;
	}
	lesHeadersduNT = reinterpret_cast<IMAGE_NT_HEADERS*>(donneeProcesseurData + reinterpret_cast<IMAGE_DOS_HEADER*>(donneeProcesseurData)->e_lfanew);
	leHeaderOptionelduProc = &lesHeadersduNT->OptionalHeader;
	leVieuxFileHeader = &lesHeadersduNT->FileHeader;

#ifdef _WIN64
	if (leVieuxFileHeader->Machine != IMAGE_FILE_MACHINE_AMD64)
	{
		printf("Invalid platform");
		delete[] donneeProcesseurData;
		return false;
}
#else
		if (leVieuxFileHeader->Machine != IMAGE_FILE_MACHINE_I386)
		{
			printf("Invalid platform");
			delete[] donneeProcesseurData;
			return false;
		}
#endif

	leprocTargetBase = reinterpret_cast<BYTE*>(VirtualAllocEx(leProc, reinterpret_cast<void*>(leHeaderOptionelduProc->ImageBase), leHeaderOptionelduProc->SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
	if (!leprocTargetBase)
	{
		leprocTargetBase = reinterpret_cast<BYTE*>(VirtualAllocEx(leProc, nullptr, leHeaderOptionelduProc->SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
		if (!leprocTargetBase)
		{
			printf("memory allocation failed", GetLastError());
			delete[] donneeProcesseurData;
			return false;
		}

	}

	MAPPING_MANUEL_DONNE madata { 0 };
	madata.plibrairieloadeeA = LibrairieLoadeeA();
	madata.pobteniradresseproc = reinterpret_cast<ObtenirAddresseProc>(ObtenirAddresseProc());

	auto* leHeaderDlaSection = IMAGE_FIRST_SECTION(lesHeadersduNT);
	for (UINT i = 0; i != leVieuxFileHeader->NumberOfSections; i++, ++leHeaderDlaSection)
	{
		if(leHeaderDlaSection->SizeOfRawData)
			if (!WriteProcessMemory(leProc, leprocTargetBase + leHeaderDlaSection->VirtualAddress, donneeProcesseurData + leHeaderDlaSection->PointerToRawData, leHeaderDlaSection->SizeOfRawData, nullptr))
			{
				printf("Can't map la petite section");
				delete[] donneeProcesseurData;
				VirtualFreeEx(leProc, leprocTargetBase,0, MEM_RELEASE);
				return false;
			}
	}
	memcpy(donneeProcesseurData, &madata, sizeof(madata));
	WriteProcessMemory(leProc, leprocTargetBase, donneeProcesseurData, 0x1000, nullptr);


	delete[] donneeProcesseurData;


	void * cbvcxvxvcb = VirtualAllocEx(leProc, nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!cbvcxvxvcb)
	{
		printf("jemapvbc");
		VirtualFreeEx(leProc, leprocTargetBase,0, MEM_RELEASE);
		return false;
	}

	WriteProcessMemory(leProc, cbvcxvxvcb, MonPtiShell, 0x1000, nullptr);

	HANDLE bcxbvxcv = (CreateRemoteThread(leProc, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(MonPtiShell), leprocTargetBase, 0, nullptr));

	if (!bcxbvxcv)
	{
		printf("fail");
		VirtualFreeEx(leProc, leprocTargetBase, 0, MEM_RELEASE);
		VirtualFreeEx(leProc, cbvcxvxvcb, 0, MEM_RELEASE);
		return false;
	}
	CloseHandle(bcxbvxcv);

	HINSTANCE bvcbcvbcv = NULL;
	while (!bvcbcvbcv)
	{
		MAPPING_MANUEL_DONNE xvbcxvcbc{ 0 };
		ReadProcessMemory(leProc, leprocTargetBase, &xvbcxvcbc, sizeof(xvbcxvcbc), nullptr);
		bvcbcvbcv = xvbcxvcbc.jpMod;
		Sleep(100);
	}
	VirtualFreeEx(leProc, cbvcxvxvcb, 0, MEM_RELEASE);

	return true;
}

#define RELOC_FLAG32(RelInfo) ((RelInfo >>0x0C) == IMAGE_REL_BASED_HIGHLOW)
#define RELOC_FLAG64(RelInfo) ((RelInfo >>0x0C) == IMAGE_REL_BASED_DIR64)
#ifdef _WIN64
#define RELOC_FLAG RELOC_FLAG64
#else
#define RELOC_FLAG RELOC_FLAG32
#endif

void __stdcall MonPtiShell(MAPPING_MANUEL_DONNE* jpData)
{
	if (!jpData)
		return;

	BYTE* jpBase = reinterpret_cast<BYTE*>(jpData);
	auto* jpOpt= &reinterpret_cast<IMAGE_NT_HEADERS*>(jpBase + reinterpret_cast<IMAGE_DOS_HEADER*>(jpData)->e_lfanew)->OptionalHeader;


	auto _JeLoadLibA = jpData->plibrairieloadeeA;
	auto _JePrendProcAdress = jpData->pobteniradresseproc;
	auto _leDllMainPs = reinterpret_cast<ENTREE_DU_DLL>(jpBase + jpOpt->AddressOfEntryPoint);

	BYTE * JPLocationDelta = jpBase - jpOpt->ImageBase;
	if (JPLocationDelta)
	{
		if (jpOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress)
			return;

		auto* jpDonneRelocation = reinterpret_cast<IMAGE_BASE_RELOCATION*>(jpBase + jpOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);

		while (jpDonneRelocation->VirtualAddress)
		{
			UINT MontantEntree = (jpDonneRelocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
			WORD* jpInfosRelatives = reinterpret_cast<WORD*>(jpDonneRelocation + 1);
			for (UINT i = 0; i != MontantEntree; ++i, ++jpInfosRelatives)
			{
				if (RELOC_FLAG(*jpInfosRelatives))
				{
					UINT_PTR* jpMonPatch = reinterpret_cast<UINT_PTR*>(jpBase + jpDonneRelocation->VirtualAddress + ((*jpInfosRelatives) & 0xFFF));
					*jpMonPatch += reinterpret_cast<UINT_PTR>(JPLocationDelta);
				}
			}
			jpDonneRelocation = reinterpret_cast<IMAGE_BASE_RELOCATION*>(reinterpret_cast<BYTE*>(jpDonneRelocation) + jpDonneRelocation->SizeOfBlock);
		}
	}
	if (jpOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size)
	{
		auto* jplaDescriptiondeLimport = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(jpBase + jpOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
		while (jplaDescriptiondeLimport->Name)
		{
			char* lePoidDuModule = reinterpret_cast<char*>(jpBase + jplaDescriptiondeLimport->Name);
			HINSTANCE bvxcxvcvxcxcvxcv = _JeLoadLibA(lePoidDuModule);
			ULONG_PTR* vccvbcvbxvcbcvxbvc = reinterpret_cast<ULONG_PTR*>(jpBase + jplaDescriptiondeLimport->OriginalFirstThunk);
			ULONG_PTR* vbcvcvxbcvbxcvbxcvbcvxbvxcbcv = reinterpret_cast<ULONG_PTR*>(jpBase + jplaDescriptiondeLimport->FirstThunk);
			if (!vccvbcvbxvcbcvxbvc)
				vccvbcvbxvcbcvxbvc = vbcvcvxbcvbxcvbxcvbcvxbvxcbcv;
			for (; *vccvbcvbxvcbcvxbvc; ++vccvbcvbxvcbcvxbvc, vbcvcvxbcvbxcvbxcvbcvxbvxcbcv++)
			{
				if (IMAGE_SNAP_BY_ORDINAL(*vccvbcvbxvcbcvxbvc))
				{
					*vbcvcvxbcvbxcvbxcvbcvxbvxcbcv = _JePrendProcAdress(bvxcxvcvxcxcvxcv, reinterpret_cast<char*>(*vccvbcvbxvcbcvxbvc & 0xFFFF));
				}
				else
				{
					auto* gbvcnvbxcnbbvcn = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(jpBase + (*vccvbcvbxvcbcvxbvc));
					*vbcvcvxbcvbxcvbxcvbcvxbvxcbcv = _JePrendProcAdress(bvxcxvcvxcxcvxcv, gbvcnvbxcnbbvcn->Name);
				}
			}
			jplaDescriptiondeLimport++;
		}
	}

	if (jpOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size)
	{
		auto * cvbnvcncvx = reinterpret_cast<IMAGE_TLS_DIRECTORY*>(jpBase + jpOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);
		auto* bcvbcvbc = reinterpret_cast<PIMAGE_TLS_CALLBACK*>(cvbnvcncvx->AddressOfCallBacks);
		for (; bcvbcvbc && *bcvbcvbc; bcvbcvbc++)
			(*bcvbcvbc)(jpBase, DLL_PROCESS_ATTACH, nullptr);

	}
	_leDllMainPs(jpBase, DLL_PROCESS_ATTACH, nullptr);
	//test
	jpData->jpMod = reinterpret_cast<HINSTANCE>(jpBase);


}
