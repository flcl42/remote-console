#include "stdafx.h"
#include "url.h"
#include <string>
#include <algorithm>
#include <cctype>
#include <functional>
using namespace std;

// ctors, copy, equality, ...
url::url(const wstring& url_s)
{
	parse(url_s);
}

void url::parse(const wstring& url_s)
{
	const wstring prot_end(L"://");
	wstring::const_iterator prot_i = search(url_s.begin(), url_s.end(), prot_end.begin(), prot_end.end());
	protocol_.reserve(distance(url_s.begin(), prot_i));
	transform(url_s.begin(), prot_i, back_inserter(protocol_), ptr_fun<int, int>(tolower)); // protocol is icase
	if (prot_i == url_s.end())
		return;
	advance(prot_i, prot_end.length());
	wstring::const_iterator path_i = find(prot_i, url_s.end(), '/');
	host_.reserve(distance(prot_i, path_i));
	std::copy(prot_i, path_i, back_inserter(host_));

	auto found = host_.find_first_of(L'@');
	if (found != std::string::npos)
	{
		auto creds = host_.substr(0, found);
		auto found2 = creds.find_first_of(L':');
		if (found2 != std::string::npos)
		{
			user_ = creds.substr(0, found2);
			passw_ = creds.substr(found2+1);
		}
		host_ = host_.substr(found+1);
	}
	 // host is icase
	wstring::const_iterator query_i = find(path_i, url_s.end(), '?');
	path_.assign(path_i, query_i);
	if (query_i != url_s.end())
		++query_i;
	query_.assign(query_i, url_s.end());
}