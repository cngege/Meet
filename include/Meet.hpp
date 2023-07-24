﻿#pragma once

#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <functional>

#pragma comment(lib,"ws2_32.lib")

#ifndef ushort
#define ushort unsigned short
#endif

#ifndef ulong
#define ulong unsigned long
#endif

#ifndef MEET_LISTEN_BACKLOG
#define MEET_LISTEN_BACKLOG 5
#endif // !MEET_LISTEN_BACKLOG

// 默认的最大连接数 在没有设置的时候
#ifndef MEET_LISTEN_DEFAULT_MAXCONNECT
#define MEET_LISTEN_DEFAULT_MAXCONNECT 30
#endif // !MEET_LISTEN_DEFAULT_MAXCONNECT


namespace meet
{
	/// <summary>
	/// IP地址类型
	/// </summary>
	enum class Family {
		UNSPEC = 0,
		IPV4 = 2,
		IPV6 = 23
	};

	enum class Error : int {
		noError = 0,
		unkError = -1,
		initializationWinsockFailed = -16,	// 初始化 Winsock 失败
		socketError,
		bindError,
		listenError,
		acceptError,
		sendFailed,
		recvFailed,
		portTooSmall,
		maxcouTooBig,
		connectFailed,
		unsupportedOperations,
		connecting,
		noConnected,
		changeError,
		theMaximumNumberOfConnectionsHasBeenReached,
		theClientIsDisconnected,
		serverIsStarted = 100,
		serverNotStarted = 101,
		noFoundClient,
		dataTooLong
	};//enum class Error

	static std::string getString(Error errorCode) {
		switch (errorCode) {
			using enum Error;
		case noError:
		{
			return "noError";
		}
		case unkError:
		{
			return "unkError";
		}
		case initializationWinsockFailed:
		{
			return "initializationWinsockFailed";
		}
		case socketError:
		{
			return "socketError";
		}
		case bindError:
		{
			return "bindError";
		}
		case listenError:
		{
			return "listenError";
		}
		case acceptError:
		{
			return "acceptError";
		}
		case sendFailed:
		{
			return "sendFailed";
		}
		case recvFailed:
		{
			return "recvFailed";
		}
		case portTooSmall:
		{
			return "portTooSmall";
		}
		case maxcouTooBig:
		{
			return "maxcouTooBig";
		}
		case connectFailed:
		{
			return "connectFailed";
		}
		case unsupportedOperations:
		{
			return "unsupportedOperations";
		}
		case connecting:
		{
			return "connecting";
		}
		case noConnected:
		{
			return "noConnected";
		}
		case changeError:
		{
			return "changeError";
		}
		case theMaximumNumberOfConnectionsHasBeenReached:
		{
			return "theMaximumNumberOfConnectionsHasBeenReached";
		}
		case serverIsStarted:
		{
			return "serverIsStarted";
		}
		case serverNotStarted:
		{
			return "serverNotStarted";
		}
		case noFoundClient:
		{
			return "noFoundClient";
		}
		case dataTooLong:
		{
			return "dataTooLong";
		}
		default:
		{
			return "Error! unk errorCode!";
		}
		}
	}//static std::string getString(Error errorCode)

	/// <summary>
	/// 
	/// </summary>
	class IP
	{
	public:
		/// <summary>
		/// 
		/// </summary>
		/// <param name="_host"></param>
		IP(hostent* host) {
			if (host->h_addrtype == AF_INET) {
				IPFamily = Family::IPV4;
				memcpy(&InAddr, host->h_addr, host->h_length);
			}
			else if (host->h_addrtype == AF_INET6) {
				IPFamily = Family::IPV6;
				memcpy(&InAddr6, host->h_addr, host->h_length);
			}
		};

		/// <summary>
		/// 
		/// </summary>
		/// <param name="ipfamily">地址族</param>
		/// <param name="addr">字符串ip地址(不能使用域名)</param>
		//IP(Family ipfamily,std::string addr) {
		//	IPFamily = ipfamily;
		//	if (ipfamily == Family::IPV4){
		//		::inet_pton(AF_INET, addr.c_str(), &InAddr);
		//	}
		//	else if (ipfamily == Family::IPV6){
		//		::inet_pton(AF_INET6, addr.c_str(), &InAddr6);
		//	}
		//}

		/// <summary>
		/// IP地址 而不是域名
		/// </summary>
		/// <param name="addr"></param>
		IP(const std::string& addr) {
			IPFamily = Family::IPV4;
			auto ret = ::inet_pton(AF_INET, addr.c_str(), &InAddr);
			if (ret == 0) {
				IPFamily = Family::IPV6;
				::inet_pton(AF_INET6, addr.c_str(), &InAddr6);
			}
		}

		IP(IN_ADDR inaddr) {
			IPFamily = Family::IPV4;
			InAddr = inaddr;
		}

		IP(in_addr6 inaddr) {
			IPFamily = Family::IPV6;
			InAddr6 = inaddr;
		}

		IP(sockaddr_in addr_in) {
			IPFamily = Family::IPV4;
			InAddr = addr_in.sin_addr;
		}

		IP(sockaddr_in6 addr_in) {
			IPFamily = Family::IPV6;
			InAddr6 = addr_in.sin6_addr;
		}

		IP() : IP("0.0.0.0") {

		}
	public:
	public:
	public:

		/// <summary>
		/// 将IP地址转为字符串输出
		/// </summary>
		/// <returns></returns>
		std::string toString() {

			if (IPFamily == Family::IPV4) {
				char IPStrBuf[INET_ADDRSTRLEN] = { '\0' };
				return std::string(inet_ntop(AF_INET, (void*)&InAddr, IPStrBuf, sizeof(IPStrBuf)));
			}
			else {
				char IPStrBuf[INET6_ADDRSTRLEN] = { '\0' };
				return std::string(inet_ntop(AF_INET6, (void*)&InAddr6, IPStrBuf, sizeof(IPStrBuf)));
			}

		}

	public:

		static IP getaddrinfo(Family f, const std::string dom) {
			WSADATA wsadata; //Define a structure of type WSADATA to store the Windows Sockets data returned by the WSAStartup function call
			if (WSAStartup(MAKEWORD(2, 0), &wsadata)) //Initialize the socket, start the build, and load "ws2_32.lib" into memory
			{
				return NULL;
			}
			addrinfo hints;
			memset(&hints, 0, sizeof(addrinfo));
			hints.ai_family = (int)f;			/* Allow IPv4 */
			hints.ai_flags = AI_CANONNAME;		/* For wildcard IP address */
			hints.ai_protocol = 0;				/* Any protocol */
			hints.ai_socktype = SOCK_STREAM;

			addrinfo* res;
			auto ret = ::getaddrinfo(dom.c_str(), NULL, &hints, &res);
			WSACleanup();
			if (ret != 0 || res == nullptr) {
				return NULL;
			}
			IP retipaddr;
			if (f == Family::IPV6) {
				retipaddr = IP(*(struct sockaddr_in6*)(res->ai_addr));
			}
			else {
				retipaddr = IP(*(struct sockaddr_in*)(res->ai_addr));
			}
			freeaddrinfo(res);
			return retipaddr;
		}

	public:
		/// <summary>
		/// IN_ADDR格式的IPV4地址
		/// </summary>
		in_addr InAddr = {};

		/// <summary>
		/// in_addr6格式的IPV6地址
		/// </summary>
		in_addr6 InAddr6 = {};
		/// <summary>
		/// IP协议族 V4/V6
		/// </summary>
		Family IPFamily = Family::IPV4;
	};//class IP


	// class TCPClient
		/// <summary>
	/// 
	/// </summary>
	class TCPClient {
		//using ushort = unsigned short;
		/// <summary>
		/// TCP断开连接时触发的事件
		/// </summary>
		using DisConnectEvent = std::function<void()>;

		/// <summary>
		/// 接收数据时触发的函数类型
		/// </summary>
		/// <param name="ULONG64">本次接收到的数据长度</param>
		/// <param name="const char*">接收到的数据</param>
		using RecvDataEvent = std::function<void(ULONG64, const char*)>;

		/// <summary>
		/// 接收消息时错误回调
		/// </summary>
		/// <param name="int">错误值</param>
		using RecvErrorEvent = std::function<void(int)>;
	public:
		TCPClient() {}
		~TCPClient() {
			if (_sockfd) {
				shutdown(_sockfd, SD_BOTH);
				closesocket(_sockfd);
				WSACleanup();
			}
			Connected = false;
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
			if (Connected) {
				return Error::connecting;
			}

			_family = ip.IPFamily;

			//Initializing the socket library
			WSADATA wsadata; //Define a structure of type WSADATA to store the Windows Sockets data returned by the WSAStartup function call
			WORD sock_version = MAKEWORD(2, 0); //Set the version number
			if (WSAStartup(sock_version, &wsadata)) //Initialize the socket, start the build, and load "ws2_32.lib" into memory
			{
				return Error::initializationWinsockFailed;
			}

			//Create sockets
			if ((_sockfd = socket((ADDRESS_FAMILY)ip.IPFamily, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR) {
				return Error::socketError;
			}

			if (ip.IPFamily == Family::IPV4) {
				memset(&_sock4, 0, sizeof(_sock4));
				_sock4.sin_family = (ADDRESS_FAMILY)ip.IPFamily;
				_sock4.sin_port = htons(port);
				_sock4.sin_addr = ip.InAddr;
				if (::connect(_sockfd, (struct sockaddr*)&_sock4, sizeof(_sock4)) == INVALID_SOCKET) {
					closesocket(_sockfd);
					return Error::connectFailed;
				}
			}
			else {
				memset(&_sock6, 0, sizeof(_sock6));
				_sock6.sin6_family = (ADDRESS_FAMILY)ip.IPFamily;
				_sock6.sin6_port = htons(port);
				_sock6.sin6_addr = ip.InAddr6;
				if (::connect(_sockfd, (struct sockaddr*)&_sock6, sizeof(_sock6)) == INVALID_SOCKET) {
					closesocket(_sockfd);
					return Error::connectFailed;
				}
			}

			Connected = true;

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
		Error connectV4(const std::string& ip, ushort port) {
			//IP ipaddrB(Family::IPV4,ip);
			return connect(ip, port);
		}

		/// <summary>
		/// Specify IPV6 IP for connection
		/// </summary>
		/// <param name="ip">Remote host ipv6 address, such as "2000:0:0:0:0:0:0:0:1"</param>
		/// <param name="port">Remote host port</param>
		/// <returns></returns>
		Error connectV6(const std::string& ip, ushort port) {
			return connect(ip, port);
		}

		/// <summary>
		/// Disconnection
		/// </summary>
		/// <returns></returns>
		Error disConnect() {
			if (!Connected) {
				return Error::noConnected;
			}
			shutdown(_sockfd, SD_BOTH);
			if (closesocket(_sockfd) == 0) {
				Connected = false;
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
			if (!Connected) {
				return Error::noConnected;
			}
			auto textlen = text.length();
			if (textlen > INT_MAX) {
				return Error::dataTooLong;		// 长度不能超过Int的最大值
			}
			int len = static_cast<int>(textlen);
			char* p = text.data();
			while (len > 0) {
				int sendcount = send(_sockfd, p, len, 0);
				if (sendcount < 0) {
					return Error::sendFailed;
				}
				len -= sendcount;
				p += sendcount;
			}
			return Error::noError;
		};

		/// <summary>
		/// Sending bytes
		/// </summary>
		/// <param name="data"></param>
		/// <returns></returns>
		Error sendData(char* data, int size) {
			if (!Connected) {
				return Error::noConnected;
			}
			char* p = data;
			int len = size;
			while (len > 0) {
				int sendcount = send(_sockfd, p, len, 0);
				if (sendcount < 0) {				// 等于0 表示剩余缓冲区空间不足,要等待协议将数据发送完
					return Error::sendFailed;
				}
				len -= sendcount;
				p += sendcount;
			}
			return Error::noError;
		};

		/// <summary>
		/// 设置阻塞模式,在连接前设置 默认为阻塞模式
		/// </summary>
		/// <param name="blocking">是否设置成阻塞模式</param>
		/// <returns></returns>
		Error setBlockingMode(bool blocking) {
			if (!Connected) {
				_blockingmode = blocking;
				return Error::noError;
			}
			return Error::connecting;
		}


		/// <summary>
		/// Register callback events
		/// </summary>
		/// <param name="disConnectEvent"></param>
		void onDisConnect(DisConnectEvent disConnectEvent) {
			_disConnectEvent = disConnectEvent;
		};

		/// <summary>
		/// Register callback events
		/// </summary>
		/// <param name="disConnectEvent"></param>
		void onRecvData(RecvDataEvent recvDataEvent) {
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
			if (c->Connected) {

				char buffer[RecvBuffSize];
				memset(buffer, '\0', RecvBuffSize);

				while (c->Connected) {
					int recvbytecount;
					if ((recvbytecount = recv(c->_sockfd, buffer, RecvBuffSize, 0)) <= 0) {
						//Return 0 Network Outage
						if (recvbytecount == 0) {
							c->Connected = false;
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
						memset(buffer, '\0', RecvBuffSize);
					}
				}//while (c->connected)
			}//if(!c->connected)
		};//static void startRecv(TCPClient* c)

	public:
		static const int RecvBuffSize = 1024;
		bool Connected = false;

	private:
		SOCKET _sockfd = NULL;
		struct sockaddr_in _sock4 = {};
		struct sockaddr_in6 _sock6 = {};

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

	//class TCPServer

	/// <summary>
	/// 
	/// </summary>
	class TCPServer {
	public:
		struct MeetClient
		{
			SOCKET clientSocket;
			IP addr;
			ushort port;
		};

		using NewClientConnectEvent = std::function<void(MeetClient)>;
		using ClientDisConnectEvent = std::function<void(MeetClient)>;
		using RecvDataEvent = std::function<void(MeetClient, ULONG64, const char*)>;
		using RecvErrorEvent = std::function<void(MeetClient, int)>;
	public:
		//TODO 监听地址 监听端口 最大连接数量
		TCPServer() {}
		~TCPServer() {
			_serverRunning = false;
			if (_socket) {
				shutdown(_socket, SD_BOTH);
				closesocket(_socket);
				WSACleanup();
			}
		}
	public:

		/// <summary>
		/// 开始监听设备地址和端口
		/// </summary>
		/// <param name="listenAddress">监听的IP地址</param>
		/// <param name="listenPort">监听的端口</param>
		/// <param name="maxConnectCount">允许的最大客户端连接数量</param>
		/// <returns></returns>
		Error Listen(IP listenAddress, ushort listenPort, int maxConnectCount) {
			if (_serverRunning) {
				return Error::serverIsStarted;
			}

			_listenAddr = listenAddress;
			_listenPort = listenPort;
			_maxCount = maxConnectCount;

			if (WSAStartup(_versionRequested, &_wsaDat) != 0) {
				return Error::initializationWinsockFailed;
			}

			// 分IP协议 对本地地址进行绑定
			{
				if (_listenAddr.IPFamily == Family::IPV4) {
					memset(&_sock4, 0, sizeof(struct sockaddr_in));
					_sock4.sin_family = (ADDRESS_FAMILY)_listenAddr.IPFamily;
					_sock4.sin_port = htons(_listenPort);
					_sock4.sin_addr = _listenAddr.InAddr;
					_socket = socket((ADDRESS_FAMILY)_listenAddr.IPFamily, SOCK_STREAM, IPPROTO_TCP);
					if (_socket == INVALID_SOCKET) {
						return Error::socketError;
					}
					if (::bind(_socket, (sockaddr*)&_sock4, sizeof(struct sockaddr_in)) == SOCKET_ERROR) {
						return Error::bindError;
					}
				}
				else {
					memset(&_sock6, 0, sizeof(struct sockaddr_in6));
					_sock6.sin6_family = (ADDRESS_FAMILY)_listenAddr.IPFamily;
					_sock6.sin6_port = htons(_listenPort);
					_sock6.sin6_addr = _listenAddr.InAddr6;
					_socket = socket((ADDRESS_FAMILY)_listenAddr.IPFamily, SOCK_STREAM, IPPROTO_TCP);
					if (_socket == INVALID_SOCKET) {
						return Error::socketError;
					}
					if (::bind(_socket, (sockaddr*)&_sock6, sizeof(struct sockaddr_in6)) == SOCKET_ERROR) {
						return Error::bindError;
					}
				}
			}

			// 系统API建立监听,这里第二个参数表示在建立握手时队列最多可等待的连接数
			if (::listen(_socket, MEET_LISTEN_BACKLOG) == SOCKET_ERROR) {
				return Error::listenError;
			}

			_serverRunning = true;

			if (!_blockingMode) {
				u_long iMode = 1;
				::ioctlsocket(_socket, FIONBIO, &iMode);
			}

			//创建一个线程 接收客户端的连接
			auto _connect_thread = std::thread([this]() {
				while (_serverRunning) {

					SOCKET c_socket;
					IP clientAddress;
					ushort clientPort = 0;
					if (_listenAddr.IPFamily == Family::IPV4) {
						struct sockaddr_in remoteAddr = {};
						int nAddrlen = sizeof(remoteAddr);
						c_socket = ::accept(_socket, (sockaddr*)&remoteAddr, &nAddrlen); //默认应该会在这里阻塞
						if (c_socket == INVALID_SOCKET) {   //无效的套接字
							continue;
						}
						clientAddress = IP(remoteAddr);
						clientPort = remoteAddr.sin_port;
					}
					else {
						struct sockaddr_in6 remoteAddr = {};
						int nAddrlen = sizeof(remoteAddr);
						c_socket = ::accept(_socket, (sockaddr*)&remoteAddr, &nAddrlen); //默认应该会在这里阻塞
						if (c_socket == INVALID_SOCKET) {   //无效的套接字
							continue;
						}
						clientAddress = IP(remoteAddr);
						clientPort = remoteAddr.sin6_port;
					}

					if (clientList.size() < _maxCount) {

						MeetClient newClient = { .clientSocket = c_socket, .addr = clientAddress , .port = clientPort };
						clientList.push_back(newClient);

						//创建线程 监听客户端传来的消息
						auto _recv_thread = std::thread([this, newClient]() {
							char buffer[1024];
							memset(buffer, '\0', sizeof(buffer));
							while (_serverRunning) {
								int recvbytecount;
								if ((recvbytecount = ::recv(newClient.clientSocket, buffer, sizeof(buffer), 0)) <= 0) {
									//Return 0 Network Outage
									if (recvbytecount == 0) {
										removeClientFromClientList(newClient.addr, newClient.port);
										if (_onClientDisConnectEvent != NULL) {
											_onClientDisConnectEvent(newClient);
										}
										break;
									}

									//阻塞模式
									if (_blockingMode) {
										if (recvbytecount == SOCKET_ERROR) {
											if (_recvErrorEvent != NULL) {
												_recvErrorEvent(newClient, recvbytecount);
											}
										}
									}

								}//if ((recvbytecount = recv(c->sockfd, buffer, sizeof(buffer), 0)) <= 0)

								//接收数据 触发回调
								else {
									if (_recvDataEvent != NULL) {
										_recvDataEvent(newClient, recvbytecount, buffer);
									}
									memset(buffer, '\0', sizeof(buffer));
								}
							}
							});
						_recv_thread.detach();

						//触发有客户端连接的回调
						if (_onNewClientConnectEvent != NULL) {
							_onNewClientConnectEvent(newClient);
						}

					}
					else {
						// 连接总数超过限制 断开这个客户端的连接
						shutdown(c_socket, SD_BOTH);
						closesocket(c_socket);
					}
				}
				});
			_connect_thread.detach();
			return Error::noError;
		}

	public:

		/// <summary>
		/// 设置阻塞模式,在连接前设置 默认为阻塞模式
		/// </summary>
		/// <param name="blocking">是否设置成阻塞模式</param>
		/// <returns></returns>
		Error setBlockingMode(bool blocking) {
			if (!_serverRunning) {
				_blockingMode = blocking;
				return Error::noError;
			}
			return Error::serverIsStarted;
		}

		/// <summary>
		/// 从客户端列表中移除客户端信息
		/// </summary>
		/// <param name="addr"></param>
		/// <param name="port"></param>
		/// <returns></returns>
		Error removeClientFromClientList(IP addr, ushort port) {
			for (auto it = clientList.begin(); it != clientList.end(); it++) {
				if ((*it).addr.toString() == addr.toString() && (*it).port == port) {		// 条件语句
					// disClientConnect 方法内部会调用这个函数
					clientList.erase(it);		// 移除他
					it--;		                // 让该迭代器指向前一个
					return Error::noError;
				}
			}
			return Error::noFoundClient;
		}



		/// <summary>
		/// 断开客户端的连接
		/// </summary>
		/// <param name="addr"></param>
		/// <param name="port"></param>
		/// <returns></returns>
		Error disClientConnect(IP addr, ushort port) {
			if (!_serverRunning) {
				return Error::serverNotStarted;
			}
			for (auto& c : clientList) {
				if (addr.toString() == c.addr.toString() && port == c.port) {
					shutdown(c.clientSocket, SD_BOTH);
					if (closesocket(c.clientSocket) == 0) {
						removeClientFromClientList(addr, port);
						//循环 
						return Error::noError;
					}
					return Error::unkError;
				}
			}
			return Error::noFoundClient;
		};


		/// <summary>
		/// 注册客户端连接时事件
		/// </summary>
		/// <param name="onNewClientConnectEvent"></param>
		void onNewClientConnect(NewClientConnectEvent onNewClientConnectEvent) {
			_onNewClientConnectEvent = onNewClientConnectEvent;
		};

		/// <summary>
		/// 注册客户端断开连接时事件
		/// </summary>
		/// <param name="onClientDisConnectEvent"></param>
		void onClientDisConnect(ClientDisConnectEvent onClientDisConnectEvent) {
			_onClientDisConnectEvent = onClientDisConnectEvent;
		};

		/// <summary>
		/// 注册接收到客户端消息时回调
		/// </summary>
		/// <param name="recvDataEvent"></param>
		void onRecvData(RecvDataEvent recvDataEvent) {
			_recvDataEvent = recvDataEvent;
		}

		/// <summary>
		/// 阻塞模式 接收客户端消息时错误回调
		/// </summary>
		/// <param name="recvErrorEvent"></param>
		void onRecvError(RecvErrorEvent recvErrorEvent) {
			_recvErrorEvent = recvErrorEvent;
		}

	public:

		/// <summary>
		///  Send text
		/// </summary>
		/// <param name="text">Text to be sent</param>
		/// <returns></returns>
		Error sendText(SOCKET socket, std::string text) {
			if (!_serverRunning) {
				return Error::serverNotStarted;
			}
			auto textlen = text.length();
			if (textlen > INT_MAX) {
				return Error::dataTooLong;		// 长度不能超过Int的最大值
			}
			int len = static_cast<int>(textlen);
			char* p = text.data();
			while (len > 0) {
				int sendcount = send(socket, p, len, 0);
				if (sendcount < 0) {
					return Error::sendFailed;
				}
				len -= sendcount;
				p += sendcount;
			}
			return Error::noError;
		};

		/// <summary>
		/// Sending bytes
		/// </summary>
		/// <param name="data"></param>
		/// <returns></returns>
		Error sendData(SOCKET socket, char* data, int size) {
			if (!_serverRunning) {
				return Error::serverNotStarted;
			}
			char* p = data;
			int len = size;
			while (len > 0) {
				int sendcount = send(socket, p, len, 0);
				if (sendcount < 0) {
					return Error::sendFailed;
				}
				//printf("发送的字节数:%d\n", sendcount);
				len -= sendcount;
				p += sendcount;
			}
			return Error::noError;
		};

		/// <summary>
		/// 获取所有已连接的客户端
		/// </summary>
		/// <returns></returns>
		std::vector<MeetClient> GetALLClient() {
			return clientList;
		}


	protected:
	private:
	private:
		/// <summary>
		/// 监听地址
		/// </summary>
		IP _listenAddr;
		/// <summary>
		/// 监听端口
		/// </summary>
		ushort _listenPort = 0;
		/// <summary>
		/// 最大客户端连接数量
		/// </summary>
		int _maxCount = MEET_LISTEN_DEFAULT_MAXCONNECT;

		WSADATA _wsaDat;
		ushort _versionRequested = MAKEWORD(2, 2);

		struct sockaddr_in _sock4 = {};
		struct sockaddr_in6 _sock6 = {};

		/// <summary>
		/// 服务端监听会话
		/// </summary>
		SOCKET _socket;

		/// <summary>
		/// 接收消息阻塞模式
		/// </summary>
		bool _blockingMode = true;

		/// <summary>
		/// 连接的客户端列表
		/// </summary>
		std::vector<MeetClient> clientList;

		/// <summary>
		/// 标志，防止重复初始化， 服务端是否启动
		/// </summary>
		bool _serverRunning = false;

		// 回调
		NewClientConnectEvent _onNewClientConnectEvent = NULL;
		ClientDisConnectEvent _onClientDisConnectEvent = NULL;
		RecvDataEvent _recvDataEvent = NULL;
		RecvErrorEvent _recvErrorEvent = NULL;
	};//class TCPServer

}//namespace meet