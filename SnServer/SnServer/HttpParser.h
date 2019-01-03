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
	static bool parseHttpFirstline(const char * buffer, size_t size, HttpRequest* req)
	{
		std::string tmp(buffer, buffer + size);
		std::stringstream ss(tmp);
		int i = 0;
		std::string item;
		while (std::getline(ss,item,' '))
		{
			if (i == 0)
			{
				req->setMethod(item);
			}
			else if(i == 1)
			{
				parseUrl(item.c_str(), item.size(), req);
				req->setUrl(item);
				
			}
			else if (i == 2)
			{
				req->setVersion(item);
			}
			else {
				break;
			}
			i++;
		}
		return true;
	}
	static bool parseUrl(const char* url,size_t size, HttpRequest * req)
	{
		enum class State
		{
			Start,
			Path,
			ParamKey,
			ParamValue,
			Fail
		} state = State::Start;
		std::string path;
		std::string key;
		std::string value;
		auto& paths = req->getPaths();
		auto& params = req->getParams();
		const char *cptr = url;
		for (int i = 0; i < size; ++i, cptr++)
		{
			char c = *cptr;
			switch (state)
			{
			case State::Start:
			{
				if (c == '/')
				{
					state = State::Path;
				}
				else
				{
					path.push_back(c);
					state = State::Path;
				}
				break;
			}
			case State::Path:
			{
				if (c == '/')
				{
					paths.emplace_back(path);
					path.clear();
				}
				else if (c == '?')
				{
					state = State::ParamKey;
					paths.emplace_back(path);
					path.clear();
				}
				else
				{
					path.push_back(c);
				}
				break;
			}
			case State::ParamKey:
			{
				if (c == '=')
				{
					state = State::ParamValue;
				}
				else
				{
					key.push_back(c);
				}
				break;
			}
			case State::ParamValue:
			{
				if (c == '&')
				{
					params.insert(std::make_pair(key,value));
					key.clear();
					value.clear();
					state = State::ParamKey;
					break;
				}
				else
				{
					value.push_back(c);
				}
				if (i == size - 1)
				{
					params.insert(std::make_pair(key, value));
				}
				break;
			}
			default:
				break;
			}
		}
		return true;
	}
	static bool parseHttpHeader(const char * buffer, size_t size,  HttpRequest* req)
	{
		enum
		{
			FirstLineStart,
			HeaderLineStart,
			HeaderLws,
			HeaderName,
			SpaceBeforeHeaderValue,
			HeaderValue,
			LineFeed,
			FinalLineFeed,
			Fail
		} state = FirstLineStart;
		std::string reason;
		std::string name;
		std::string value;
		size_t index = 0;
		const char *cptr = buffer;
		for (int i = 0; i < size; ++i, cptr++)
		{
			char c = *cptr;
			switch (state)
			{
			case FirstLineStart:
			{
				if (c == '\r')
					state = FinalLineFeed;
				else if (!is_char(c) || is_ctl(c) || is_tspecial(c))
					state = Fail;
				else
				{
					name.push_back(c);
					state = HeaderName;
				}
				break;
			}
			case HeaderLineStart:
			{
				if (c == '\r')
				{
					trim(name);
					trim(value);
					check_header(name, value, req);
					name.clear();
					value.clear();
					state = FinalLineFeed;
				}
				else if (c == ' ' || c == '\t')
					state = HeaderLws;
				else if (!is_char(c) || is_ctl(c) || is_tspecial(c))
					state = Fail;
				else
				{
					trim(name);
					trim(value);
					check_header(name, value, req);
					name.clear();
					value.clear();
					name.push_back(c);
					state = HeaderName;
				}
				break;
			}
			case HeaderLws:
			{
				if (c == '\r')
					state = LineFeed;
				else if (c == ' ' || c == '\t');
				else if (is_ctl(c))
					state = Fail;
				else
				{
					state = HeaderValue;
					value.push_back(c);
				}
			}
			case HeaderName:
			{
				if (c == ':')
					state = SpaceBeforeHeaderValue;
				else if (!is_char(c) || is_ctl(c) || is_tspecial(c))
					state = Fail;
				else
					name.push_back(c);
				break;
			}
			case SpaceBeforeHeaderValue:
			{
				if (c == ' ')
					state = HeaderValue;
				else if (is_ctl(c))
					state = Fail;
				else
				{
					value.push_back(c);
					state = HeaderValue;
				}
				break;
			}
			case HeaderValue:
			{
				if (c == '\r')
					state = LineFeed;
				else if (is_ctl(c))
					state = Fail;
				else
				{
					value.push_back(c);
				}
				break;
			}
			case LineFeed:
			{
				state = (c == '\n') ? HeaderLineStart : Fail;
				break;
			}
			case FinalLineFeed:
			{
				return (c == '\n') ? true : false;
			}
			default:
				return false;
			}

		}
		return false;
	}
};