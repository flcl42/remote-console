#pragma once
#include "main.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>

#define BUFSIZE 1024 * 5


using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams
											//using namespace web::http::experimental::listener;          // HTTP server
											//using namespace web::experimental::web_sockets::client;     // WebSockets client
using namespace web::json;                  // JSON library
using namespace std;


class ConsoleHelper
{
public:
	ConsoleHelper(http_client *client);
	~ConsoleHelper();
public:
	void Start();
	void Exit();
	void Enter(string_t command);
private:
	HANDLE hReadPipe1, hWritePipe1, hReadPipe2, hWritePipe2;
	
	// thread for reading
	HANDLE hReadThread;
	size_t skipChars = 0;
	// cmd process	
	HANDLE hProcess;
	HANDLE hProcessThread;
	//http_client *client;
	void writeToWeb(string_t buff);
	http_client *client;
	friend DWORD WINAPI readFromPipe(LPVOID helper);
};

