#include "proxy.h"

HMODULE g_hRealPowrprof = nullptr;

// NTSTATUS = LONG, POWER_INFORMATION_LEVEL = int; using primitive types
// avoids pulling in powerbase.h / ntstatus.h for a simple passthrough.
typedef LONG (WINAPI* fn_CallNtPowerInformation)(int, PVOID, ULONG, PVOID, ULONG);
typedef DWORD (WINAPI* fn_PowerReadACValueIndex)(HKEY, const GUID*, const GUID*, const GUID*, LPDWORD);
typedef DWORD (WINAPI* fn_PowerGetActiveScheme)(HKEY, GUID**);
typedef DWORD (WINAPI* fn_PowerSettingRegisterNotification)(LPCGUID, DWORD, HANDLE, HANDLE*);
typedef DWORD (WINAPI* fn_PowerSettingUnregisterNotification)(HANDLE);

static fn_CallNtPowerInformation          real_CallNtPowerInformation          = nullptr;
static fn_PowerReadACValueIndex           real_PowerReadACValueIndex           = nullptr;
static fn_PowerReadACValueIndex           real_PowerReadDCValueIndex           = nullptr;
static fn_PowerGetActiveScheme            real_PowerGetActiveScheme            = nullptr;
static fn_PowerSettingRegisterNotification   real_PowerSettingRegisterNotification   = nullptr;
static fn_PowerSettingUnregisterNotification real_PowerSettingUnregisterNotification = nullptr;

bool Proxy_Init() {
	char path[MAX_PATH];

	GetSystemDirectoryA(path, MAX_PATH);
	lstrcatA(path, "\\powrprof.dll");
	g_hRealPowrprof = LoadLibraryA(path);

	if (!g_hRealPowrprof) {
		return false;
	}

	real_CallNtPowerInformation = (fn_CallNtPowerInformation)
		GetProcAddress(g_hRealPowrprof, "CallNtPowerInformation");
	real_PowerReadACValueIndex = (fn_PowerReadACValueIndex)
		GetProcAddress(g_hRealPowrprof, "PowerReadACValueIndex");
	real_PowerReadDCValueIndex = (fn_PowerReadACValueIndex)
		GetProcAddress(g_hRealPowrprof, "PowerReadDCValueIndex");
	real_PowerGetActiveScheme = (fn_PowerGetActiveScheme)
		GetProcAddress(g_hRealPowrprof, "PowerGetActiveScheme");
	real_PowerSettingRegisterNotification = (fn_PowerSettingRegisterNotification)
		GetProcAddress(g_hRealPowrprof, "PowerSettingRegisterNotification");
	real_PowerSettingUnregisterNotification = (fn_PowerSettingUnregisterNotification)
		GetProcAddress(g_hRealPowrprof, "PowerSettingUnregisterNotification");

	return real_CallNtPowerInformation != nullptr;
}

void Proxy_Shutdown() {
	if (g_hRealPowrprof) {
		FreeLibrary(g_hRealPowrprof);
		g_hRealPowrprof = nullptr;
	}
}

extern "C" {
	__declspec(dllexport) LONG WINAPI CallNtPowerInformation(
		int level, PVOID inBuf, ULONG inSize, PVOID outBuf, ULONG outSize)
	{
		return real_CallNtPowerInformation(level, inBuf, inSize, outBuf, outSize);
	}

	__declspec(dllexport) DWORD WINAPI PowerReadACValueIndex(
		HKEY rootKey, const GUID* scheme, const GUID* subgroup, const GUID* setting, LPDWORD index)
	{
		if (real_PowerReadACValueIndex)
			return real_PowerReadACValueIndex(rootKey, scheme, subgroup, setting, index);
		return ERROR_NOT_SUPPORTED;
	}

	__declspec(dllexport) DWORD WINAPI PowerReadDCValueIndex(
		HKEY rootKey, const GUID* scheme, const GUID* subgroup, const GUID* setting, LPDWORD index)
	{
		if (real_PowerReadDCValueIndex)
			return real_PowerReadDCValueIndex(rootKey, scheme, subgroup, setting, index);
		return ERROR_NOT_SUPPORTED;
	}

	__declspec(dllexport) DWORD WINAPI PowerGetActiveScheme(
		HKEY userRootPowerKey, GUID** activePolicyGuid)
	{
		if (real_PowerGetActiveScheme)
			return real_PowerGetActiveScheme(userRootPowerKey, activePolicyGuid);
		return ERROR_NOT_SUPPORTED;
	}

	__declspec(dllexport) DWORD WINAPI PowerSettingRegisterNotification(
		LPCGUID settingGuid, DWORD flags, HANDLE recipient, HANDLE* registrationHandle)
	{
		if (real_PowerSettingRegisterNotification)
			return real_PowerSettingRegisterNotification(settingGuid, flags, recipient, registrationHandle);
		return ERROR_NOT_SUPPORTED;
	}

	__declspec(dllexport) DWORD WINAPI PowerSettingUnregisterNotification(
		HANDLE registrationHandle)
	{
		if (real_PowerSettingUnregisterNotification)
			return real_PowerSettingUnregisterNotification(registrationHandle);
		return ERROR_NOT_SUPPORTED;
	}
}
