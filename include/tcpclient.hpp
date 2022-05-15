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

namespace meet{
	/// <summary>
	/// 
	/// </summary>
	class TCPClient{
		using DisConnectEvent = void(__fastcall*)();
		using ushort = unsigned short;
	public:
		TCPClient(){}
		~TCPClient(){
			if (_sockfd){
				closesocket(_sockfd);
			}
			_connected = false;
		}
	public:
	public:

		/// <summary>
		/// To connect to a remote host, you must connect without establishing a connection
		/// </summary>
		/// <param name="ip">host</param>
		/// <param name="port">port</param>
		/// <returns></returns>
		Error connect(IP ip, ushort port) {
			if (_connected){
				return Error::connecting;
			}
			//Get IP address type v4/v6
			memset(&_sock, 0, sizeof(_sock));
			_family = ip.family;
			_sock.sin_family = (ip.family == Family::IPV4) ? AF_INET : AF_INET6;
			if (ip.family == Family::IPV6){
				return Error::unsupportedOperations;
			}

			//初始化套接字库
			WSADATA wsadata; //定义一个WSADATA类型的结构体，存储被WSAStartup函数调用后返回的Windows Sockets数据
			WORD sock_version = MAKEWORD(2, 0); //设置版本号
			if (WSAStartup(sock_version, &wsadata)) //初始化套接字，启动构建，将“ws2_32.lib”加载到内存中
			{
				return Error::initializationWinsockFailed;
			}

			//Create sockets
			if ((_sockfd = socket(_sock.sin_family, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR){
				return Error::socketError;
			}
			_sock.sin_port = htons(port);
			_sock.sin_addr = ip.inaddr;
			if (::connect(_sockfd, (struct sockaddr*)&_sock, sizeof(_sock)) == INVALID_SOCKET){
				closesocket(_sockfd);
				return Error::connectFailed;
			}
			_connected = true;

			///Open a thread Triggers a listening event after receiving and processing a message
			_recv_thread = std::thread(startRecv, this);
			_recv_thread.detach();
			return Error::noError;
		};
		
		/// <summary>
		/// Specify IPV4 IP for connection
		/// </summary>
		/// <param name="ip">Remote host ipv4 address, e.g. "127.0.0.1"</param>
		/// <param name="port">Remote host port</param>
		/// <returns></returns>
		Error connectV4(std::string ip, ushort port) {
			IP ipaddrB(Family::IPV4,ip);
			return connect(ipaddrB, port);
		}

		/// <summary>
		/// Specify IPV6 IP for connection
		/// </summary>
		/// <param name="ip">Remote host ipv6 address, such as "2000:0:0:0:0:0:0:0:1"</param>
		/// <param name="port">Remote host port</param>
		/// <returns></returns>
		Error connectV6(std::string ip, ushort port) {
			hostent* ipaddrA = new hostent();
			if (::inet_pton(AF_INET6, ip.c_str(), &ipaddrA->h_addr) != 1) //0:字符串不是有效的IP地址,-1:其他错误
			{
				return Error::changeError;
			};
			IP ipaddrB(ipaddrA);
			return connect(ipaddrB, port);
		}

		/// <summary>
		/// Disconnection
		/// </summary>
		/// <returns></returns>
		Error disConnect(){
			if (_connected && _sockfd && closesocket(_sockfd) == 0){
				return Error::noError;
			}
			return Error::unkError;
		};

		/// <summary>
		///  Send text
		/// </summary>
		/// <param name="text">Text to be sent</param>
		/// <returns></returns>
		Error sendText(std::string text) {
			if (!_connected) {
				return Error::noConnected;
			}
			auto textlen = text.length();
			auto sendbyte = new char[textlen + 10]; // textlen \n:1 + size_t:8 + datatype:1 = 10
			memset(sendbyte, 0, textlen + 10);
			memcpy(sendbyte, &textlen, sizeof(size_t));
			sendbyte[8] = (byte)DataType::TEXT;
			sprintf(sendbyte + 9,text.c_str());
			auto sendcount = send(_sockfd, sendbyte, strlen(sendbyte), 0);
			if (sendcount <= 0) {
				return Error::sendFailed;
			}
			return Error::noError;
		};

		/// <summary>
		/// 发送字节
		/// </summary>
		/// <param name="data"></param>
		/// <returns></returns>
		Error sendData(char* data) {
			if (!_connected) {
				return Error::noConnected;
			}
			auto sendcount = send(_sockfd, data, strlen(data), 0);
			if (sendcount <= 0) {
				return Error::sendFailed;
			}
			return Error::noError;
		};

		/// <summary>
		/// Register callback events
		/// </summary>
		/// <param name="disConnectEvent"></param>
		void onDisConnect(DisConnectEvent disConnectEvent){
			_disConnectEvent = disConnectEvent;
		};

	public:

		/// <summary>
		/// Thread function, responsible for receiving network packets, and classify and analyze the processing
		/// </summary>
		/// <param name="c">TCPClient Instance</param>
		static void startRecv(TCPClient* c) {
			if (!c->_connected){
				char buffer[1024];
				memset(buffer, 0, sizeof(buffer));

				while (c->_connected){
					int recvbytecount;
					if ((recvbytecount = recv(c->_sockfd, buffer, sizeof(buffer), 0)) <= 0){
						//Return 0 Network Outage
						if (recvbytecount == 0){
							c->_connected = false;
							c->_disConnectEvent();
							break;
						}
						if (recvbytecount == SOCKET_ERROR){
							//Error while copying data
							//Calling error callbacks
						}
					}//if ((recvbytecount = recv(c->sockfd, buffer, sizeof(buffer), 0)) <= 0)

					if (recvbytecount < 5){
						continue;
					}
				}//while (c->connected)
			}//if(!c->connected)
		};//static void startRecv(TCPClient* c)
	private:
		SOCKET _sockfd = NULL;
		sockaddr_in _sock;
    	bool _connected = false;
	    Family _family = Family::IPV4;

		std::thread _recv_thread;

		/// <summary>
		/// Event
		/// </summary>
		DisConnectEvent _disConnectEvent = NULL;
	};//class TCPClient

}//namespace meet

#endif //!___MIRACLEFOREST_MEET_TCPCLIENT___