#pragma once
#include"stdafx.h"
class HttpResponse
{
public:
	const char * LRLF = "\r\n";
	const char SPACE = ' ';
	enum CODE
	{
		C200,
		C400,
		C401,
		C403,
		C404,
		C500,
		C503
	};
private:
	const char* codeToString(CODE code)
	{
		switch (code)
		{
		case HttpResponse::C200:
			return "200 OK";
		case HttpResponse::C400:
			return "400 Bad Request";
		case HttpResponse::C401:
			return "401 Unauthorized";
		case HttpResponse::C403:
			return "403 Forbidden";
		case HttpResponse::C404:
			return "404 Not Found";
		case HttpResponse::C500:
			return "500 Internal Server Error";
		case HttpResponse::C503:
			return "503 Server Unavailable";
		default:
			return "200 OK";
		}
	}
public:
	void setCode(CODE code)
	{
		code_ = code;
	}
	void AddHeader(const std::string& name, const std::string value)
	{
		header_.insert(std::make_pair(name, value));
	}

	void setContent(const std::string& content)
	{
		content_ = content;
		AddHeader("Content-Length", std::to_string(content.size()));
	}

	std::string serialize ()
	{
		std::stringstream ss;
		ss << "HTTP/1.1" << SPACE << codeToString(code_) << LRLF;
		for (auto& item : header_)
		{
			ss << item.first << ":" << SPACE << item.second << LRLF;
		}
		ss << LRLF;
		ss << content_;
		return ss.str();
	}
private:
	CODE code_;
	std::map<std::string, std::string> header_;
	std::string content_;
};