#pragma once

using namespace utility;

class FtpHelper
{
public:
	FtpHelper();
	~FtpHelper();
	static void GetFile(string_t ftpAddr, string_t fileName);
	static void UploadFile(string_t ftpAddr, string_t fileName);
};

