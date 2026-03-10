#include "functions.h"
#include "../lua_hook.h"
#include <windows.h>
#include <string>

// Maximum number of lines held in the feed at once.
static constexpr int MAX_LINES = 8;

// Keys used as the RawText identifier for each slot, enabling Remove_Tutorial_Text lookups.
static constexpr float PERPETUAL = -1.0f;

static char  g_keys[MAX_LINES][16]; // "FOCA_0" .. "FOCA_7"
static int   g_head  = 0;           // index of oldest active slot
static int   g_count = 0;           // number of active slots

static void reset_feed()
{
    g_head  = 0;
    g_count = 0;
    for (int i = 0; i < MAX_LINES; i++)
        wsprintfA(g_keys[i], "FOCA_%d", i);
}

// Print_Screen_Text(text [, duration])
// Appends a line to the bottom of the tutorial text feed.
// duration defaults to -1 (perpetual). Pass a positive number of seconds to auto-expire.
static int L_Print_Screen_Text(lua_State* L)
{
    const char* text = pfn_tostring(L, 1);
    if (!text || !pfn_add_tutorial_text || !g_command_bar)
        return 0;

    float duration = PERPETUAL;
    if (pfn_tonumber)
    {
        double arg2 = pfn_tonumber(L, 2);
        if (arg2 != 0.0)
            duration = (float)arg2;
    }

    // Widen to wchar_t for the game's basic_string<wchar_t> parameter.
    int wlen = MultiByteToWideChar(CP_ACP, 0, text, -1, nullptr, 0);
    std::wstring wtext(wlen - 1, L'\0');
    MultiByteToWideChar(CP_ACP, 0, text, -1, &wtext[0], wlen);

    // If the feed is full, evict the oldest entry.
    if (g_count >= MAX_LINES)
    {
        pfn_remove_tutorial_text(g_command_bar, g_keys[g_head]);
        g_head  = (g_head + 1) % MAX_LINES;
        g_count--;
    }

    int slot = (g_head + g_count) % MAX_LINES;
    g_count++;

    // Inner overload: (this, wstring*, key, duration, index, teletype, color)
    // index=0 so StartTime is set immediately; teletype=false for instant display; color=nullptr for default.
    pfn_add_tutorial_text(g_command_bar, &wtext, g_keys[slot], duration, 0, false, nullptr);

    return 0;
}

// Clear_Screen_Text()
// Removes all active FoCAPI-managed screen text entries.
static int L_Clear_Screen_Text(lua_State* L)
{
    if (!pfn_remove_tutorial_text || !g_command_bar)
        return 0;

    for (int i = 0; i < g_count; i++)
    {
        int slot = (g_head + i) % MAX_LINES;
        pfn_remove_tutorial_text(g_command_bar, g_keys[slot]);
    }

    g_head  = 0;
    g_count = 0;

    return 0;
}

void Register_ScreenText(lua_State* L)
{
    if (!pfn_add_tutorial_text || !pfn_remove_tutorial_text || !g_command_bar)
        return;

    reset_feed();

    register_global(L, "Print_Screen_Text", L_Print_Screen_Text);
    register_global(L, "Clear_Screen_Text", L_Clear_Screen_Text);
}
