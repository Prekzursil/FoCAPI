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
typedef lua_State*   (*fn_lua_open)();
typedef void         (*fn_lua_pushstring)(lua_State*, const char*);
typedef void         (*fn_lua_pushcclosure)(lua_State*, lua_CFunction, int);
typedef void         (*fn_lua_settable)(lua_State*, int);
typedef const char*  (*fn_lua_tostring)(lua_State*, int);
typedef double       (*fn_lua_tonumber)(lua_State*, int);
typedef void*        (*fn_GetPlayerByIndex)(void*, int);			// PlayerListClass::Get_Player_By_Index
typedef void*        (*fn_PlayerWrapperCreate)(void*, void*);		// PlayerWrapper::Create(PlayerClass*, LuaScriptClass*)
typedef void*        (*fn_GetScriptFromState)(lua_State*);			// LuaScriptClass::Get_Script_From_State
typedef void         (*fn_MapVarToLua)(lua_State*, void*);			// LuaScriptClass::Map_Var_To_Lua (static)
typedef void         (*fn_DebugPrint)(const char*);					// Debug_Print
typedef void         (*fn_lua_newtable)(lua_State*);				// lua_newtable(L)
typedef void         (*fn_lua_rawseti)(lua_State*, int, int);		// lua_rawseti(L, idx, n)

// Resolved function pointers
static fn_lua_open          real_lua_open = nullptr;
static fn_lua_pushstring    pfn_pushstring = nullptr;
static fn_lua_pushcclosure  pfn_pushcclosure = nullptr;
static fn_lua_settable      pfn_settable = nullptr;
static fn_lua_tostring      pfn_tostring = nullptr;
static fn_lua_tonumber      pfn_tonumber = nullptr;
static fn_GetPlayerByIndex  pfn_get_player_by_index = nullptr;
static void*                g_player_list = nullptr;
static fn_PlayerWrapperCreate pfn_player_wrapper_create = nullptr;
static fn_GetScriptFromState  pfn_get_script_from_state = nullptr;
static fn_MapVarToLua         pfn_map_var_to_lua = nullptr;
static fn_DebugPrint          pfn_debug_print = nullptr;
static fn_lua_newtable        pfn_newtable = nullptr;
static fn_lua_rawseti         pfn_rawseti  = nullptr;

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

// Find_Internet_Player(index) - returns a PlayerWrapper for the given internet player index,
// identical in structure to the object returned by Find_Player().
static int L_Find_Internet_Player(lua_State* L) {
	int index = (int)pfn_tonumber(L, 1);
	if (pfn_debug_print) pfn_debug_print("[SWEAW Hooks] Find_Internet_Player called\n");

	if (index < 0) {
		if (pfn_debug_print) pfn_debug_print("[SWEAW Hooks] Find_Internet_Player: index must be >= 0\n");
		return 0;
	}

	void* player = pfn_get_player_by_index(g_player_list, index);
	if (!player) {
		if (pfn_debug_print) pfn_debug_print("[SWEAW Hooks] Find_Internet_Player: Get_Player_By_Index returned null\n");
		return 0;
	}

	void* script = pfn_get_script_from_state(L);
	void* wrapper = pfn_player_wrapper_create(player, script);
	if (!wrapper) {
		if (pfn_debug_print) pfn_debug_print("[SWEAW Hooks] Find_Internet_Player: PlayerWrapper::Create returned null\n");
		return 0;
	}

	pfn_map_var_to_lua(L, wrapper);
	return 1;
}

// Offsets for reading PlayersByID count directly from the PlayerList object.
// PlayersByID (DynamicVectorClass) is at +0x18 within PlayerListClass.
// The active count field is at +0x10 within DynamicVectorClass.
static constexpr int OFFSET_PLAYERS_BY_ID  = 0x18;
static constexpr int OFFSET_VECTOR_COUNT   = 0x10;

// Get_All_Players() - returns a 1-based Lua table of PlayerWrappers for all players,
// iterable with ipairs(). Identical wrapper type to Find_Player().
static int L_Get_All_Players(lua_State* L) {
	int count = *(int*)((char*)g_player_list + OFFSET_PLAYERS_BY_ID + OFFSET_VECTOR_COUNT);

	if (pfn_debug_print) {
		char msg[64];
		wsprintfA(msg, "[SWEAW Hooks] Get_All_Players: PlayersByID count = %d\n", count);
		pfn_debug_print(msg);
	}

	if (count <= 0 || count > 64) {
		if (pfn_debug_print) pfn_debug_print("[SWEAW Hooks] Get_All_Players: count out of expected range, aborting\n");
		pfn_newtable(L);
		return 1;
	}

	if (pfn_debug_print) pfn_debug_print("[SWEAW Hooks] Get_All_Players: calling Get_Script_From_State\n");
	void* script = pfn_get_script_from_state(L);
	if (pfn_debug_print) {
		char msg[64];
		wsprintfA(msg, "[SWEAW Hooks] Get_All_Players: script = %p\n", script);
		pfn_debug_print(msg);
	}

	if (pfn_debug_print) pfn_debug_print("[SWEAW Hooks] Get_All_Players: calling lua_newtable\n");
	pfn_newtable(L);
	if (pfn_debug_print) pfn_debug_print("[SWEAW Hooks] Get_All_Players: lua_newtable OK\n");

	for (int i = 0; i < count; i++) {
		if (pfn_debug_print) {
			char msg[64];
			wsprintfA(msg, "[SWEAW Hooks] Get_All_Players: Get_Player_By_Index(%d)\n", i);
			pfn_debug_print(msg);
		}
		void* player = pfn_get_player_by_index(g_player_list, i);
		if (!player) continue;

		if (pfn_debug_print) pfn_debug_print("[SWEAW Hooks] Get_All_Players: PlayerWrapper::Create\n");
		void* wrapper = pfn_player_wrapper_create(player, script);
		if (pfn_debug_print) {
			char msg[128];
			wsprintfA(msg, "[SWEAW Hooks] Get_All_Players: wrapper = %p, player = %p\n", wrapper, player);
			pfn_debug_print(msg);
		}
		if (!wrapper) continue;

		if (pfn_debug_print) pfn_debug_print("[SWEAW Hooks] Get_All_Players: Map_Var_To_Lua\n");
		pfn_map_var_to_lua(L, wrapper);

		if (pfn_debug_print) pfn_debug_print("[SWEAW Hooks] Get_All_Players: lua_rawseti\n");
		pfn_rawseti(L, -2, i + 1);
	}

	return 1;
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
	if (pfn_debug_print) pfn_debug_print("[SWEAW Hooks] Registered WriteToFile Lua function\n");

	if (pfn_map_var_to_lua) {
		register_global(L, "Find_Internet_Player", L_Find_Internet_Player);
		if (pfn_debug_print) pfn_debug_print("[SWEAW Hooks] Registered Find_Internet_Player Lua function\n");

		if (pfn_newtable && pfn_rawseti) {
			register_global(L, "Get_All_Players", L_Get_All_Players);
			if (pfn_debug_print) pfn_debug_print("[SWEAW Hooks] Registered Get_All_Players Lua function\n");
		}
	}

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

	pfn_pushstring   = (fn_lua_pushstring)(base + rvas->lua_pushstring);
	pfn_pushcclosure = (fn_lua_pushcclosure)(base + rvas->lua_pushcclosure);
	pfn_settable     = (fn_lua_settable)(base + rvas->lua_settable);
	pfn_tostring     = (fn_lua_tostring)(base + rvas->lua_tostring);

	if (rvas->lua_tonumber && rvas->get_player_by_index && rvas->player_list &&
		rvas->player_wrapper_create && rvas->get_script_from_state && rvas->map_var_to_lua)
	{
		pfn_tonumber              = (fn_lua_tonumber)(base + rvas->lua_tonumber);
		pfn_get_player_by_index   = (fn_GetPlayerByIndex)(base + rvas->get_player_by_index);
		g_player_list             = (void*)(base + rvas->player_list);
		pfn_player_wrapper_create = (fn_PlayerWrapperCreate)(base + rvas->player_wrapper_create);
		pfn_get_script_from_state = (fn_GetScriptFromState)(base + rvas->get_script_from_state);
		pfn_map_var_to_lua        = (fn_MapVarToLua)(base + rvas->map_var_to_lua);
	}

	if (rvas->debug_print)
		pfn_debug_print = (fn_DebugPrint)(base + rvas->debug_print);

	if (rvas->lua_newtable && rvas->lua_rawseti) {
		pfn_newtable = (fn_lua_newtable)(base + rvas->lua_newtable);
		pfn_rawseti  = (fn_lua_rawseti)(base + rvas->lua_rawseti);
	}

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