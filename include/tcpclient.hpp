/*
*
* Copyright(C) 2022 MiracleForest Studio. All Rights Reserved.
*
* @filename:tcpclient.hpp
* @creation time:2022.5.10.12:47
* @created by:Lovelylavender4
* -----------------------------------------------------------------------------
*
*
* -----------------------------------------------------------------------------
* If you have contact or find bugs,
* you can go to Github or email (MiracleForest@Outlook.com) to give feedback.
* We will try to do our best!
*/
#ifndef ___MIRACLEFOREST_MEET_TCPCLIENT___
#define ___MIRACLEFOREST_MEET_TCPCLIENT___

#include <string>
#include "ip.hpp"

namespace meet
{

	class TCPClient
	{
		TCPClient(Family f = Family::IPV4)
		{
			memset(&sock, 0, sizeof(sock));
			family = f;
			if (f == Family::IPV4)
			{
				sock.sin_family = AF_INET;
			}
			else if (f == Family::IPV6)
			{
				//暂不支持
				sock.sin_family = AF_INET6;
			}
		}
		~TCPClient()
		{
			if (sockfd)
			{
				closesocket(sockfd);
			}
		}

	private:
		SOCKET sockfd;
		sockaddr_in sock;
		bool connected = false;
		Family family;

	public:

		//初始化客户端  初始化前先注册好各种监听函数
		auto Init() -> bool
		{
			if ((sockfd = socket(sock.sin_family, SOCK_STREAM, 0)) == -1)
			{
				return false;
			}
			// 开启一个线程 循环接收网络消息
			return true;
		};

		//连接原创主机
		auto Connect(IP ip, unsigned short port) -> bool
		{
			sock.sin_port = htons(port);
			memcpy(&sock.sin_addr, ip.GetHost()->h_addr, ip.GetHost()->h_length);
			if (connect(sockfd, (struct sockaddr*)&sock, sizeof(sock)) != 0)
			{
				//perror("connect");
				closesocket(sockfd);
				return false;
			}
			connected = true;
			return true;
		};

		//断开连接 调用函数断开与主机的连接
		auto DisConnect() -> bool
		{

		};

		auto SendText(std::string) -> bool
		{

		};

		// 传输文件有个回调 CALL,时刻回传显示传输进度
		auto SendFile(std::string) -> bool
		{

		};

	private:
		auto StartRecv() -> void
		{

		};
	};

	
}

#endif //!___MIRACLEFOREST_MEET_TCPCLIENT___

