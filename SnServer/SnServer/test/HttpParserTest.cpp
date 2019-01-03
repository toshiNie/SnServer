#include<gtest/gtest.h>
#include"../HttpRequst.h"
#include"../HttpParser.h"
TEST(HTTPParseURLTEST, NormalTest)
{
	HttpRequest request;
	std::string url = "/g/search?hl=zh-CN&source=hp&q=domety&aq=f&oq=";
	HttpParser::parseUrl(url.c_str(),url.size(), &request);
	ASSERT_EQ(request.getPaths().size(), 2);
	ASSERT_STREQ(request.getPaths()[0].c_str(), "g");
	ASSERT_STREQ(request.getPaths()[1].c_str(), "search");

	ASSERT_STREQ(request.getParams()["hl"].c_str(), "zh-CN");
	ASSERT_STREQ(request.getParams()["source"].c_str(), "hp");
	ASSERT_STREQ(request.getParams()["q"].c_str(), "domety");
	ASSERT_STREQ(request.getParams()["aq"].c_str(), "f");
	ASSERT_STREQ(request.getParams()["oq"].c_str(), "");
}