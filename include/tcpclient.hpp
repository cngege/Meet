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
		using ushort = unsigned short;

		/// <summary>
		/// TCP断开连接时触发的事件
		/// </summary>
		using DisConnectEvent = void(__fastcall*)();

		/// <summary>
		/// 接收数据时触发的函数类型
		/// </summary>
		/// <param name="ULONG64">本次接收到的数据长度</param>
		/// <param name="const char*">接收到的数据</param>
		using RecvDataEvent = void(__fastcall*)(ULONG64, const char*);

		/// <summary>
		/// 接收消息时错误回调
		/// </summary>
		/// <param name="int">错误值</param>
		using RecvErrorEvent = void(__fastcall*)(int);
	public:
		TCPClient(){}
		~TCPClient(){
			if (_sockfd){
				shutdown(_sockfd, SD_BOTH);
				closesocket(_sockfd);
				WSACleanup();
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
			_family = ip.IPFamily;
			_sock.sin_family = (ip.IPFamily == Family::IPV4) ? AF_INET : AF_INET6;
			if (ip.IPFamily == Family::IPV6){
				return Error::unsupportedOperations;
			}

			//Initializing the socket library
			WSADATA wsadata; //Define a structure of type WSADATA to store the Windows Sockets data returned by the WSAStartup function call
			WORD sock_version = MAKEWORD(2, 0); //Set the version number
			if (WSAStartup(sock_version, &wsadata)) //Initialize the socket, start the build, and load "ws2_32.lib" into memory
			{
				return Error::initializationWinsockFailed;
			}

			//Create sockets
			if ((_sockfd = socket(_sock.sin_family, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR){
				return Error::socketError;
			}
			_sock.sin_port = htons(port);
			_sock.sin_addr = ip.InAddr;
			if (::connect(_sockfd, (struct sockaddr*)&_sock, sizeof(_sock)) == INVALID_SOCKET){
				closesocket(_sockfd);
				return Error::connectFailed;
			}
			_connected = true;

			// Set the connection to non-blocking mode recv return in time
			if (!_blockingmode) {
				u_long iMode = 1;
				::ioctlsocket(_sockfd, FIONBIO, &iMode);
			}

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
			if (::inet_pton(AF_INET6, ip.c_str(), &ipaddrA->h_addr) != 1) //0:String is not a valid IP address,-1:Other error
			{
				return Error::changeError;
			}
			IP ipaddrB(ipaddrA);
			return connect(ipaddrB, port);
		}

		/// <summary>
		/// Disconnection
		/// </summary>
		/// <returns></returns>
		Error disConnect(){
			if (!_connected) {
				return Error::noConnected;
			}
			shutdown(_sockfd, SD_BOTH);
			if (closesocket(_sockfd) == 0) {
				_connected = false;
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
			auto sendcount = send(_sockfd, text.data(), text.length(), 0);
			if (sendcount <= 0) {
				return Error::sendFailed;
			}
			return Error::noError;
		};

		/// <summary>
		/// Sending bytes
		/// </summary>
		/// <param name="data"></param>
		/// <returns></returns>
		Error sendData(const char* data) {
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
		/// 设置阻塞模式,在连接前设置 默认为阻塞模式
		/// </summary>
		/// <param name="blocking">是否设置成阻塞模式</param>
		/// <returns></returns>
		Error setBlockingMode(bool blocking){
			if (!_connected) {
				_blockingmode = blocking;
				return Error::noError;
			}
			return Error::connecting;
		}


		/// <summary>
		/// Register callback events
		/// </summary>
		/// <param name="disConnectEvent"></param>
		void onDisConnect(DisConnectEvent disConnectEvent){
			_disConnectEvent = disConnectEvent;
		};

		/// <summary>
		/// Register callback events
		/// </summary>
		/// <param name="disConnectEvent"></param>
		void onRecvData(RecvDataEvent recvDataEvent){
			_recvDataEvent = recvDataEvent;
		}

		/// <summary>
		/// Register callback events
		/// </summary>
		/// <param name="recvErrorEvent"></param>
		void onRecvError(RecvErrorEvent recvErrorEvent) {
			_recvErrorEvent = recvErrorEvent;
		}

	public:

		/// <summary>
		/// Thread function, responsible for receiving network packets, and classify and analyze the processing
		/// </summary>
		/// <param name="c">TCPClient Instance</param>
		static void startRecv(TCPClient* c) {
			if (c->_connected){

				char buffer[1024];
				memset(buffer, '\0', sizeof(buffer));

				while (c->_connected){
					int recvbytecount;
					if ((recvbytecount = recv(c->_sockfd, buffer, sizeof(buffer), 0)) <= 0){
						//Return 0 Network Outage
						if (recvbytecount == 0){
							c->_connected = false;
							if (c->_disConnectEvent != NULL) {
								c->_disConnectEvent();
							}
							break;
						}

						//阻塞模式
						if (c->_blockingmode) {
							if (recvbytecount == SOCKET_ERROR) {
								if (c->_recvErrorEvent != NULL) {
									c->_recvErrorEvent(recvbytecount);
								}
							}
						}

					}//if ((recvbytecount = recv(c->sockfd, buffer, sizeof(buffer), 0)) <= 0)

					//接收数据 触发回调
					else {
						if (c->_recvDataEvent != NULL) {
							c->_recvDataEvent(recvbytecount, buffer);
						}
						memset(buffer, '\0', sizeof(buffer));
					}
				}//while (c->connected)
			}//if(!c->connected)
		};//static void startRecv(TCPClient* c)
	private:
		SOCKET _sockfd = NULL;
		sockaddr_in _sock = {};
    	bool _connected = false;
		bool _blockingmode = true;
	    Family _family = Family::IPV4;

		std::thread _recv_thread;

		/// <summary>
		/// 断开连接时触发的事件
		/// </summary>
		DisConnectEvent _disConnectEvent = NULL;

		/// <summary>
		/// 有数据达到时自动触发的事件
		/// </summary>
		RecvDataEvent _recvDataEvent = NULL;

		/// <summary>
		/// recv线程接收消息时出错触发事件
		/// </summary>
		RecvErrorEvent _recvErrorEvent = NULL;
	};//class TCPClient

}//namespace meet

#endif //!___MIRACLEFOREST_MEET_TCPCLIENT___