#include"stdafx.h"
#include "TcpServer.h"


void AcceptHandler::ReadHandle()
{
	int sock = ::accept(sock_, NULL, NULL);
	socketutil::make_socket_non_blocking(sock);
	EventHandlerPtr spReadHandler(new EchoReadHandler(sock, spReactor_));
	spReactor_->AddHandler(spReadHandler);
}

 void EchoReadHandler::ReadHandle()
{
	LOG_INFO(__FUNCTION__);
	int r = 0;
	while (true)
	{
		LOG_INFO("Loop");
		std::vector<char> buffer(1025);
		r = socketutil::readline(sock_, buffer.data(), 1024);
		if (r < 0)
		{
			std::cout << "error:" << errno;
			spReactor_->Remove(sock_);
			::close(sock_);
			return;
		}
		if (r == 0)
		{
			LOG_INFO("client disconnect");
			readBuffer_.reset();
			spReactor_->Remove(sock_);
			::close(sock_);
			return;
		}
		std::cout << "read size:" << r << std::endl;
		if (r < 1024)
		{
			readBuffer_.append(buffer.data(), r);
			break;
		}
		readBuffer_.append(buffer.data(), r - 1);
	}
	std::cout << readBuffer_.getbuffer() << std::endl;
	//EventHandlerPtr spHander(new EchoWriteHandler(sock_,spReactor_, std::move(readBuffer_)));
	//ReactorPtr spReactor = Reactor::GetReactorPtr();
	//spReactor->AddHandler(spHander);

	int w = 0;
	w = socketutil::writen(sock_, (void*)readBuffer_.getbuffer(), readBuffer_.size());
	std::cout << "write size:" << w << std::endl;
	readBuffer_.reset();
	if (w < r)
	{
		readBuffer_.consumHead(w);
		EventHandlerPtr spHander(new EchoWriteHandler(sock_, spReactor_, std::move(readBuffer_)));
		spHander->SetHandlerType(WriteEvent);
		ReactorPtr spReactor = Reactor::GetReactorPtr();
		spReactor->AddHandler(spHander);
	}
	spReactor_->Mod(sock_, ReadEvent);
	//else
	//{
	//	readBuffer_.reset();
	//	spReactor_->Mod(sock_, ReadEvent);
	//}
}
 

 void EchoWriteHandler::WriteHandle()
 {
	 LOG_INFO(__FUNCTION__);
	 socketutil::writen(sock_, (void*)writedBuffer_.getbuffer(), writedBuffer_.size());
	 writedBuffer_.reset();
 }

 void EchoServer::run()
 {
	 listenAddress_.strIp_ = "127.0.0.1";
	 listenAddress_.port = 4321;
	 listtenSocket_.BindAddress(listenAddress_);
	 listtenSocket_.Listen(1024);
	 LOG_INFO("listen OK");

	 for (int i = 0; i < 1; ++i)
	 {
		 pool_.PushPack(std::bind(&EchoServer::readThread, this));
	 }
	 //pool_.PushPack(std::bind(&EchoServer::acceptThread, this));
	 pool_.Start();

	 while (pool_.isRunning())
	 {
		 std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	 }
 }

 void EchoServer::acceptThread()
 {
	 LOG_INFO("acceptThread start");
	 int i = 0;
	 while (true)
	 {
		 int sock = listtenSocket_.Accept();
		 LOG_INFO("new client connect: " + std::to_string(sock));
		 std::lock_guard<std::mutex> lg(mutex_);
		 i = i % subReactors_.size();
		 ReactorPtr spReactor = subReactors_[i];
		 //printf("accept address: %x\n", &(*spReactor));
		 socketutil::make_socket_non_blocking(sock);
		 EventHandlerPtr spEventHandller(new EchoReadHandler(sock, spReactor));
		 spReactor -> AddHandler(spEventHandller);
		 i++;
	 }
 }

 void EchoServer::readThread()
 {
	 LOG_INFO("readThread start");
	 ReactorPtr spReactor(new Reactor());
	 EventHandlerPtr spAcceptHandler(new AcceptHandler(listtenSocket_.GetSockFd(), spReactor));
	 spReactor->AddHandler(spAcceptHandler);
	 while (true)
	 {
		 spReactor->Loop(10);
		 //printf("loop address: %x\n", &(*spReactor));
		 std::this_thread::sleep_for(std::chrono::milliseconds(100));
	 }
 }