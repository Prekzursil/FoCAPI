#include "functions.h"
#include "../lua_hook.h"
#include <windows.h>
#include <string>

// Add_Tutorial_Text(key, text [, duration [, r, g, b, a]])
// duration defaults to -1 (perpetual). r/g/b/a default to 1.0 (white, opaque).
// The color pointer must be non-null: the game copies 16 bytes from it unconditionally
// before its own null check, so we always pass a valid buffer.
static int L_Add_Tutorial_Text(lua_State* L)
{
    const char* key  = pfn_tostring(L, 1);
    const char* text = pfn_tostring(L, 2);
    if (!key || !text)
        return 0;

    float duration = -1.0f;
    float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    if (pfn_tonumber)
    {
        double arg3 = pfn_tonumber(L, 3);
        if (arg3 != 0.0) duration = (float)arg3;

        double r = pfn_tonumber(L, 4);
        double g = pfn_tonumber(L, 5);
        double b = pfn_tonumber(L, 6);
        double a = pfn_tonumber(L, 7);
        if (r != 0.0 || g != 0.0 || b != 0.0 || a != 0.0)
        {
            color[0] = (float)r;
            color[1] = (float)g;
            color[2] = (float)b;
            color[3] = (float)a;
        }
    }

    int wlen = MultiByteToWideChar(CP_ACP, 0, text, -1, nullptr, 0);
    std::wstring wtext(wlen - 1, L'\0');
    MultiByteToWideChar(CP_ACP, 0, text, -1, &wtext[0], wlen);

    pfn_add_tutorial_text(g_command_bar, &wtext, (char*)key, duration, 0, false, color);

    return 0;
}

// Remove_Tutorial_Text(key)
static int L_Remove_Tutorial_Text(lua_State* L)
{
    const char* key = pfn_tostring(L, 1);
    if (!key)
        return 0;

    pfn_remove_tutorial_text(g_command_bar, (char*)key);

    return 0;
}

void Register_ScreenText(lua_State* L)
{
    if (!pfn_add_tutorial_text || !pfn_remove_tutorial_text || !g_command_bar)
        return;

    register_global(L, "Add_Tutorial_Text",    L_Add_Tutorial_Text);
    register_global(L, "Remove_Tutorial_Text", L_Remove_Tutorial_Text);
}
