#include "functions.h"
#include "../lua_hook.h"
#include <windows.h>
#include <string>
#include <cstring>

// Offset of SmartPtr<GameObjectTypeClass> within GameObjectTypeWrapper.
// SmartPtr stores the raw pointer as its first member.
static constexpr int OFFSET_OBJECT = 0x70;

// MSVC std::wstring layout (x64, MSVC 14.0+).
// Used to read the wstring* returned by GameTextClass::Get.
struct MSVCWString {
    union {
        wchar_t buf[8];    // SSO buffer (up to 7 wchars + null)
        wchar_t* ptr;      // heap pointer when capacity > 7
    };
    size_t size;
    size_t capacity;       // <= 7 means SSO, > 7 means heap

    const wchar_t* c_str() const { return (capacity <= 7) ? buf : ptr; }
};

// Custom member function registered on GameObjectTypeWrapper.
// Signature matches LuaTable* __thiscall (LuaScriptClass*, LuaTable*).
// On x64 this is the standard calling convention: this_wrapper in RCX, script in RDX, params in R8.
static void* Lua_Get_Display_Name(void* this_wrapper, void* /*script*/, void* /*params*/)
{
    // 1. Get GameObjectTypeClass* from SmartPtr at offset 0x70
    void* got_class = *(void**)((char*)this_wrapper + OFFSET_OBJECT);
    if (!got_class)
        return nullptr;

    // 2. Get the text ID string (e.g. "TEXT_UNIT_X_WING") via Get_Text_Name_ID.
    //    The game's function constructs a std::string into our buffer via copy-ctor.
    //    Assumes shared CRT (both DLL and game link to vcruntime140.dll dynamically).
    std::string text_id;
    pfn_get_text_name_id(got_class, &text_id);

    // 3. Resolve the localized display name via TheGameText.Get(text_id, false).
    //    Returns a pointer to an internal wstring (the translated text).
    MSVCWString* display_wstr = (MSVCWString*)pfn_game_text_get(g_the_game_text, &text_id, false);
    if (!display_wstr)
        return nullptr;

    // 4. Convert the wide display name to a narrow (UTF-8) string for Lua.
    const wchar_t* wdata = display_wstr->c_str();
    int wlen = (int)display_wstr->size;

    int narrow_len = WideCharToMultiByte(CP_UTF8, 0, wdata, wlen, nullptr, 0, nullptr, nullptr);
    if (narrow_len <= 0)
        return nullptr;

    std::string narrow(narrow_len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wdata, wlen, &narrow[0], narrow_len, nullptr, nullptr);

    // 5. Create a LuaString (LuaValue<string>, 0x78 bytes) and return it.
    void* lua_value = ::operator new(0x78);
    if (!lua_value)
        return nullptr;
    memset(lua_value, 0, 0x78);
    pfn_lua_value_ctor(lua_value, &narrow);

    return pfn_return_variable(this_wrapper, lua_value);
}

void Register_GOTMembers(void* wrapper)
{
    if (!pfn_lmfw_ctor || !pfn_register_member)
        return;
    if (!pfn_get_text_name_id || !pfn_game_text_get || !g_the_game_text)
        return;
    if (!pfn_lua_value_ctor || !pfn_return_variable)
        return;

    // Allocate a LuaMemberFunctionWrapper<GameObjectTypeWrapper> (0x88 bytes).
    void* lmfw = ::operator new(0x88);
    if (!lmfw) return;
    memset(lmfw, 0, 0x88);

    // Build the 16-byte pointer-to-member-function structure.
    // MSVC x64 multiple-inheritance PMF: [8-byte func addr][8-byte this-adjustment].
    // this-adjustment is 0 (our function expects the full GameObjectTypeWrapper*).
    alignas(8) uint8_t pmf[16] = {};
    *(uintptr_t*)pmf = (uintptr_t)&Lua_Get_Display_Name;

    // Construct the LuaMemberFunctionWrapper via the game's ctor.
    pfn_lmfw_ctor(lmfw, wrapper, pmf, false);

    // Register as a member callable via wrapper.Get_Display_Name()
    pfn_register_member(wrapper, "Get_Display_Name", lmfw);
}
