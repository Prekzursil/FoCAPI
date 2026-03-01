#include "functions.h"
#include "../lua_hook.h"
#include <windows.h>

// Offsets for reading PlayersByID count directly from the PlayerList object.
// PlayersByID (DynamicVectorClass) is at +0x18 within PlayerListClass.
// The active count field is at +0x10 within DynamicVectorClass.
static constexpr int OFFSET_PLAYERS_BY_ID = 0x18;
static constexpr int OFFSET_VECTOR_COUNT  = 0x10;

// Get_All_Players() - returns a 1-based Lua table of PlayerWrappers for all players,
// iterable with ipairs(). Identical wrapper type to Find_Player().
static int L_Get_All_Players(lua_State* L) {
	int count = *(int*)((char*)g_player_list + OFFSET_PLAYERS_BY_ID + OFFSET_VECTOR_COUNT);

	if (pfn_debug_print) {
		char msg[64];
		wsprintfA(msg, "[FoCAPI] Get_All_Players: PlayersByID count = %d\n", count);
		pfn_debug_print(msg);
	}

	if (count <= 0 || count > 64) {
		if (pfn_debug_print) pfn_debug_print("[FoCAPI] Get_All_Players: count out of expected range, aborting\n");
		pfn_newtable(L);
		return 1;
	}

	if (pfn_debug_print) pfn_debug_print("[FoCAPI] Get_All_Players: calling Get_Script_From_State\n");
	void* script = pfn_get_script_from_state(L);
	if (pfn_debug_print) {
		char msg[64];
		wsprintfA(msg, "[FoCAPI] Get_All_Players: script = %p\n", script);
		pfn_debug_print(msg);
	}

	if (pfn_debug_print) pfn_debug_print("[FoCAPI] Get_All_Players: calling lua_newtable\n");
	pfn_newtable(L);
	if (pfn_debug_print) pfn_debug_print("[FoCAPI] Get_All_Players: lua_newtable OK\n");

	for (int i = 0; i < count; i++) {
		if (pfn_debug_print) {
			char msg[64];
			wsprintfA(msg, "[FoCAPI] Get_All_Players: Get_Player_By_Index(%d)\n", i);
			pfn_debug_print(msg);
		}
		void* player = pfn_get_player_by_index(g_player_list, i);
		if (!player) continue;

		if (pfn_debug_print) pfn_debug_print("[FoCAPI] Get_All_Players: PlayerWrapper::Create\n");
		void* wrapper = pfn_player_wrapper_create(player, script);
		if (pfn_debug_print) {
			char msg[128];
			wsprintfA(msg, "[FoCAPI] Get_All_Players: wrapper = %p, player = %p\n", wrapper, player);
			pfn_debug_print(msg);
		}
		if (!wrapper) continue;

		if (pfn_debug_print) pfn_debug_print("[FoCAPI] Get_All_Players: Map_Var_To_Lua\n");
		pfn_map_var_to_lua(L, wrapper);

		if (pfn_debug_print) pfn_debug_print("[FoCAPI] Get_All_Players: lua_rawseti\n");
		pfn_rawseti(L, -2, i + 1);
	}

	return 1;
}

void Register_Players(lua_State* L) {
	if (!pfn_map_var_to_lua || !pfn_newtable || !pfn_rawseti) return;
	register_global(L, "Get_All_Players", L_Get_All_Players);
}
