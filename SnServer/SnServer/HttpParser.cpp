
#include "stdafx.h"
#include "HttpParser.h"

const char HttpParser::hex_chars[] = "0123456789abcdef";


bool HttpParser::parseHttpFirstline(const char * buffer, size_t size, HttpRequest* req)
{
	std::string tmp(buffer, buffer + size);
	std::stringstream ss(tmp);
	int i = 0;
	std::string item;
	while (std::getline(ss, item, ' '))
	{
		if (i == 0)
		{
			req->setMethod(item);
		}
		else if (i == 1)
		{
			parseUrl(item.c_str(), item.size(), req);
			req->setUrl(item);
		}
		else if (i == 2)
		{
			req->setVersion(item);
		}
		else 
		{
			break;
		}
		i++;
	}
	return true;
}
bool HttpParser::parseUrl(const char* url, size_t size, HttpRequest* req)
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
				params.insert(std::make_pair(key, value));
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
bool HttpParser::parseHttpHeader(const char * buffer, size_t size, HttpRequest* req)
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