#include "sigscanner.h"
#include "error.h"

SignatureScanner::SignatureScanner() {
	procID = 0;
	mod = { {} };
	hProc = NULL;
}

SignatureScanner::~SignatureScanner() {
	CloseHandle(hProc);
}

char* SignatureScanner::PatternScan(char* pattern, char* mask, char* begin, size_t size) {
	size_t pl = strlen(mask); // pattern length
	bool matching = true;
	size_t offset = 0;

	for (offset = 0; offset < size - pl; offset++) { // move start of signature
		matching = true;
		for (size_t j = 0; j < pl; j++) { // loop for pattern
			if (mask[j] != '?' && pattern[j] != *(begin + offset + j)) {
				matching = false;
				break;
			}
		}
		if (matching) {
			break;
		}
	}
	
	return (matching ? (begin + offset) : nullptr);
}

// useful stuff:
// https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualqueryex
// https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualprotectex
// https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-memory_basic_information
// Thank you GH
char* SignatureScanner::PatternScanEx(char* pattern, char* mask, char* begin, char* end, HANDLE hProc) {
	char* memLoc = begin;
	char* sigAddr = nullptr;
	DWORD lpflOldProtect = NULL;
	SIZE_T bytesRead = NULL;
	char* memBuffer = nullptr;
	MEMORY_BASIC_INFORMATION lpBuffer = { {} };

	while (VirtualQueryEx(hProc, memLoc, &lpBuffer, sizeof(lpBuffer)) && memLoc < end) {
		if (lpBuffer.State != MEM_COMMIT || lpBuffer.Protect == PAGE_NOACCESS) {
			memLoc += lpBuffer.RegionSize;
			continue;
		}

		memBuffer = new char[lpBuffer.RegionSize];

		if (VirtualProtectEx(hProc, lpBuffer.BaseAddress, lpBuffer.RegionSize, PAGE_EXECUTE_READWRITE, &lpflOldProtect)) {
			ReadProcessMemory(hProc, lpBuffer.BaseAddress, memBuffer, lpBuffer.RegionSize, &bytesRead);
			VirtualProtectEx(hProc, lpBuffer.BaseAddress, lpBuffer.RegionSize, lpflOldProtect, &lpflOldProtect);

			char* memPageAddr = PatternScan(pattern, mask, memBuffer, bytesRead);

			// add memPageAddr to memLoc as offset for location of signature in memory
			if (memPageAddr != nullptr) {
				sigAddr = memLoc + (uintptr_t)(memPageAddr - memBuffer);
				break;
			}
		}

		delete[] memBuffer;

		memLoc += lpBuffer.RegionSize;
	}

	delete[] memBuffer; // delete after break;

#ifdef DEBUG
	std::cout << "Found Address at: 0x" << std::hex << (int)sigAddr << std::dec << std::endl;
#endif // !DEBUG

	return sigAddr;
}

char* SignatureScanner::FindPattern(const char* pattern, const char* mask, char* begin, char* end) {
	return PatternScanEx(const_cast<char*>(pattern), const_cast<char*>(mask), begin, end, hProc);;
}

const DWORD SignatureScanner::GetProcId(const wchar_t* procName, long accessRights) {
	HANDLE handleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (handleSnap != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 entry = { {} };
		entry.dwSize = sizeof PROCESSENTRY32;

		if (Process32First(handleSnap, &entry)) {
			do {
				if (_wcsicmp(entry.szExeFile, procName) == 0) {
					procID = entry.th32ProcessID;
					hProc = OpenProcess(PROCESS_ALL_ACCESS, NULL, procID);
					break;
				}
			} while (Process32Next(handleSnap, &entry));
		}
	}

	CloseHandle(handleSnap);

	if (!hProc) {
		std::wstring process(procName);
		std::string cProc(process.begin(), process.end());
		throw Error(1, std::string("Unable to find [" + cProc + "] process."));
	}

	return procID;
}

const Module SignatureScanner::GetModule(const wchar_t* modName) {
	if (procID) {
		HANDLE handleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, procID);

		if (handleSnap != INVALID_HANDLE_VALUE) {
			MODULEENTRY32 entry = { {} };
			entry.dwSize = sizeof MODULEENTRY32;

			if (Module32First(handleSnap, &entry)) {
				do {
					if (_wcsicmp(entry.szModule, modName) == 0) {
						mod = { (char*)entry.modBaseAddr, entry.modBaseSize };
						break;
					}
				} while (Module32Next(handleSnap, &entry));
			}
		}

		CloseHandle(handleSnap);

		if (!mod.dwBase) {
			std::wstring module(modName);
			std::string cModule(module.begin(), module.end());
			throw Error(2, std::string("Unable to find [" + cModule + "] module."));
		}

	}
	else {
		throw Error(3, std::string("No active process."));
	}

	return mod;
}
