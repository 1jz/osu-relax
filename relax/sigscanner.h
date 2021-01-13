#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

struct Module {
	char* dwBase = nullptr;
	DWORD dwSize = 0;
};

class SignatureScanner {
	char* PatternScan(char* pattern, char* mask, char* begin, size_t size);
	char* PatternScanEx(char* pattern, char* mask, char* begin, char* end, HANDLE hProc);

public:
	HANDLE hProc;
	DWORD procID;
	Module mod;

	SignatureScanner();
	~SignatureScanner();

	char* FindPattern(const char* signature, const char* mask, char* begin = 0x00000000, char* end = (char*)0xFFFFFFFF);
	const DWORD GetProcId(const wchar_t* procName, long accessRights = PROCESS_ALL_ACCESS);
	const Module GetModule(const wchar_t* modName);

	template <typename T>
	T ReadMemory(char* Address) {
		T value = {};
		if (hProc) {
			ReadProcessMemory(hProc, Address, &value, sizeof T, NULL);
		}
		return value;
	}
};
