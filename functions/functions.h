#pragma once

struct lua_State;

// Registers all custom Lua functions into the given state.
// Called once per lua_open() from Hook_lua_open().
void Register_All(lua_State* L);

// Per-module registration functions.
// Each checks its own required function pointer guards before registering.
void Register_FileIO(lua_State* L);
void Register_Players(lua_State* L);
void Register_ScreenText(lua_State* L);
