#include "functions.h"
#include "../lua_hook.h"
#include <windows.h>

// WriteToFile(path, text) - appends text to a file
static int L_WriteToFile(lua_State* L) {
	const char* path = pfn_tostring(L, 1);
	const char* text = pfn_tostring(L, 2);

	if (!path || !text) {
		return 0;
	}

	HANDLE hFile = CreateFileA(
		path,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile != INVALID_HANDLE_VALUE) {
		DWORD written;

		SetFilePointer(hFile, 0, NULL, FILE_END);
		WriteFile(hFile, text, lstrlenA(text), &written, NULL);
		WriteFile(hFile, "\n", 1, &written, NULL);
		CloseHandle(hFile);
	}

	return 0;
}

void Register_FileIO(lua_State* L) {
	register_global(L, "WriteToFile", L_WriteToFile);
}
