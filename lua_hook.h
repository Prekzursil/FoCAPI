#pragma once
#include <cstdint>

struct LuaRVAs {
    uintptr_t lua_open;
    uintptr_t lua_pushstring;
    uintptr_t lua_pushcclosure;
    uintptr_t lua_settable;
    uintptr_t lua_tostring;
    uintptr_t lua_tonumber;
    uintptr_t get_player_by_index;     // PlayerListClass::Get_Player_By_Index
    uintptr_t player_list;             // PlayerList global object (data, not a function)
    uintptr_t player_wrapper_create;   // PlayerWrapper::Create (static)
    uintptr_t get_script_from_state;   // LuaScriptClass::Get_Script_From_State (static)
    uintptr_t map_var_to_lua;          // LuaScriptClass::Map_Var_To_Lua (member)
    uintptr_t debug_print;             // Debug_Print(char*)
    uintptr_t lua_newtable;            // lua_newtable(L)
    uintptr_t lua_rawseti;             // lua_rawseti(L, idx, n)
};

// StarWarsI.exe -- debug kit, PDB available
constexpr LuaRVAs RVAs_StarWarsI = {
    0x12AB790, // lua_open
    0x12AC930, // lua_pushstring
    0x12AC6E0, // lua_pushcclosure
    0x12AD020, // lua_settable
    0x12AD430, // lua_tostring
    0x12AD300, // lua_tonumber
    0x06FA990, // PlayerListClass::Get_Player_By_Index
    0x1913158, // PlayerList global object
    0x0F12010, // PlayerWrapper::Create
    0x0537BB0, // LuaScriptClass::Get_Script_From_State
    0x0536FE0, // LuaScriptClass::Map_Var_To_Lua
    0x0476AE0, // Debug_Print
    0x12AC460, // lua_newtable
    0x12ACD70, // lua_rawseti
};

// StarWarsG.exe -- client binary, no PDB
constexpr LuaRVAs RVAs_StarWarsG = {
    0x07b8930, // lua_open
    0x07b9540, // lua_pushstring
    0x07b9340, // lua_pushcclosure
    0x07b9a60, // lua_settable
    0x07b9cc0, // lua_tostring
    0,         // lua_tonumber (TODO)
    0,         // PlayerListClass::Get_Player_By_Index (TODO)
    0,         // PlayerList global object (TODO)
    0,         // PlayerWrapper::Create (TODO)
    0,         // LuaScriptClass::Get_Script_From_State (TODO)
    0,         // LuaScriptClass::Map_Var_To_Lua (TODO)
    0,         // Debug_Print (TODO)
    0,         // lua_newtable (TODO)
    0,         // lua_rawseti (TODO)
};

constexpr int LUA_GLOBALSINDEX = -10001;

bool LuaHook_Init();
void LuaHook_Shutdown();