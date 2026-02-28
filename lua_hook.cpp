#include "lua_hook.h"
#include "minhook/include/MinHook.h"
#include <windows.h>

#if defined _M_X64
#pragma comment(lib, "MinHook.x64.lib")
#elif defined _M_IX86
#pragma comment(lib, "MinHook.x86.lib")
#endif

// Opaque Lua state - never dereferenced, just passed around
struct lua_State;
typedef int (*lua_CFunction)(lua_State*);

// Function pointer types
typedef lua_State* (*fn_lua_open)();
typedef void (*fn_lua_pushstring)(lua_State*, const char*);
typedef void (*fn_lua_pushcclosure)(lua_State*, lua_CFunction, int);
typedef void (*fn_lua_settable)(lua_State*, int);
typedef const char* (*fn_lua_tostring)(lua_State*, int);

// Resolved function pointers
static fn_lua_open real_lua_open = nullptr;
static fn_lua_pushstring pfn_pushstring = nullptr;
static fn_lua_pushcclosure pfn_pushcclosure = nullptr;
static fn_lua_settable pfn_settable = nullptr;
static fn_lua_tostring pfn_tostring = nullptr;

// Helper, equivalent of lua_setglobal
static void register_global(lua_State* L, const char* name, lua_CFunction fn) {
	pfn_pushstring(L, name);
	pfn_pushcclosure(L, fn, 0);
	pfn_settable(L, LUA_GLOBALSINDEX);
}

// =========================
// Custom Lua file functions
// =========================

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

// =========================
// Hooks
// =========================

// Fired every time the game creates a new Lua state
static lua_State* Hook_lua_open() {
	lua_State* L = real_lua_open();

	if (!L) {
		return L;
	}

	// Register custom Lua functions
	register_global(L, "WriteToFile", L_WriteToFile);

	return L;
}

bool LuaHook_Init() {
	char exePath[MAX_PATH];
	GetModuleFileNameA(nullptr, exePath, MAX_PATH);

	const LuaRVAs* rvas = nullptr;
	if (strstr(exePath, "StarWarsI.exe"))
		rvas = &RVAs_StarWarsI;
	else if (strstr(exePath, "StarWarsG.exe"))
		rvas = &RVAs_StarWarsG;
	else
		return false;

	if (!rvas->lua_open || !rvas->lua_pushstring || !rvas->lua_pushcclosure ||
		!rvas->lua_settable || !rvas->lua_tostring)
		return false;

	uintptr_t base = (uintptr_t)GetModuleHandleA(nullptr);

	pfn_pushstring = (fn_lua_pushstring)(base + rvas->lua_pushstring);
	pfn_pushcclosure = (fn_lua_pushcclosure)(base + rvas->lua_pushcclosure);
	pfn_settable = (fn_lua_settable)(base + rvas->lua_settable);
	pfn_tostring = (fn_lua_tostring)(base + rvas->lua_tostring);

	void* const target = (void*)(base + rvas->lua_open);

	if (MH_Initialize() != MH_OK) {
		return false;
	}

	if (MH_CreateHook(target, &Hook_lua_open, (void**)&real_lua_open) != MH_OK) {
		return false;
	}

	if (MH_EnableHook(target) != MH_OK) {
		return false;
	}

	return true;
}

void LuaHook_Shutdown() {
	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}