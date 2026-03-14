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

    // --- Scripts ---
    uintptr_t get_script_from_state;   // LuaScriptClass::Get_Script_From_State (static)
    uintptr_t map_var_to_lua;          // LuaScriptClass::Map_Var_To_Lua (member)
    uintptr_t register_member;         // LuaUserVar::Register_Member(char*, LuaVar*)
    uintptr_t lua_value_string_vftable; // LuaValue<basic_string<>>::vftable
    size_t    lua_value_string_size;   // allocation size of LuaValue<basic_string<>>
    size_t    lua_value_string_offset; // offset of basic_string member within LuaValue
    uintptr_t return_variable;         // LuaUserVar::Return_Variable(LuaVar*)

    // --- TheGameText ---
	uintptr_t the_game_text;		  // TheGameText global object (data, not a function)
	uintptr_t get_string;			  // GameTextClass::Get(char*, bool)

    // --- Players ---
    uintptr_t get_player_by_index;     // PlayerListClass::Get_Player_By_Index
    uintptr_t player_list;             // PlayerList global object (data, not a function)
    uintptr_t player_wrapper_create;   // PlayerWrapper::Create (static)

    // --- Screen Text ---
    uintptr_t command_bar;             // TheCommandBar global object (data, not a function)
    uintptr_t add_tutorial_text;       // CommandBarClass::Add_Tutorial_Text (inner overload: wstring*, char*, float, uint, bool, RGBAClass*)
    uintptr_t remove_tutorial_text;    // CommandBarClass::Remove_Tutorial_Text(char*)

    // --- Game Object Type Wrapper ---
    uintptr_t got_wrapper_ctor;        // GameObjectTypeWrapper::GameObjectTypeWrapper()
    size_t    got_text_name_id_offset;  // offset of TextNameID (std::string) within GameObjectTypeClass
    // LuaMemberFunctionWrapper<GameObjectTypeWrapper> layout (ctor always inlined)
    uintptr_t lmfw_vftable;            // LuaMemberFunctionWrapper<>::vftable
    uintptr_t lua_uservar_ctor;        // LuaUserVar::LuaUserVar(int, bool)
    size_t    lmfw_alloc_size;         // allocation size of LuaMemberFunctionWrapper<>
    size_t    lmfw_pmf_offset;         // offset of MemberFunction (16-byte PMF)
    size_t    lmfw_object_offset;      // offset of Object (wrapper pointer)
    size_t    lmfw_usemaps_offset;     // offset of UseMaps (bool)

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

    // --- Scripts ---
    0x0537BB0, // LuaScriptClass::Get_Script_From_State
    0x0536FE0, // LuaScriptClass::Map_Var_To_Lua
    0x0046f51, // LuaUserVar::Register_Member
    0x1453310, // LuaValue<basic_string<>>::vftable
    0x78,      // LuaValue<basic_string<>> allocation size
    0x58,      // basic_string member offset within LuaValue
    0x0058887, // LuaUserVar::Return_Variable

    // --- TheGameText ---
    0x18578b0, // TheGameText global object
    0x002b4cc, // GameTextClass::Get

    // --- Players ---
    0x000b0e1, // PlayerListClass::Get_Player_By_Index
    0x1913158, // PlayerList global object
    0x0F12010, // PlayerWrapper::Create

    // --- Screen Text ---
    0x1916b80, // TheCommandBar global object
    0x003c83a, // CommandBarClass::Add_Tutorial_Text inner
    0x006e1eb, // CommandBarClass::Remove_Tutorial_Text

    // --- Game Object Type Wrapper ---
    0x004f6c9, // GameObjectTypeWrapper::GameObjectTypeWrapper()
    0x160,     // GameObjectTypeClass::TextNameID field offset
    0x158a680, // LuaMemberFunctionWrapper<>::vftable
    0x0074f55, // LuaUserVar::LuaUserVar
    0x88,      // LuaMemberFunctionWrapper<> allocation size
    0x68,      // MemberFunction offset
    0x78,      // Object offset
    0x80,      // UseMaps offset

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
    0x07b9140, // lua_newtable
    0x07b9820, // lua_rawseti

    // --- Scripts ---
    0x0245790, // LuaScriptClass::Get_Script_From_State
    0x0247700, // LuaScriptClass::Map_Var_To_Lua
    0x024be40, // LuaUserVar::Register_Member
    0x0855a98, // LuaValue<basic_string<>>::vftable
    0x30,     // LuaValue<basic_string<>> allocation size
    0x10,     // basic_string member offset within LuaValue
    0x0256d40, // LuaUserVar::Return_Variable

    // --- TheGameText ---
    0x0a7bc58, // TheGameText global object
    0x01fa680, // GameTextClass::Get

    // --- Players ---
    0x0294bc0, // PlayerListClass::Get_Player_By_Index
    0x0a16fd0, // PlayerList global object
    0x06019f0, // PlayerWrapper::Create

    // --- Screen Text ---
    0x0b27f60, // TheCommandBar global object
    0x02dfd10, // CommandBarClass::Add_Tutorial_Text inner
    0x02ff290, // CommandBarClass::Remove_Tutorial_Text

    // --- Game Object Type Wrapper ---
    0x0604a10, // GameObjectTypeWrapper::GameObjectTypeWrapper()
    0x120,     // GameObjectTypeClass::TextNameID field offset
    0x08ab770, // LuaMemberFunctionWrapper<>::vftable
    0x0249c20, // LuaUserVar::LuaUserVar
    0x48,      // LuaMemberFunctionWrapper<> allocation size
    0x28,      // MemberFunction offset
    0x38,      // Object offset
    0x40,      // UseMaps offset

    // --- Debug ---
    0,         // Debug_Print (not mapped)
};
