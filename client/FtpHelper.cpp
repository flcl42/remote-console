#include "stdafx.h"
#include "FtpHelper.h"
#include "Urlmon.h"
#include <wininet.h> 
#include <windows.h>
#include "Shlwapi.h"
#include "url.h"

FtpHelper::FtpHelper()
{
}


FtpHelper::~FtpHelper()
{
}


void FtpHelper::GetFile(string_t ftpAddr, string_t fileName)
{
	url addr(ftpAddr);
	URLDownloadToFile(NULL, &ftpAddr.c_str()[0], (LPCWSTR)&fileName.c_str()[0], 0, NULL);
}

void FtpHelper::UploadFile(string_t ftpAddr, string_t fileName)
{
	url addr(ftpAddr);
	HINTERNET hInternet;
	HINTERNET hFtpSession;
	hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (hInternet == NULL)
	{
		return;
	}
	else
	{		
		auto user = addr.user_.empty() ? string_t(L"Anonymous") : addr.user_;
		auto passw = addr.passw_.empty() ? string_t(L"Anonymous") : addr.passw_;

		hFtpSession = InternetConnect(hInternet, addr.host_.c_str(), INTERNET_DEFAULT_FTP_PORT, user.c_str(), passw.c_str(), INTERNET_SERVICE_FTP, 0, 0);
		if (hFtpSession == NULL)
		{
			return;
		}
		else
		{
			auto filepart = string_t(PathFindFileName(fileName.c_str()));
			if (!FtpPutFile(hFtpSession, fileName.c_str(), addr.path_.c_str(), FTP_TRANSFER_TYPE_BINARY, INTERNET_FLAG_PASSIVE))
			{
				return;
			}
		}
	}

}