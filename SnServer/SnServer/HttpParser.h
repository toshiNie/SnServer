#pragma once
#include"HttpRequst.h"
class HttpParser
{

private:
	static const char hex_chars[];
	static inline bool is_char(int c)
	{
		return c >= 0 && c <= 127;
	}
	static inline bool is_digit(char c)
	{
		return c >= '0' && c <= '9';
	}
	static inline bool is_ctl(int c)
	{
		return (c >= 0 && c <= 31) || c == 127;
	}
	static inline bool is_tspecial(int c)
	{
		switch (c)
		{
		case '(': case ')': case '<': case '>': case '@':
		case ',': case ';': case ':': case '\\': case '"':
		case '/': case '[': case ']': case '?': case '=':
		case '{': case '}': case ' ': case '\t':
			return true;
		default:
			return false;
		}
	}
	static std::string trim(std::string & source) 
	{
		source.erase(0, source.find_first_not_of(" \n\r\t"));
		source.erase(source.find_last_not_of(" \n\r\t") + 1);
		return source;
	}

	static size_t split(const std::string& str, const std::string& delimiter, std::vector<std::string>& results)
	{
		auto tmp = str;
		size_t pos = 0;
		std::string token;
		size_t i = 0;
		while (!tmp.empty()) {
			if ((pos = tmp.find(delimiter)) != std::string::npos)
			{
				results.emplace_back(tmp.substr(0, pos));
				tmp.erase(0, pos + delimiter.length());
			}
			else
			{
				results.emplace_back(std::move(tmp));
			}
			i++;
		}
		return i;
	}

	static size_t split(std::string&& str, const std::string& delimiter, std::vector<std::string>& results)
	{
		size_t pos = 0;
		std::string token;
		size_t i = 0;
		while (!str.empty()) {
			if ((pos = str.find(delimiter)) != std::string::npos)
			{
				results.emplace_back(str.substr(0, pos));
				str.erase(0, pos + delimiter.length());
			}
			else
			{
				results.emplace_back(std::move(str));
			}
			i++;
		}
		return i;
	}

	char from_hex(char ch) {
		return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
	}

	std::string urlEncode(const std::string &value)
	{
		std::ostringstream escaped;
		escaped.fill('0');
		escaped << std::hex;

		for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i)
		{
			std::string::value_type c = (*i);

			if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
			{
				escaped << c;
				continue;
			}

			escaped << std::uppercase;
			escaped << '%' << std::setw(2) << int((unsigned char)c);
			escaped << std::nouppercase;
		}

		return escaped.str();
	}

	std::string urlDecode(std::string text) {
		char h;
		std::ostringstream escaped;
		escaped.fill('0');

		for (auto i = text.begin(), n = text.end(); i != n; ++i) {
			std::string::value_type c = (*i);

			if (c == '%') {
				if (i[1] && i[2]) {
					h = from_hex(i[1]) << 4 | from_hex(i[2]);
					escaped << h;
					i += 2;
				}
			}
			else if (c == '+') {
				escaped << ' ';
			}
			else {
				escaped << c;
			}
		}
		return escaped.str();
	}

	static inline std::string to_hex(std::string const& s)
	{
		std::string ret;
		for (std::string::const_iterator i = s.begin(); i != s.end(); ++i)
		{
			ret += hex_chars[((unsigned char)*i) >> 4];
			ret += hex_chars[((unsigned char)*i) & 0xf];
		}
		return ret;
	}
	static inline void to_hex(char const *in, int len, char* out)
	{
		for (char const* end = in + len; in < end; ++in)
		{
			*out++ = hex_chars[((unsigned char)*in) >> 4];
			*out++ = hex_chars[((unsigned char)*in) & 0xf];
		}
		*out = '\0';
	}

	 
	static inline bool is_print(char c)
	{
		return c >= 32 && c < 127;
	}
	static inline bool tolower_compare(char a, char b)
	{
		return std::tolower(a) == std::tolower(b);
	}
	static inline bool headers_equal(const std::string &a, const std::string &b)
	{
		if (a.length() != b.length())
			return false;
		return std::equal(a.begin(), a.end(), b.begin(), tolower_compare);
	}
	static inline void check_header(const std::string &name, const std::string &value,
		std::string &content_type, std::size_t &content_length,
		std::string &location)
	{
		if (headers_equal(name, "Content-Type"))
			content_type = value;
		else if (headers_equal(name, "Content-Length"))
			content_length = std::atoi(value.c_str());
		else if (headers_equal(name, "Location"))
			location = value;
	}
	static inline void check_header(const std::string &name, const std::string &value, HttpRequest* req)
	{

		if (headers_equal(name, "Content-Length"))
			req->setContentLength(std::atoi(value.c_str()));
		req->addHeader(name, value);
	}

public:
	static bool parseHttpFirstline(const char * buffer, size_t size, HttpRequest* req);
	static bool parseUrl(const char* url, size_t size, HttpRequest * req);
	static bool parseHttpHeader(const char * buffer, size_t size, HttpRequest* req);
};