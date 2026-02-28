#include "proxy.h"
#include "lua_hook.h"
#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved) {
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstDll);

		if (!Proxy_Init()) {
			return FALSE;
		}

		if (!LuaHook_Init()) {
			OutputDebugStringA("[SWEAW Hooks] WARNING: LuaHook_Init() failed - Lua hooks are not active.\n");
		}
		break;

	case DLL_PROCESS_DETACH:
		LuaHook_Shutdown();
		Proxy_Shutdown();
		break;
	}

	return TRUE;
}
