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
TEST(HTTPParserTest, NormalTest)
{
	std::string strHttp = "GET https://kyfw.12306.cn/otn/leftTicket/queryZ?leftTicketDTO.traindate=2019-02-11&leftTicketDTO.from_station=ZBK&leftTicketDTO.tostation=SNH&purposecodes=ADULT HTTP/1.1\r\n";
	strHttp += "Accept: */*\r\n";
	strHttp += "Accept-Language: zh-CN,zh;q=0.9\r\n";
	strHttp += "Accept-Encoding: gzip, deflate, br\r\n";
	strHttp += "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/71.0.3578.98 Safari/537.36\r\n";
	strHttp += "Cache-Control: no-cache\r\n";
	strHttp += "X-Requested-With: XMLHttpRequest\r\n";
	strHttp += "If-Modified-Since: 0\r\n";
	strHttp += "Referer: https://kyfw.12306.cn/otn/leftTicket/init?linktypeid=dc\r\n";
	strHttp += "Host: kyfw.12306.cn\r\n";
	strHttp += "Cookie: 123141235346\r\n";
	strHttp += "Connection: Close\r\n\r\n";;
	HttpRequest req;
	HttpParser::parseHttpFirstline(strHttp.c_str(), strHttp.find("\r\n"), &req);
	HttpParser::parseHttpHeader(strHttp.c_str() + strHttp.find("\r\n") + 2 , strHttp.size() - strHttp.find("\r\n") - 2, &req);
	EXPECT_EQ(req.getMethod(), HttpRequest::Method::GET);
	EXPECT_EQ(req.getVersion(), HttpRequest::Version::HTTP11);
	EXPECT_EQ(req.getHeader().size(), 11);
	EXPECT_EQ(req.getParams().size(), 4);
	EXPECT_EQ(req.getPaths().size(), 3);

}