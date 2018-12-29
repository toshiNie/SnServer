#pragma once
#include"stdafx.h"


class HttpRequest
{
public:
	enum class Method
	{
		GET,
		POST,
		HEAD,
		PUT,
		DELETE,
		UNKNOWMEN
	};
	enum class Version
	{
		HTTP10,
		HTTP11,
		UNKNOWMEN
	};
public:
	HttpRequest()
		:method_(Method::UNKNOWMEN),
		version_(Version::HTTP11),
		contentLength_(-1)
	{

	}
	HttpRequest(HttpRequest&& rhs):
		method_(std::move(rhs.method_)),
		version_(std::move(rhs.version_)),
		url_(std::move(rhs.url_)),
		header_(std::move(rhs.header_)),
		contentLength_(std::move(rhs.contentLength_)),
		content_(std::move(content_))
	{

	}


	void setMethod(const std::string& strMethod)
	{
		if (strMethod == "GET")
		{
			method_ = Method::GET;
		}
		else if (strMethod == "POST")
		{
			method_ = Method::POST;
		}
		else if (strMethod == "HEAD")
		{
			method_ = Method::HEAD;
		}
		else if (strMethod == "PUT")
		{
			method_ = Method::PUT;
		}
		else if (strMethod == "DELETE")
		{
			method_ = Method::DELETE;
		}
		else
		{
			method_ = Method::UNKNOWMEN;
		}
		return;
	}
	void setVersion(const std::string& strVersion)
	{
		if (strVersion == "HTTP/1.0")
		{
			version_ = Version::HTTP10;
		}
		else
		{
			version_ = Version::HTTP11;
		}
	}
	void setUrl(const std::string& strUrl)
	{
		url_ = strUrl;
	}
	void appendHeader(const std::string& strHeader)
	{
		int index = strHeader.find_first_of(':');
		header_.insert(std::make_pair(strHeader.substr(0,index),strHeader.substr(index+1,header_.size())));
	}
	int getContentLength()
	{
		return contentLength_;
	}
	void setContentLength(int length)
	{
		contentLength_ = length;
	}
	void addHeader(const std::string& name, const std::string& value)
	{
		header_[name] = value;
	}
	

	void swap(HttpRequest& req)
	{
		std::swap(method_, req.method_);
		std::swap(version_, req.version_);
		url_.swap(req.url_);
		header_.swap(req.header_);
		content_.swap(req.content_);
		std::swap(contentLength_ , req.contentLength_);
	}
	void reset()
	{
		method_ = Method::UNKNOWMEN;
		version_ = Version::HTTP11;
		url_.clear();
		header_.clear();
		content_.clear();
		contentLength_ = -1;
	}
	std::map<std::string, std::string>& getHeader()
	{
		return header_;
	}

	template<typename T>
	void setContent(T&& content)
	{
		content_ = std::forward<T>(content);
	}

	void setContent(std::vector<char>&& content)
	{
		content_ = content;
	}

	std::vector<char>& getContent()
	{
		return content_;
	}
private:
	Method method_;
	Version version_;
	std::string url_;
	std::map<std::string, std::string> header_;
	int contentLength_;
	std::vector<char> content_;
};