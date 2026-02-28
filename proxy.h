#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

extern HMODULE g_hRealPowrprof;
bool Proxy_Init();
void Proxy_Shutdown();