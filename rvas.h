#pragma once
#include <cstdint>

struct LuaRVAs {
    // --- Lua Core ---
    uintptr_t lua_open;
    uintptr_t lua_pushstring;
    uintptr_t lua_pushcclosure;
    uintptr_t lua_settable;
    uintptr_t lua_tostring;
    uintptr_t lua_tonumber;
    uintptr_t lua_newtable;
    uintptr_t lua_rawseti;

    // --- Players ---
    uintptr_t get_player_by_index;     // PlayerListClass::Get_Player_By_Index
    uintptr_t player_list;             // PlayerList global object (data, not a function)
    uintptr_t player_wrapper_create;   // PlayerWrapper::Create (static)

    // --- Scripts ---
    uintptr_t get_script_from_state;   // LuaScriptClass::Get_Script_From_State (static)
    uintptr_t map_var_to_lua;          // LuaScriptClass::Map_Var_To_Lua (member)

    // --- Screen Text ---
    uintptr_t command_bar;             // TheCommandBar global object (data, not a function)
    uintptr_t add_tutorial_text;       // CommandBarClass::Add_Tutorial_Text (inner overload: wstring*, char*, float, uint, bool, RGBAClass*)
    uintptr_t remove_tutorial_text;    // CommandBarClass::Remove_Tutorial_Text(char*)

    // --- Debug ---
    uintptr_t debug_print;             // Debug_Print(char*)
};

// StarWarsI.exe -- debug kit, PDB available
constexpr LuaRVAs RVAs_StarWarsI = {
    // --- Lua Core ---
    0x12AB790, // lua_open
    0x12AC930, // lua_pushstring
    0x12AC6E0, // lua_pushcclosure
    0x12AD020, // lua_settable
    0x12AD430, // lua_tostring
    0x12AD300, // lua_tonumber
    0x12AC460, // lua_newtable
    0x12ACD70, // lua_rawseti

    // --- Players ---
    0x000b0e1, // PlayerListClass::Get_Player_By_Index
    0x1913158, // PlayerList global object
    0x0F12010, // PlayerWrapper::Create

    // --- Scripts ---
    0x0537BB0, // LuaScriptClass::Get_Script_From_State
    0x0536FE0, // LuaScriptClass::Map_Var_To_Lua

    // --- Screen Text ---
    0x1916b80, // TheCommandBar global object
    0x003c83a, // CommandBarClass::Add_Tutorial_Text inner
    0x006e1eb, // CommandBarClass::Remove_Tutorial_Text

    // --- Debug ---
    0x0476AE0, // Debug_Print
};

// StarWarsG.exe -- client binary, no PDB
constexpr LuaRVAs RVAs_StarWarsG = {
    // --- Lua Core ---
    0x07b8930, // lua_open
    0x07b9540, // lua_pushstring
    0x07b9340, // lua_pushcclosure
    0x07b9a60, // lua_settable
    0x07b9cc0, // lua_tostring
    0x07b9bc0, // lua_tonumber
    0x07b9140, // lua_newtable (TODO)
    0x07b9820, // lua_rawseti (TODO)

    // --- Players ---
    0x0294bc0, // PlayerListClass::Get_Player_By_Index (TODO)
    0x0a16fd0, // PlayerList global object (TODO)
    0x06019f0, // PlayerWrapper::Create (TODO)

    // --- Scripts ---
    0x0245790, // LuaScriptClass::Get_Script_From_State (TODO)
    0x0247700, // LuaScriptClass::Map_Var_To_Lua (TODO)

    // --- Screen Text ---
    0,         // TheCommandBar (TODO)
    0,         // CommandBarClass::Add_Tutorial_Text inner (TODO)
    0,         // CommandBarClass::Remove_Tutorial_Text (TODO)

    // --- Debug ---
    0,         // Debug_Print (TODO)
};
