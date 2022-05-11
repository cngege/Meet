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
#include <thread>
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
				connected = false;
			}
		}
	public:
		using DisConnectEvent = void(__fastcall*)();

	private:
		SOCKET sockfd;
		sockaddr_in sock;
		bool connected = false;
		Family family;

		//thread
		std::thread recv_thread;

		//Event
		DisConnectEvent disConnectEvent;

	public:

		/// <summary>
		/// 连接远程主机
		/// </summary>
		/// <param name="ip">主机 ip类</param>
		/// <param name="port">远程主机的端口</param>
		/// <returns>连接成功/失败</returns>
		auto Connect(IP ip, unsigned short port) -> bool
		{
			if (connected)
			{
				return true;
			}
			if ((sockfd = socket(sock.sin_family, SOCK_STREAM, 0)) == -1)
			{
				return false;
			}
			sock.sin_port = htons(port);
			memcpy(&sock.sin_addr, ip.GetHost()->h_addr, ip.GetHost()->h_length);
			if (connect(sockfd, (struct sockaddr*)&sock, sizeof(sock)) != 0)
			{
				closesocket(sockfd);
				return false;
			}
			connected = true;
			
			// 开启线程 接收并处理消息后触发监听事件
			recv_thread = std::thread(startRecv,this);
			recv_thread.detach();
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
	public:
		// 注册回调事件
		void OnDisConnect(DisConnectEvent _disConnectEvent)
		{
			disConnectEvent = _disConnectEvent;
		};

	private:
		static auto startRecv(TCPClient* c) -> void
		{
			if (!c->connected)
			{
				char buffer[5];
				memset(buffer, 0, sizeof(buffer));

				while (c->connected)
				{
					int recvbytecount;
					if ((recvbytecount = recv(c->sockfd, buffer, sizeof(buffer), 0)) <= 0)
					{
						//返回 0 网络中断
						if (recvbytecount == 0)
						{
							c->connected = false;
							c->disConnectEvent();
							break;
						}
						if (recvbytecount == SOCKET_ERROR)
						{
							//copy数据时出错
							//调用错误回调
						}
					}

					if (recvbytecount < 5)
					{
						continue;
					}
				}
			}
		};
	};

	
}

#endif //!___MIRACLEFOREST_MEET_TCPCLIENT___

