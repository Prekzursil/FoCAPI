#pragma once
#include <cstdint>

constexpr uintptr_t RVA_lua_open = 0x12AB790;
constexpr uintptr_t RVA_lua_pushstring = 0x12AC930;
constexpr uintptr_t RVA_lua_pushcclosure = 0x12AC6E0;
constexpr uintptr_t RVA_lua_settable = 0x12AD020;
constexpr uintptr_t RVA_lua_tostring = 0x12AD430;

constexpr int LUA_GLOBALSINDEX = -10001;

bool LuaHook_Init();
void LuaHook_Shutdown();