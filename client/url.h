#ifndef URL_HH_
#define URL_HH_    
#include <string>
struct url {
	url(const std::wstring& url_s); // omitted copy, ==, accessors, ...
private:
	void parse(const std::wstring& url_s);
public:
	std::wstring protocol_, host_, path_, query_, user_, passw_;
};
#endif /* URL_HH_ */