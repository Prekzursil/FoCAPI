#pragma once
#include <cstdint>

struct LuaRVAs {
    uintptr_t lua_open;
    uintptr_t lua_pushstring;
    uintptr_t lua_pushcclosure;
    uintptr_t lua_settable;
    uintptr_t lua_tostring;
};

// StarWarsI.exe -- debug kit, PDB available
constexpr LuaRVAs RVAs_StarWarsI = {
    0x12AB790, // lua_open
    0x12AC930, // lua_pushstring
    0x12AC6E0, // lua_pushcclosure
    0x12AD020, // lua_settable
    0x12AD430, // lua_tostring
};

// StarWarsG.exe -- client binary, no PDB
constexpr LuaRVAs RVAs_StarWarsG = {
    0x07b8930, // lua_open
    0x07b9540, // lua_pushstring
    0x07b9340, // lua_pushcclosure
    0x07b9a60, // lua_settable
    0x07b9cc0, // lua_tostring
};

constexpr int LUA_GLOBALSINDEX = -10001;

bool LuaHook_Init();
void LuaHook_Shutdown();