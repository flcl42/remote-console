#pragma once

#include <Windows.h>



class RegistryHelper
{
private:
	RegistryHelper();
	~RegistryHelper();
	static BOOL IsMyProgramRegisteredForStartup(PCWSTR pszAppName);
	static BOOL RegisterMyProgramForStartup(PCWSTR pszAppName, PCWSTR pathToExe, PCWSTR args);	
public:
	static void RegisterProgram();
	static utility::string_t GetValue(utility::string_t& key);
	static utility::string_t GetOsVersion();
	static void SetValue(utility::string_t& key, utility::string_t& value);
	static void Clean();
};

