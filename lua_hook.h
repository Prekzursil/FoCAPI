#pragma once
#include "rvas.h"
#include <string>

struct lua_State;
typedef int (*lua_CFunction)(lua_State*);

// Function pointer types
typedef lua_State*   (*fn_lua_open)();
typedef void         (*fn_lua_pushstring)(lua_State*, const char*);
typedef void         (*fn_lua_pushcclosure)(lua_State*, lua_CFunction, int);
typedef void         (*fn_lua_settable)(lua_State*, int);
typedef const char*  (*fn_lua_tostring)(lua_State*, int);
typedef double       (*fn_lua_tonumber)(lua_State*, int);
typedef void         (*fn_lua_newtable)(lua_State*);
typedef void         (*fn_lua_rawseti)(lua_State*, int, int);
typedef void*        (*fn_GetPlayerByIndex)(void*, int);          // PlayerListClass::Get_Player_By_Index
typedef void*        (*fn_PlayerWrapperCreate)(void*, void*);     // PlayerWrapper::Create(PlayerClass*, LuaScriptClass*)
typedef void*        (*fn_GetScriptFromState)(lua_State*);        // LuaScriptClass::Get_Script_From_State
typedef void         (*fn_MapVarToLua)(lua_State*, void*);        // LuaScriptClass::Map_Var_To_Lua (static)
typedef void         (*fn_AddTutorialText)(void*, void*, const char*, float, unsigned int, bool, void*); // CommandBarClass::Add_Tutorial_Text (inner)
typedef void         (*fn_RemoveTutorialText)(void*, const char*);                                      // CommandBarClass::Remove_Tutorial_Text
typedef void         (*fn_DebugPrint)(const char*);               // Debug_Print
// Scripting internals (LuaUserVar) -- usable by any wrapper module
typedef void         (*fn_RegisterMember)(void* uservar, const char* name, void* fn_ptr); // LuaUserVar::Register_Member
typedef void*        (*fn_ReturnVariable)(void* uservar, void* lua_var);                  // LuaUserVar::Return_Variable(LuaVar*) -> LuaTable*
// Game Object Type Wrapper internals
typedef void*        (*fn_GotWrapperCtor)(void* this_wrapper);                            // GameObjectTypeWrapper::GameObjectTypeWrapper()
typedef void*        (*fn_LmfwCtor)(void* lmfw, void* wrapper, void* pmf, bool use_maps); // LuaMemberFunctionWrapper<GameObjectTypeWrapper>::ctor
typedef void*        (*fn_GetTextNameId)(void* got_class, void* out_string);              // GameObjectTypeClass::Get_Text_Name_ID() -- returns via hidden out-param
typedef void*        (*fn_GameTextGet)(void* game_text, void* text_id_str, bool insert);  // GameTextClass::Get(basic_string*, bool) -- returns wstring*

// Resolved function pointers (defined in lua_hook.cpp)
extern fn_lua_open            real_lua_open;
extern fn_lua_pushstring      pfn_pushstring;
extern fn_lua_pushcclosure    pfn_pushcclosure;
extern fn_lua_settable        pfn_settable;
extern fn_lua_tostring        pfn_tostring;
extern fn_lua_tonumber        pfn_tonumber;
extern fn_lua_newtable        pfn_newtable;
extern fn_lua_rawseti         pfn_rawseti;
extern fn_GetPlayerByIndex    pfn_get_player_by_index;
extern void*                  g_player_list;
extern fn_PlayerWrapperCreate pfn_player_wrapper_create;
extern fn_GetScriptFromState  pfn_get_script_from_state;
extern fn_MapVarToLua         pfn_map_var_to_lua;
extern fn_AddTutorialText     pfn_add_tutorial_text;
extern fn_RemoveTutorialText  pfn_remove_tutorial_text;
extern void*                  g_command_bar;
extern fn_DebugPrint          pfn_debug_print;
extern fn_RegisterMember      pfn_register_member;
extern fn_ReturnVariable      pfn_return_variable;
extern fn_GotWrapperCtor      real_got_wrapper_ctor;
extern fn_LmfwCtor            pfn_lmfw_ctor;
extern fn_GetTextNameId       pfn_get_text_name_id;
extern fn_GameTextGet         pfn_game_text_get;
extern void*                  g_the_game_text;

constexpr int LUA_GLOBALSINDEX = -10001;

// Equivalent of lua_setglobal — registers a C function in the Lua global table
void register_global(lua_State* L, const char* name, lua_CFunction fn);

// Creates a LuaValue<basic_string<>> from a std::string.
// Reproduces the inlined constructor pattern using per-executable layout constants.
void* CreateLuaValueString(const std::string& str);

bool LuaHook_Init();
void LuaHook_Shutdown();
