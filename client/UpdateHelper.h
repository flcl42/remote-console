#pragma once

using namespace utility;

class UpdateHelper
{
public:
	UpdateHelper();
	~UpdateHelper();
	static void Restart(string_t exe);
	static void Remove(string_t exe);
	static void RemoveAndStart(string_t exe, string_t tmp, string_t newName);
};

