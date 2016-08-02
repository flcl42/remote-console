#include "stdafx.h"
#include "UpdateHelper.h"
#include "Windows.h"
#include <fstream>
#include <Shlwapi.h>

UpdateHelper::UpdateHelper()
{
}


UpdateHelper::~UpdateHelper()
{
}


void UpdateHelper::Restart(string_t exe)
{

}


void UpdateHelper::Remove(string_t exe)
{
	string_t currentDir(exe);
	PathRemoveFileSpec((LPWSTR)&currentDir.c_str()[0]);
	string_t szTempFileName(256, L'\0');
	size_t len = GetTempFileName(&currentDir.c_str()[0], // directory for tmp files
		TEXT("self"),     // temp file name prefix 
		0,                // create unique name 
		(LPWSTR)&szTempFileName.c_str()[0]);  // buffer for name 
		
	szTempFileName.erase(szTempFileName.find_first_of(L'\0'));
	auto szTempFileNameBat = szTempFileName + string_t(L".bat");

	std::wofstream myfile;
	myfile.open(szTempFileNameBat);
	myfile << L":Repeat\r\n";
	myfile << L"del \"" << exe.c_str() << L"\"\r\n";
	myfile << L"if exist \"" << exe.c_str() << L"\" goto Repeat\r\n";
	myfile << L"del \"" << szTempFileName.c_str() << L"\"\r\n";
	myfile << L"del \"" << szTempFileNameBat.c_str() << L"\"\r\n";
	myfile.close();
	ShellExecute(0, L"open", szTempFileNameBat.c_str(), NULL, NULL, SW_HIDE);
}

void UpdateHelper::RemoveAndStart(string_t exe, string_t tmp, string_t newName)
{
	string_t currentDir(exe);
	PathRemoveFileSpec((LPWSTR)&currentDir.c_str()[0]);
	string_t szTempFileName(256, L'\0');
	size_t len = GetTempFileName(&currentDir.c_str()[0], // directory for tmp files
		TEXT("self"),     // temp file name prefix 
		0,                // create unique name 
		(LPWSTR)&szTempFileName.c_str()[0]);  // buffer for name 

	szTempFileName.erase(szTempFileName.find_first_of(L'\0'));
	auto szTempFileNameBat = szTempFileName + string_t(L".bat");

	std::wofstream myfile;
	myfile.open(szTempFileNameBat);
	myfile << L":Repeat\r\n";
	myfile << L"del \"" << exe.c_str() << L"\"\r\n";
	myfile << L"if exist \"" << exe.c_str() << L"\" goto Repeat\r\n";
	myfile << L"del \"" << szTempFileName.c_str() << L"\"\r\n";
	myfile << L"del \"" << szTempFileNameBat.c_str() << L"\"\r\n";
	myfile << L"move \"" << tmp.c_str() << L" " << newName.c_str() << L"\"\r\n";
	myfile << L"start \"" << newName.c_str() << L"\"\r\n";

	myfile.close();
	ShellExecute(0, L"open", szTempFileNameBat.c_str(), NULL, NULL, SW_HIDE);
}