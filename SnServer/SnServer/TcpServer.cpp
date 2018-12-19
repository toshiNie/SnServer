#include "stdafx.h"
#include "SigProcess.h"
#include "TcpServer.h"


void AcceptHandler::ReadHandle()
{
	LOG_INFO("accept handle");
	std::lock_guard<std::mutex> lg(*mutex_);
	int sock = ::accept(sock_, NULL, NULL);
	if (sock < 0)
	{
		return;
	}
	LOG_INFO("accept sock: " + std::to_string(sock));
	socketutil::make_socket_non_blocking(sock);
	EventHandlerPtr spReadHandler(new EchoReadHandler(sock, spReactor_));
	spReactor_->AddHandler(spReadHandler);
}

 void EchoReadHandler::ReadHandle()
{
	LOG_INFO("echo read handle");
	int r = 0;
	int len = 0;
	int ret = ::read(sock_, (void*)&len, sizeof(len));
	if (ret == 0)
	{
		LOG_INFO("client disconnect:" + std::to_string(sock_));
		spReactor_->Remove(sock_);
		::close(sock_);
		return;
	}
	if (ret < 0)
	{
		LOG_INFO("error:" + std::to_string(errno));
		spReactor_->Remove(sock_);
		::close(sock_);
		return;
	}
	LOG_INFO("read len:" + std::to_string(len));
	std::vector<char> buffer(len);
	while (r < len)
	{
		r = ::read(sock_, buffer.data() + r, len);
		LOG_INFO("read size:" + std::to_string(r));
		if (r <= 0)
		{
			if (errno = EAGAIN)
			{
				return;
			}
			LOG_INFO("error:"  + std::to_string(errno));
			spReactor_->Remove(sock_);
			::close(sock_);
			return;
		}
	}
	readBuffer_.append(buffer.data(), len);
	LOG_INFO(readBuffer_.getbuffer());
	//EventHandlerPtr spHander(new EchoWriteHandler(sock_,spReactor_, std::move(readBuffer_)));
	//ReactorPtr spReactor = Reactor::GetReactorPtr();
	//spReactor->AddHandler(spHander);

	EventHandlerPtr spHander(new EchoWriteHandler(sock_, spReactor_, std::move(readBuffer_)));
	spReactor_->AddHandler(spHander);
}
 

 void EchoWriteHandler::WriteHandle()
 {
	 LOG_INFO(__FUNCTION__);
	 int w = 0;
	 while (writedBuffer_.size() > 0)
	 {
		 w = ::write(sock_, (void*)writedBuffer_.getbuffer(), writedBuffer_.size());
		 LOG_INFO("write size:" + std::to_string(w));
		 if (w < 0)
		 {
			 if (errno == EAGAIN)
			 {
				 continue;
			 }
			 else
			 {
				 LOG_INFO("write error:" + std::to_string(errno));
				 spReactor_->Remove(sock_);
				 ::close(sock_);
			 }
		 }
		 writedBuffer_.consumHead(w);
	 }
	 EventHandlerPtr spReadHandler(new EchoReadHandler(sock_, spReactor_));
	 spReactor_->AddHandler(spReadHandler);
 }

 void EchoServer::run()
 {
	 listenAddress_.strIp_ = "127.0.0.1";
	 listenAddress_.port = 4321;
	 listtenSocket_.BindAddress(listenAddress_);
	 if (!listtenSocket_.Listen(1024))
	 {
		 LOG_INFO("listen failed");
		 return;
	 }
	 socketutil::make_socket_non_blocking(listtenSocket_.GetSockFd());
	 LOG_INFO("listen OK");

	 //for (int i = 0; i < 1; ++i)
	 //{
		// pool_.PushPack(std::bind(&EchoServer::readThread, this));
	 //}
	 //pool_.PushPack(std::bind(&EchoServer::acceptThread, this));
	 //pool_.Start();
	 for (int i = 0; i < 3; ++i)
	 {
		 threads_.push_back(ThreadPtr(new std::thread(std::bind(&EchoServer::readThread, this))));
	 }
	 for (int i = 0; i < 3; ++i)
	 {
		 threads_[i]->join();
	 }

	 //while (pool_.isRunning())
	 //{
		// std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	 //}
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
	 EventHandlerPtr spAcceptHandler(new AcceptHandler(listtenSocket_.GetSockFd(), spReactor, &mutex_));
	 spReactor->AddHandler(spAcceptHandler);
	 while (true)
	 // for(int i =0;i<10000;++i)
	 {
		 spReactor->Loop(10);
		 //rintf("loop address: %x\n", &(*spReactor));
		 //std::this_thread::sleep_for(std::chrono::milliseconds(100));
	 }
 }