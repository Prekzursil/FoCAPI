#include "lua_hook.h"
#include "functions/functions.h"
#include <MinHook.h>
#include <windows.h>
#include <cstring>
#include <new>

// Cached during init for CreateLuaValueString
static const LuaRVAs* g_rvas = nullptr;
static uintptr_t      g_base = 0;

// Resolved function pointers
fn_lua_open            real_lua_open          = nullptr;
fn_lua_pushstring      pfn_pushstring         = nullptr;
fn_lua_pushcclosure    pfn_pushcclosure       = nullptr;
fn_lua_settable        pfn_settable           = nullptr;
fn_lua_tostring        pfn_tostring           = nullptr;
fn_lua_tonumber        pfn_tonumber           = nullptr;
fn_lua_newtable        pfn_newtable           = nullptr;
fn_lua_rawseti         pfn_rawseti            = nullptr;
fn_GetPlayerByIndex    pfn_get_player_by_index   = nullptr;
void*                  g_player_list             = nullptr;
fn_PlayerWrapperCreate pfn_player_wrapper_create = nullptr;
fn_GetScriptFromState  pfn_get_script_from_state = nullptr;
fn_MapVarToLua         pfn_map_var_to_lua        = nullptr;
fn_AddTutorialText     pfn_add_tutorial_text     = nullptr;
fn_RemoveTutorialText  pfn_remove_tutorial_text  = nullptr;
void*                  g_command_bar             = nullptr;
fn_DebugPrint          pfn_debug_print           = nullptr;
fn_RegisterMember      pfn_register_member       = nullptr;
fn_ReturnVariable      pfn_return_variable       = nullptr;
fn_GotWrapperCtor      real_got_wrapper_ctor     = nullptr;
fn_LmfwCtor            pfn_lmfw_ctor             = nullptr;
fn_GetTextNameId       pfn_get_text_name_id      = nullptr;
fn_GameTextGet         pfn_game_text_get         = nullptr;
void*                  g_the_game_text           = nullptr;

void register_global(lua_State* L, const char* name, lua_CFunction fn) {
	pfn_pushstring(L, name);
	pfn_pushcclosure(L, fn, 0);
	pfn_settable(L, LUA_GLOBALSINDEX);
}

void* CreateLuaValueString(const std::string& str) {
	if (!g_rvas || !g_rvas->lua_value_string_vftable)
		return nullptr;

	void* obj = ::operator new(g_rvas->lua_value_string_size);
	if (!obj)
		return nullptr;
	memset(obj, 0, g_rvas->lua_value_string_size);

	// Set the vftable pointer
	*(uintptr_t*)obj = g_base + g_rvas->lua_value_string_vftable;

	// Placement-new the string into the member offset
	new ((char*)obj + g_rvas->lua_value_string_offset) std::string(str);

	return obj;
}

// Fired every time the game constructs a GameObjectTypeWrapper
static void* Hook_GOTWrapperCtor(void* this_wrapper) {
	void* result = real_got_wrapper_ctor(this_wrapper);
	Register_GOTMembers(this_wrapper);
	return result;
}

// Fired every time the game creates a new Lua state
static lua_State* Hook_lua_open() {
	lua_State* L = real_lua_open();

	if (!L) {
		return L;
	}

	Register_All(L);

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
	g_rvas = rvas;
	g_base = base;

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

	if (rvas->command_bar && rvas->add_tutorial_text && rvas->remove_tutorial_text) {
		g_command_bar            = (void*)(base + rvas->command_bar);
		pfn_add_tutorial_text    = (fn_AddTutorialText)(base + rvas->add_tutorial_text);
		pfn_remove_tutorial_text = (fn_RemoveTutorialText)(base + rvas->remove_tutorial_text);
	}

	if (rvas->debug_print)
		pfn_debug_print = (fn_DebugPrint)(base + rvas->debug_print);

	if (rvas->register_member)
		pfn_register_member = (fn_RegisterMember)(base + rvas->register_member);
	if (rvas->return_variable)
		pfn_return_variable = (fn_ReturnVariable)(base + rvas->return_variable);

	if (rvas->the_game_text)
		g_the_game_text = (void*)(base + rvas->the_game_text);
	if (rvas->get_string)
		pfn_game_text_get = (fn_GameTextGet)(base + rvas->get_string);
	if (rvas->got_get_text_name_id)
		pfn_get_text_name_id = (fn_GetTextNameId)(base + rvas->got_get_text_name_id);
	if (rvas->got_lmfw_ctor)
		pfn_lmfw_ctor = (fn_LmfwCtor)(base + rvas->got_lmfw_ctor);

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

	// Hook GameObjectTypeWrapper constructor to register custom member functions
	if (rvas->got_wrapper_ctor && rvas->got_lmfw_ctor &&
		pfn_register_member && pfn_lmfw_ctor &&
		pfn_get_text_name_id && pfn_game_text_get && g_the_game_text &&
		rvas->lua_value_string_vftable && pfn_return_variable)
	{
		void* got_ctor_target = (void*)(base + rvas->got_wrapper_ctor);
		if (MH_CreateHook(got_ctor_target, &Hook_GOTWrapperCtor, (void**)&real_got_wrapper_ctor) == MH_OK) {
			MH_EnableHook(got_ctor_target);
		}
	}

	return true;
}

void LuaHook_Shutdown() {
	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}
