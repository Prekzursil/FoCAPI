#include "functions.h"
#include "../lua_hook.h"
#include <windows.h>
#include <string>
#include <cstring>

// Offset of SmartPtr<GameObjectTypeClass> within GameObjectTypeWrapper.
// SmartPtr stores the raw pointer as its first member.
static constexpr int OFFSET_OBJECT = 0x70;

// MSVC std::string layout (x64, MSVC 14.0+).
// Used to read the std::string at a known field offset within GameObjectTypeClass.
struct MSVCString {
    union {
        char buf[16];      // SSO buffer (up to 15 chars + null)
        char* ptr;         // heap pointer when capacity > 15
    };
    size_t size;
    size_t capacity;       // <= 15 means SSO, > 15 means heap

    const char* c_str() const { return (capacity <= 15) ? buf : ptr; }
};

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

    // 2. Read the TextNameID string directly from the GameObjectTypeClass field.
    //    Get_Text_Name_ID() is always inlined by the compiler, so we use the field offset.
    MSVCString* text_name = (MSVCString*)((char*)got_class + g_text_name_id_offset);
    std::string text_id(text_name->c_str(), text_name->size);

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

    // 5. Create a LuaString (LuaValue<string>) and return it.
    void* lua_value = CreateLuaValueString(narrow);
    if (!lua_value)
        return nullptr;

    return pfn_return_variable(this_wrapper, lua_value);
}

void Register_GOTMembers(void* wrapper)
{
    if (!pfn_lua_uservar_ctor || !pfn_register_member)
        return;
    if (!g_text_name_id_offset || !pfn_game_text_get || !g_the_game_text)
        return;
    if (!pfn_return_variable || !g_rvas || !g_rvas->lmfw_vftable)
        return;

    const LuaRVAs* rvas = g_rvas;

    // Allocate a LuaMemberFunctionWrapper<GameObjectTypeWrapper>.
    void* lmfw = ::operator new(rvas->lmfw_alloc_size);
    if (!lmfw) return;
    memset(lmfw, 0, rvas->lmfw_alloc_size);

    // Initialize the LuaUserVar base class.
    pfn_lua_uservar_ctor(lmfw, 0, false);

    // Set the LuaMemberFunctionWrapper<> vftable.
    *(uintptr_t*)lmfw = g_base + rvas->lmfw_vftable;

    // Write the 16-byte pointer-to-member-function (PMF) into the MemberFunction field.
    // MSVC x64 multiple-inheritance PMF: [8-byte func addr][8-byte this-adjustment].
    // this-adjustment is 0 (our function expects the full GameObjectTypeWrapper*).
    alignas(8) uint8_t pmf[16] = {};
    *(uintptr_t*)pmf = (uintptr_t)&Lua_Get_Display_Name;
    memcpy((char*)lmfw + rvas->lmfw_pmf_offset, pmf, 16);

    // Set the Object (wrapper pointer) and UseMaps flag.
    *(void**)((char*)lmfw + rvas->lmfw_object_offset) = wrapper;
    *(bool*)((char*)lmfw + rvas->lmfw_usemaps_offset) = false;

    // Register as a member callable via wrapper.Get_Display_Name()
    pfn_register_member(wrapper, "Get_Display_Name", lmfw);
}
