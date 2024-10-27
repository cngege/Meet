#ifndef MEETLIB
#define MEETLIB


#pragma once

#include <vector>
#include <string>
#include <thread>
#include <functional>
#include <shared_mutex>	// 读写锁

#include <WinSock2.h>
#include <WS2tcpip.h>

#ifndef EXTERNAL_LINKWS2_32LIB
#pragma comment(lib,"ws2_32.lib")
#endif // !EXTERNAL_LINKWS2_32LIB

// 表示在建立握手时队列最多可等待的连接数
#ifndef MEET_LISTEN_BACKLOG
#define MEET_LISTEN_BACKLOG 5
#endif // !MEET_LISTEN_BACKLOG

// 默认的最大连接数 在没有设置的时候
#ifndef MEET_LISTEN_DEFAULT_MAXCONNECT
#define MEET_LISTEN_DEFAULT_MAXCONNECT -1
#endif // !MEET_LISTEN_DEFAULT_MAXCONNECT


namespace meet
{
	/// <summary>
	/// IP地址类型
	/// </summary>
	enum class Family {
		UNSPEC = AF_UNSPEC,
		IPV4 = AF_INET,
		IPV6 = AF_INET6
	};

	enum class Error : int {
		initializationWinsockFailed = -16,	// 初始化 Winsock 失败
		unkError = -1,
		noError = 0,
		uninitialized,
		inited,
		ipInvalid,
		unFamily,
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
		case Error::initializationWinsockFailed:
		{
			return "initializationWinsockFailed";
		}
		case Error::unkError:
		{
			return "unkError";
		}
		case Error::noError:
		{
			return "noError";
		}
		case Error::uninitialized:
		{
			return "uninitialized";
		}
		case Error::inited:
		{
			return "inited";
		}
		case Error::ipInvalid:
		{
			return "ipInvalid";
		}
		case Error::unFamily:
		{
			return "unFamily";
		}
		case Error::socketError:
		{
			return "socketError";
		}
		case Error::bindError:
		{
			return "bindError";
		}
		case Error::listenError:
		{
			return "listenError";
		}
		case Error::acceptError:
		{
			return "acceptError";
		}
		case Error::sendFailed:
		{
			return "sendFailed";
		}
		case Error::recvFailed:
		{
			return "recvFailed";
		}
		case Error::portTooSmall:
		{
			return "portTooSmall";
		}
		case Error::maxcouTooBig:
		{
			return "maxcouTooBig";
		}
		case Error::connectFailed:
		{
			return "connectFailed";
		}
		case Error::unsupportedOperations:
		{
			return "unsupportedOperations";
		}
		case Error::connecting:
		{
			return "connecting";
		}
		case Error::noConnected:
		{
			return "noConnected";
		}
		case Error::changeError:
		{
			return "changeError";
		}
		case Error::theMaximumNumberOfConnectionsHasBeenReached:
		{
			return "theMaximumNumberOfConnectionsHasBeenReached";
		}
		case Error::serverIsStarted:
		{
			return "serverIsStarted";
		}
		case Error::serverNotStarted:
		{
			return "serverNotStarted";
		}
		case Error::noFoundClient:
		{
			return "noFoundClient";
		}
		case Error::dataTooLong:
		{
			return "dataTooLong";
		}
		default:
		{
			return "Error! unk errorCode!";
		}
		}
	}//static std::string getString(Error errorCode)

#pragma region IP
	/// <summary>
	/// 
	/// </summary>
	class IP
	{
	public:
		IP(hostent* host) {
			// 做 IP ip = NULL 兼容
			if (host == NULL) {
				_valid = false;
			}
			else {
				if (host->h_addrtype == AF_INET) {
					IPFamily = Family::IPV4;
					memcpy(&InAddr, host->h_addr, host->h_length);
				}
				else if (host->h_addrtype == AF_INET6) {
					IPFamily = Family::IPV6;
					memcpy(&InAddr6, host->h_addr, host->h_length);
				}
				_valid = true;
			}
		};

		/**
		 * @brief IP?域名? 直接转化,并识别V4还是V6
		 * @param ipdom IP或域名
		*/
		IP(const std::string& ipdom) {
			auto ips = IP::getaddrinfo(ipdom);
			if (ips.size()) {
				this->InAddr = ips[0].InAddr;
				this->InAddr6 = ips[0].InAddr6;
				this->IPFamily = ips[0].IPFamily;
				this->_valid = ips[0]._valid;
			}
			else {
				this->_valid = false;
			}
		}

		IP(const char* ipdom) : IP(std::string(ipdom)) {}

		IP(IN_ADDR inaddr) {
			IPFamily = Family::IPV4;
			InAddr = inaddr;
			_valid = true;
		}

		IP(in_addr6 inaddr) {
			IPFamily = Family::IPV6;
			InAddr6 = inaddr;
			_valid = true;
		}

		// 需要研究下如果是ipv6那么传进来的数据会不会丢失
		IP(sockaddr* addr) {
			if (addr->sa_family == (ADDRESS_FAMILY)Family::IPV4) {
				IPFamily = Family::IPV4;
				InAddr = ((sockaddr_in*)addr)->sin_addr;
			}
			else 
			{
				IPFamily = Family::IPV6;
				InAddr6 = ((sockaddr_in6*)addr)->sin6_addr;
			}
			_valid = true;
		}

		IP(sockaddr_storage* addr) {
			if (addr->ss_family == (ADDRESS_FAMILY)Family::IPV4) {
				IPFamily = Family::IPV4;
				InAddr = ((sockaddr_in*)addr)->sin_addr;
			}
			else {
				IPFamily = Family::IPV6;
				InAddr6 = ((sockaddr_in6*)addr)->sin6_addr;
			}
			_valid = true;
		}

		IP(){
			_valid = false;
		}
	public:
		void Clear() {
			_valid = false;
		}
	public:
	public:

		/**
		 * @brief 将IP地址转为字符串输出
		 * @return 
		*/
		std::string toString() const {
			if (!_valid) {
				return std::string();
			}
			if (IPFamily == Family::IPV4) {
				char IPStrBuf[INET_ADDRSTRLEN] = { '\0' };
				return std::string(inet_ntop(AF_INET, (void*)&InAddr, IPStrBuf, sizeof(IPStrBuf)));
			}
			else {
				char IPStrBuf[INET6_ADDRSTRLEN] = { '\0' };
				return std::string(inet_ntop(AF_INET6, (void*)&InAddr6, IPStrBuf, sizeof(IPStrBuf)));
			}
		}

		/**
		 * @brief 随参数附加一个端口 转为SOCK用地址,使用这个方法请充分考虑到sockaddr生存周期,sockaddr长度不能装下IPv6的问题
		 * @param saddr 可以是一个空的sockaddr指针
		 * @param port 端口,非必须
		 * @return 
		*/
		/*sockaddr* toSockaddr(sockaddr* saddr, u_short port = 0) {
			if (_valid) {
				saddr->sa_family = (ADDRESS_FAMILY)Family::IPV4;
				if (isIPv4()) {
					((sockaddr_in*)saddr)->sin_family = (ADDRESS_FAMILY)Family::IPV4;
					((sockaddr_in*)saddr)->sin_addr = InAddr;
					if (port != 0) {
						((sockaddr_in*)saddr)->sin_port = htons(port);
					}
				}
				else if(isIPv6()) {
					// 如果变量的实际类型是sockaddr,而isIPv6() 为true 则这里会出大问题，因为sockaddr的大小不够容纳
					((sockaddr_in6*)saddr)->sin6_family = (ADDRESS_FAMILY)Family::IPV6;
					((sockaddr_in6*)saddr)->sin6_addr = InAddr6;
					if (port != 0) {
						((sockaddr_in6*)saddr)->sin6_port = htons(port);
					}
				}
			}
			return saddr;
		}*/

		std::shared_ptr<sockaddr_storage> toSockaddr(u_short port = 0) {
			if (_valid) {
				//saddr->sa_family = (ADDRESS_FAMILY)Family::IPV4;
				if (isIPv4()) {
					//std::shared_ptr<SOCKADDR_IN> saddr = std::make_shared<SOCKADDR_IN>();
					SOCKADDR_IN* saddr = new SOCKADDR_IN;
					saddr->sin_family = (ADDRESS_FAMILY)Family::IPV4;
					saddr->sin_addr = InAddr;
					if (port != 0) {
						saddr->sin_port = htons(port);
					}
					return std::shared_ptr<sockaddr_storage>((sockaddr_storage*)saddr);
				}
				else if (isIPv6()) {
					//std::shared_ptr<SOCKADDR_IN6> saddr = std::make_shared<SOCKADDR_IN6>();
					SOCKADDR_IN6* saddr = new SOCKADDR_IN6;
					saddr->sin6_family = (ADDRESS_FAMILY)Family::IPV6;
					saddr->sin6_addr = InAddr6;
					if (port != 0) {
						saddr->sin6_port = htons(port);
					}
					return std::shared_ptr<sockaddr_storage>((sockaddr_storage*)saddr);
				}
			}
			return NULL;
		}

		sockaddr_storage* toSockaddr(sockaddr_storage* saddr, u_short port = 0) {
			if (_valid) {
				saddr->ss_family = (ADDRESS_FAMILY)Family::IPV4;
				if (isIPv4()) {
					((sockaddr_in*)saddr)->sin_family = (ADDRESS_FAMILY)Family::IPV4;
					((sockaddr_in*)saddr)->sin_addr = InAddr;
					if (port != 0) {
						((sockaddr_in*)saddr)->sin_port = htons(port);
					}
				}
				else if (isIPv6()) {
					((sockaddr_in6*)saddr)->sin6_family = (ADDRESS_FAMILY)Family::IPV6;
					((sockaddr_in6*)saddr)->sin6_addr = InAddr6;
					if (port != 0) {
						((sockaddr_in6*)saddr)->sin6_port = htons(port);
					}
				}
			}
			return saddr;
		}

		int toSockaddrLen() {
			return (this->IPFamily == Family::IPV4) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6);
		}

		/**
		 * @brief 该IP是否有效 比如由域名转化的IP,或用户输入的IP后应当校验
		 * @return 
		*/
		bool isValid() {
			return _valid;
		}

		bool isIPv4() {
			return this->IPFamily == Family::IPV4;
		}

		bool isIPv6() {
			return this->IPFamily == Family::IPV6;
		}

	public:

		/**
		 * @brief 尝试将域名转为IP地址 一个域名(主机名)可能解析为多个IP
		 * @param dom 域名
		 * @return 
		*/
		static std::vector<IP> getaddrinfo(const std::string& dom) {
			std::vector<IP> ips;
			WSADATA wsadata; //Define a structure of type WSADATA to store the Windows Sockets data returned by the WSAStartup function call
			if (WSAStartup(MAKEWORD(2, 0), &wsadata)) //Initialize the socket, start the build, and load "ws2_32.lib" into memory
			{
				return ips;
			}
			addrinfo hints;
			memset(&hints, 0, sizeof(addrinfo));
			hints.ai_family = AF_UNSPEC;			/* Allow IPv4 V6 */ // 自动识别
			//hints.ai_flags = AI_CANONNAME;		/* For wildcard IP address */
			hints.ai_flags = AI_ALL;				/* For wildcard IP address */
			hints.ai_protocol = 0;					/* Any protocol */ // IPPROTO_TCP、IPPROTO_UDP
			hints.ai_socktype = 0;					/* ALL */

			addrinfo* res;
			auto ret = ::getaddrinfo(dom.c_str(), NULL, &hints, &res);
			WSACleanup();
			if (ret != 0) {
				return ips;
			}
			for (addrinfo* chires = res; chires != NULL; chires = chires->ai_next) {
				ips.push_back(IP(chires->ai_addr));
			}
			freeaddrinfo(res);
			return ips;
		}

		//IP& operator=(const IP& ip) {
		//	if (this == &ip) {
		//		return *this;
		//	}
		//	this->InAddr = ip.InAddr;
		//	this->InAddr6 = ip.InAddr6;
		//	this->IPFamily = ip.IPFamily;
		//	this->_valid = ip._valid;
		//	return *this;
		//}
		IP& operator=(const IP& copy) {
			if (this == &copy) {
				return *this;
			}
			this->IPFamily = copy.IPFamily;
			if (copy._valid) {
				if (copy.IPFamily == Family::IPV4) {
					this->InAddr = copy.InAddr;
				}
				else {
					this->InAddr6 = copy.InAddr6;
				}
			}
			this->_valid = copy._valid;
			return *this;
		}

		bool operator==(const IP& ip) const {
			if (this->_valid != ip._valid || this->IPFamily != ip.IPFamily) {
				return false;
			}
			if (this->toString() != ip.toString()) {
				return false;
			}
			return true;
		}

	public:
		/**
		 * @brief IN_ADDR格式的IPV4地址
		*/
		in_addr InAddr = {};

		/**
		 * @brief in_addr6格式的IPV6地址
		*/
		in_addr6 InAddr6 = {};

		/**
		 * @brief IP协议族 V4&V6
		*/
		Family IPFamily = Family::IPV4;

	private:
		/**
		 * @brief 该IP是否有效 比如由域名转化的IP,或用户输入的IP后应当校验
		*/
		bool _valid = false;
	};//class IP
#pragma endregion

#pragma region TCPClient

	// class TCPClient
	class TCPClient {
		//using ushort = unsigned short;
		
		/**
		 * @brief TCP断开连接时触发的事件
		*/
		using DisConnectEvent = std::function<void()>;

		/**
		 * @brief 接收数据时触发的函数类型
		 * @param ULONG64 本次接收到的数据长度
		 * @param const char* 接收到的数据
		*/
		using RecvDataEvent = std::function<void(ULONG64, const char*)>;

		/**
		 * @brief 接收消息时错误回调
		 * @param int 错误值
		*/
		using RecvErrorEvent = std::function<void(int)>;
	public:
		TCPClient() {}
		/**
		 * @brief 如果手动调用析构函数释放了,那就不要再用这个类是,会出问题的
		*/
		~TCPClient() {
			close();
		}
	public:
		/**
		 * @brief 关闭套接字连接
		*/
		void close() {
			Connected = false;
			if (_sockfd) {
				shutdown(_sockfd, SD_BOTH);
				closesocket(_sockfd);
				WSACleanup();
				_sockfd = NULL;
			}
		}

	public:

		/**
		 * @brief 设置接收数据时数据缓冲区的大小
		 * @param size 
		 * @return [Error::connecting / Error::noError]
		*/
		Error setRecvBufferSize(int size) {
			if (Connected) {
				return Error::connecting;
			}
			_recvBuffSize = size;
			return Error::noError;
		}

		/**
		 * @brief 获取已经设置的接收数据的数据缓冲区的大小
		 * @return 
		*/
		int getRecvBufferSize() {
			return _recvBuffSize;
		}

		/**
		 * @brief 现在是否是已经连接
		 * @return 
		*/
		bool isConnected() {
			return Connected;
		}


	public:

		/**
		 * @brief To connect to a remote host, you must connect without establishing a connection
		 * @param ip host
		 * @param port port
		 * @return [Error::connecting / Error::initializationWinsockFailed / Error::socketError / Error::connectFailed / Error::noError]
		*/
		Error connect(IP ip, u_short port) {
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
			
			//if (ip.IPFamily == Family::IPV4) {
			//	memset(&_sock4, 0, sizeof(_sock4));
			//	_sock4.sin_family = (ADDRESS_FAMILY)ip.IPFamily;
			//	_sock4.sin_port = htons(port);
			//	_sock4.sin_addr = ip.InAddr;
			//	if (::connect(_sockfd, (struct sockaddr*)&_sock4, sizeof(_sock4)) == INVALID_SOCKET) {
			//		closesocket(_sockfd);
			//		return Error::connectFailed;
			//	}
			//}
			//else {
			//	memset(&_sock6, 0, sizeof(_sock6));
			//	_sock6.sin6_family = (ADDRESS_FAMILY)ip.IPFamily;
			//	_sock6.sin6_port = htons(port);
			//	_sock6.sin6_addr = ip.InAddr6;
			//	if (::connect(_sockfd, (struct sockaddr*)&_sock6, sizeof(_sock6)) == INVALID_SOCKET) {
			//		closesocket(_sockfd);
			//		return Error::connectFailed;
			//	}
			//}
			
			// 这里没有判断而直接使用 sockaddr_in6 是因为 sockaddr能装得下v4 但装不下v6 故直接使用v6(虽然在使用v4连接的时候会浪费一点内存)
			//sockaddr_in6 saddr{};
			// 这里使用 sockaddr_storage 而不是使用 sockaddr ，因为前者的结构大小足够容纳IPv4 和 IPv6
			sockaddr_storage saddr{};
			if (::connect(_sockfd, (sockaddr*)ip.toSockaddr(&saddr, port), ip.toSockaddrLen()) == INVALID_SOCKET) {
				closesocket(_sockfd);
				return Error::connectFailed;
			}

			Connected = true;
			// Set the connection to non-blocking mode recv return in time
			if (!_blockingMode) {
				u_long iMode = 1;
				::ioctlsocket(_sockfd, FIONBIO, &iMode);
			}

			///Open a thread Triggers a listening event after receiving and processing a message
			_recv_thread = std::thread(&TCPClient::startRecv, this);
			_recv_thread.detach();
			return Error::noError;
		};

		/**
		 * @brief 使用Ipv4协议进行连接 / Specify IPV4 IP for connection
		 * @param ip 远程主机的ipv4地址, 比如: "127.0.0.1" / Remote host ipv4 address, e.g. "127.0.0.1"
		 * @param port 远程主机端口 / Remote host port
		 * @return 
		*/
		Error connectV4(const std::string& ip, u_short port) {
			//IP ipaddrB(Family::IPV4,ip);
			return connect(ip, port);
		}

		/**
		 * @brief 使用Ipv6协议进行连接 / Specify IPV6 IP for connection 
		 * @param ip 远程主机的ipv6地址, 比如: "2000:0:0:0:0:0:0:0:1" / Remote host ipv6 address, such as "2000:0:0:0:0:0:0:0:1"
		 * @param port 远程主机端口 / Remote host port
		 * @return 
		*/
		Error connectV6(const std::string& ip, u_short port) {
			return connect(ip, port);
		}

		/**
		 * @brief 断开同服务端的连接
		 * @return 是否有错误 [Error::noConnected] / [Error::noError] / [Error::unkError]
		*/
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

		/**
		 * @brief 发送字符串 / Send text
		 * @param text 待发送的文本 / Text to be sent
		 * @return 是否有错误 [Error::noConnected] / [Error::dataTooLong] / [Error::noError] / [Error::sendFailed]
		*/
		Error sendText(std::string text) {
			if (!Connected) {
				return Error::noConnected;
			}
			auto textlen = text.length();
			if (textlen > INT_MAX) {
				return Error::dataTooLong;		// 长度不能超过Int的最大值
			}
			int len = static_cast<int>(textlen);
			char* p = const_cast<char*>(text.data());
			while (len > 0) {
				int sendcount = ::send(_sockfd, p, len, 0);
				if (sendcount < 0) {
					return Error::sendFailed;
				}
				len -= sendcount;
				p += sendcount;
			}
			return Error::noError;
		};

		/**
		 * @brief 发送字节 / Sending bytes
		 * @param data 要发送的字节数据
		 * @param size 数据长度
		 * @return 是否有错误 [Error::noConnected] / [Error::noError] / [Error::sendFailed]
		*/
		Error sendData(char* data, int size) {
			if (!Connected) {
				return Error::noConnected;
			}
			char* p = data;							// 数据由前向后发送,发送后指针右移
			int len = size;							// 剩余要发送数据的大小
			while (len > 0) {
				int sendcount = ::send(_sockfd, p, len, 0);
				if (sendcount < 0) {				// 等于0 表示剩余缓冲区空间不足,要等待协议将数据发送完
					return Error::sendFailed;
				}
				len -= sendcount;
				p += sendcount;
			}
			return Error::noError;
		};

		/**
		 * @brief 设置阻塞模式,在连接前设置 默认为阻塞模式
		 * @param blocking true阻塞模式
		 * @return 是否有错误 [Error::connecting] / [Error::noError]
		*/
		Error setBlockingMode(bool blocking) {
			if (!Connected) {
				_blockingMode = blocking;
				return Error::noError;
			}
			return Error::connecting;
		}

		/**
		 * @brief 是否是阻塞模式
		 * @return
		*/
		bool isBlockingMode() {
			return _blockingMode;
		}

		/**
		 * @brief 注册回调事件 / Register callback events
		 * @param disConnectEvent 
		*/
		void onDisConnect(DisConnectEvent disConnectEvent) {
			_disConnectEvent = disConnectEvent;
		};

		/**
		 * @brief 注册回调事件 / Register callback events
		 * @param recvDataEvent
		*/
		void onRecvData(RecvDataEvent recvDataEvent) {
			_recvDataEvent = recvDataEvent;
		}

		/**
		 * @brief 注册回调事件 / Register callback events
		 * @param recvErrorEvent
		*/
		void onRecvError(RecvErrorEvent recvErrorEvent) {
			_recvErrorEvent = recvErrorEvent;
		}

	public:

		/**
		 * @brief 线程函数,开启一个循环接受网络包,并对相关状态进行分析 / Thread function, responsible for receiving network packets, and classify and analyze the processing
		 * @param c TCP客户端 / TCPClient Instance
		*/
		void startRecv() {
			if (Connected) {

				char* buffer = new char[_recvBuffSize];
				//char buffer[RecvBuffSize];
				memset(buffer, '\0', _recvBuffSize);
				while (Connected) {
					int recvbytecount;
					if ((recvbytecount = recv(_sockfd, buffer, _recvBuffSize - 1, 0)) <= 0) {
						//Return 0 Network Outage
						if (recvbytecount == 0) {
							Connected = false;
							if (_disConnectEvent != NULL) {
								_disConnectEvent();
							}
							break;
						}

						//阻塞模式
						if (_blockingMode) {
							if (recvbytecount == SOCKET_ERROR) {
								if (_recvErrorEvent != NULL) {
									_recvErrorEvent(recvbytecount);
								}
							}
						}

					}//if ((recvbytecount = recv(sockfd, buffer, sizeof(buffer), 0)) <= 0)

					//接收数据 触发回调
					else {
						if (_recvDataEvent != NULL) {
							buffer[recvbytecount] = '\0';
							_recvDataEvent(recvbytecount, buffer);
						}
					}
				}//while (connected)
				delete[] buffer;
			}//if(!connected)
		};//void startRecv()

	public:

		bool Connected = false;

	private:
		SOCKET _sockfd = NULL;
		//struct sockaddr_in _sock4 = {};
		//struct sockaddr_in6 _sock6 = {};

		bool _blockingMode = true;
		Family _family = Family::IPV4;

		std::thread _recv_thread;

		int _recvBuffSize = 1024;

		/**
		 * @brief 断开连接时触发的事件
		*/
		DisConnectEvent _disConnectEvent = NULL;

		/**
		 * @brief 有数据达到时自动触发的事件
		*/
		RecvDataEvent _recvDataEvent = NULL;

		/**
		 * @brief recv线程接收消息时出错触发事件
		*/
		RecvErrorEvent _recvErrorEvent = NULL;
	};//class TCPClient
#pragma endregion

#pragma region TCPServer
	//class TCPServer
	class TCPServer {
	public:
		struct MeetClient
		{
		private:
			SOCKET _clientSocket;
			IP _addr;
			u_short _port;
			TCPServer* _server;
			bool _waitDisconnect = false;
		public:
			MeetClient(TCPServer* ser, SOCKET socket, IP address, u_short port) : _server(ser){
				_clientSocket = socket;
				_addr = address;
				_port = port;
			}

			/**
			 * @brief 获取客户端会话接口
			*/
			SOCKET getSocket() const {
				return _clientSocket;
			}

			/**
			 * @brief 获取客户端IP
			*/
			IP getIp() const {
				return _addr;
			}

			/**
			 * @brief 获取客户端端口
			*/
			u_short getPort() const {
				return _port;
			}

			/**
			 * @brief 是否要在循环线程中移除这个客户端
			*/
			bool hasDisconnectTag() {
				return _waitDisconnect;
			}

			/**
			 * @brief 设置标志 在循环线程中移除此客户端
			*/
			void setDisconnectTag() {
				_waitDisconnect = true;
			}

			bool operator==(const MeetClient& c) const {
				return this->_server == c._server && this->_addr == c._addr && this->_port == c._port;
			}

			/**
			 * @brief 断开这个客户端的连接
			*/
			Error disConnect() {
				_waitDisconnect = true;
				return _server->disClientConnect(_addr, _port);
			}

			/**
			 * @brief 客户端信息转为字符串
			*/
			std::string toString() const {
				return _addr.toString() + ":" + std::to_string(_port);
			}

		};

		using NewClientConnectEvent = std::function<void(MeetClient)>;
		using ClientDisConnectEvent = std::function<void(MeetClient)>;
		using RecvDataEvent = std::function<void(MeetClient, ULONG64, const char*)>;
		using RecvErrorEvent = std::function<void(MeetClient, int)>;
	public:
		//TODO 监听地址 监听端口 最大连接数量
		TCPServer() {}

		/**
		 * @brief 如果手动调用析构函数释放了,那就不要再用这个类是,会出问题的
		*/
		~TCPServer() {
			close();
		}

		//TCPServer(TCPServer& tcpserver) {

		//}
	public:

		/**
		 * @brief 关闭套接字,停止TCP服务的运行
		*/
		void close() {
			_serverRunning = false;
			if (_socket) {
				shutdown(_socket, SD_BOTH);
				closesocket(_socket);
				WSACleanup();
				_socket = NULL;
			}
			if (_data_buffer) {
				delete[] _data_buffer;
			}
		}
	public:

		/**
		 * @brief 设置 服务端可允许最大客户端连接数
		 * @param count 连接数量
		 * @return 是否有错误 [Error::serverIsStarted / Error::noError]
		*/
		Error setMaxConnectCount(int count) {
			if (_serverRunning) {					// 暂时不允许在监听之后设置,后续加入如果设置的连接数小于现有的连接数,则断开多余的连接
				return Error::serverIsStarted;
			}
			_maxCount = count;
			return Error::noError;
		}

		/**
		 * @brief 获取允许最大的客户端数量
		 * @return 
		*/
		int getMaxConnectCount() {
			return _maxCount;
		}

		/**
		 * @brief 设置服务端的监听地址
		 * @param address 监听地址比如 0.0.0.0 / 127.0.0.0 / ::
		 * @return 是否有错误 [Error::serverIsStarted / Error::noError]
		*/
		Error setListenAddress(const std::string& address) {
			if (_serverRunning) {
				return Error::serverIsStarted;
			}
			_listenAddr = address;					// 因为构造函数中有string 所以可以隐式转换
			return Error::noError;
		}

		/**
		 * @brief 获取监听地址
		 * @return 
		*/
		IP getListenAddress() {
			return _listenAddr;
		}

		/**
		 * @brief 获取监听端口
		 * @return 
		*/
		u_short getListenPort() {
			return _listenPort;
		}

		/**
		 * @brief 服务端是否正在运行
		 * @return 是否正在监听
		*/
		bool isRunning() {
			return _serverRunning;
		}

		/**
		 * @brief 设置接收数据时数据缓冲区的大小
		 * @param size
		 * @return [Error::serverIsStarted / Error::noError]
		*/
		Error setRecvBufferSize(int size) {
			if (_serverRunning) {
				return Error::serverIsStarted;
			}
			_recvBuffSize = size;
			return Error::noError;
		}

		/**
		 * @brief 获取已经设置的接收数据的数据缓冲区的大小
		 * @return
		*/
		int getRecvBufferSize() {
			return _recvBuffSize;
		}

		/**
		 * @brief 开启一个服务单的监听, 监听地址默认0.0.0.0
		 * @param listenPort 监听端口 0-65535
		 * @return 是否有错误
		*/
		Error Listen(u_short listenPort) {
			if (_serverRunning) {
				return Error::serverIsStarted;
			}

			_listenPort = listenPort;

			// 初始化设备
			if (WSAStartup(_versionRequested, &_wsaDat) != 0) {
				return Error::initializationWinsockFailed;
			}

			// 创建Socket
			{
				memset(&_sock, 0, sizeof(struct sockaddr_storage));
				_listenAddr.toSockaddr(&_sock, _listenPort);
				_socket = socket((ADDRESS_FAMILY)_listenAddr.IPFamily, SOCK_STREAM, IPPROTO_TCP);
				if (_socket == INVALID_SOCKET) {
					return Error::socketError;
				}
				if (::bind(_socket, (sockaddr*)&_sock, _listenAddr.toSockaddrLen()) == SOCKET_ERROR) {
					return Error::bindError;
				}
			}


			// 系统API建立监听,这里第二个参数表示在建立握手时队列最多可等待的连接数
			if (::listen(_socket, MEET_LISTEN_BACKLOG) == SOCKET_ERROR) {
				return Error::listenError;
			}

			_serverRunning = true;

			_data_buffer = new char[_recvBuffSize];
			memset(_data_buffer, '\0', _recvBuffSize);

			if (!_blockingMode) {	// 非阻塞模式
				u_long iMode = 1;
				::ioctlsocket(_socket, FIONBIO, &iMode);
			}


			//创建一个线程 接收客户端的连接
			auto _connect_thread = std::thread([this]() {
				while (_serverRunning) {
					if (!_blockingMode) {	// 非阻塞模式 在此循环中接收 处理消息
						recvAllClickData();
					}
					SOCKET c_socket;
					IP clientAddress;
					u_short clientPort = 0;

					struct sockaddr_storage remoteAddr = {};
					int nAddrlen = sizeof(remoteAddr);
					c_socket = ::accept(_socket, (sockaddr*)&remoteAddr, &nAddrlen); //默认应该会在这里阻塞
					if (c_socket == INVALID_SOCKET) {   //无效的套接字
						continue;
					}
					clientAddress = IP(&remoteAddr);
					clientPort = ntohs((remoteAddr.ss_family == (ADDRESS_FAMILY)Family::IPV4)? ((sockaddr_in*)&remoteAddr)->sin_port : ((sockaddr_in6*)&remoteAddr)->sin6_port);

					size_t clientCount = 0;
					{
						std::shared_lock<std::shared_mutex> lock(rw_mtx_clientList);
						clientCount = clientList.size();
					}

					if (clientCount < _maxCount || _maxCount < 0) {

						MeetClient newClient(this, c_socket, clientAddress, clientPort);
						{
							std::unique_lock<std::shared_mutex> lock(rw_mtx_clientList);
							clientList.push_back(newClient);
						}
						
						//触发有客户端连接的回调
						if (_onNewClientConnectEvent != NULL) {
							_onNewClientConnectEvent(newClient);
						}

						if (!_blockingMode) {	// 在非阻塞模式下 由一个统一的线程统一接收所有客户端的数据， 后面是阻塞模式要处理的代码
							continue;
						}

						//创建线程 监听客户端传来的消息
						auto _recv_thread = std::thread([this, newClient]() {
							while (_serverRunning) {
								int recvbytecount;
								if ((recvbytecount = ::recv(newClient.getSocket(), _data_buffer, _recvBuffSize - 1, 0)) <= 0) {
									//Return 0 Network Outage
									if (recvbytecount == 0) {
										std::unique_lock<std::shared_mutex> lock(rw_mtx_clientList);
										removeClientFromClientList(newClient.getIp(), newClient.getPort());
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
										_data_buffer[recvbytecount] = '\0';
										_recvDataEvent(newClient, recvbytecount, _data_buffer);
									}
								}
							}
							//delete[] buffer;
							});
						_recv_thread.detach();



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
	private:

		/**
		 * @brief 在非阻塞模式下 处理接收远程客户端消息
		*/
		void recvAllClickData() {
			// 轮询所有客户端 接收数据
			std::shared_lock<std::shared_mutex> lock(rw_mtx_clientList);
			for (auto it = clientList.begin(); it != clientList.end();) {
				//if ((*it).hasDisconnectTag()) {
				//	it = clientList.erase(it);
				//	continue;
				//}
				int recvbytecount;
				if ((recvbytecount = ::recv((*it).getSocket(), _data_buffer, _recvBuffSize - 1, 0)) <= 0) {
					//Return 0 Network Outage
					if (recvbytecount == 0) {
						MeetClient& c = *it;
						{
							lock.unlock();
							std::unique_lock<std::shared_mutex> lock(rw_mtx_clientList);
							it = clientList.erase(it);
						}
						lock.lock();
						if (_onClientDisConnectEvent != NULL) {
							_onClientDisConnectEvent(c);
						}
						continue;
					}

				}//if ((recvbytecount = recv(c->sockfd, _data_buffer, sizeof(_data_buffer), 0)) <= 0)

				//接收数据 触发回调
				else {
					if (_recvDataEvent != NULL) {
						_data_buffer[recvbytecount] = '\0';
						_recvDataEvent((*it), recvbytecount, _data_buffer);
					}
				}
				++it;
			}
		}

	public:

		/**
		 * @brief 设置阻塞模式,在连接前设置 默认为阻塞模式
		 * @param blocking 阻塞模式
		 * @return 是否有错误 [Error::serverIsStarted] / [Error::noError]
		*/
		Error setBlockingMode(bool blocking) {
			if (!_serverRunning) {
				_blockingMode = blocking;
				return Error::noError;
			}
			return Error::serverIsStarted;
		}

		/**
		 * @brief 是否是阻塞模式
		 * @return 
		*/
		bool isBlockingMode() {
			return _blockingMode;
		}

	private:
		/**
		 * @brief 仅从客户端列表中移除客户端信息//注意线程安全
		 * @param addr 
		 * @param port 
		 * @return 是否有错误 [Error::noFoundClient] / [Error::noError]
		*/
		Error removeClientFromClientList(IP addr, u_short port) {
			for (auto it = clientList.begin(); it != clientList.end(); it++) {
				if ((*it).getIp().toString() == addr.toString() && (*it).getPort() == port) {		// 条件语句
					// disClientConnect 方法内部会调用这个函数
					clientList.erase(it);		// 移除他
					//it--;									// 让该迭代器指向前一个， 只删除一个没必要,而且会出现错误
					return Error::noError;
				}
			}
			return Error::noFoundClient;
		}

		Error removeClientFromClientList(MeetClient& client) {
			for (auto it = clientList.begin(); it != clientList.end(); it++) {
				if ((*it) == client) {						// 条件语句
					clientList.erase(it);				// 移除他
					//it--;									// 让该迭代器指向前一个， 只删除一个没必要,而且会出现错误
					return Error::noError;
				}
			}
			return Error::noFoundClient;
		}

	public:
		/**
		 * @brief 断开客户端的连接
		 * @param addr 
		 * @param port 
		 * @return 是否有错误 [Error::serverNotStarted] / [Error::noError] / [Error::unkError] / [Error::noFoundClient]
		*/
		Error disClientConnect(IP addr, u_short port) {
			if (!_serverRunning) {
				return Error::serverNotStarted;
			}
			std::shared_lock<std::shared_mutex> lock(rw_mtx_clientList);
			for (auto& c : clientList) {
				if (addr.toString() == c.getIp().toString() && port == c.getPort()) {
					shutdown(c.getSocket(), SD_BOTH);
					if (closesocket(c.getSocket()) == 0) {
						//if (_blockingMode) {
						//	lock.unlock();
						//	std::unique_lock<std::shared_mutex> uniquelock(rw_mtx_clientList);
						//	removeClientFromClientList(addr, port);
						//}
						//else {
						//	c.setDisconnectTag();
						//}
						{
							lock.unlock();
							std::unique_lock<std::shared_mutex> uniquelock(rw_mtx_clientList);
							removeClientFromClientList(addr, port);
						}
						//循环 
						return Error::noError;
					}
					return Error::unkError;
				}
			}
			return Error::noFoundClient;
		};

		Error disClientConnect(MeetClient& client) {
			return disClientConnect(client.getIp(), client.getPort());
		}


		/**
		 * @brief 注册客户端连接时事件
		 * @param onNewClientConnectEvent 
		*/
		void onNewClientConnect(NewClientConnectEvent onNewClientConnectEvent) {
			_onNewClientConnectEvent = onNewClientConnectEvent;
		};

		/**
		 * @brief 注册客户端断开连接时事件
		 * @param onClientDisConnectEvent 
		*/
		void onClientDisConnect(ClientDisConnectEvent onClientDisConnectEvent) {
			_onClientDisConnectEvent = onClientDisConnectEvent;
		};

		/**
		 * @brief 注册接收到客户端消息时回调
		 * @param recvDataEvent 
		*/
		void onRecvData(RecvDataEvent recvDataEvent) {
			_recvDataEvent = recvDataEvent;
		}

		/**
		 * @brief 阻塞模式 接收客户端消息时错误回调
		 * @param recvErrorEvent 
		*/
		void onRecvError(RecvErrorEvent recvErrorEvent) {
			_recvErrorEvent = recvErrorEvent;
		}

	public:

		/**
		 * @brief 发送文本 /  Send text
		 * @param socket 
		 * @param text 
		 * @return 是否有错误 [Error::serverNotStarted] / [Error::noError] / [Error::dataTooLong] / [Error::sendFailed]
		*/
		Error sendText(SOCKET socket, std::string text) {
			if (!_serverRunning) {
				return Error::serverNotStarted;
			}
			auto textlen = text.length();
			if (textlen > INT_MAX) {
				return Error::dataTooLong;		// 长度不能超过Int的最大值
			}
			int len = static_cast<int>(textlen);
			char* p = const_cast<char*>(text.data());
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

		/**
		 * @brief 发送字节 / Sending bytes
		 * @param socket 
		 * @param data 
		 * @param size 
		 * @return 是否有错误 [Error::serverNotStarted] / [Error::noError] / [Error::sendFailed]
		*/
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

		/**
		 * @brief 获取所有已连接的客户端， 不允许在外部修改容器，所以不返回引用
		 * @return 客户端列表
		*/
		std::vector<MeetClient> GetALLClient() {
			std::shared_lock<std::shared_mutex> lock(rw_mtx_clientList);
			return clientList;
		}

	private:
		/**
		 * @brief 监听地址
		*/
		IP _listenAddr = "0.0.0.0";
		/**
		 * @brief 监听端口
		*/
		u_short _listenPort = 0;
		/**
		 * @brief 最大客户端连接数量
		*/
		int _maxCount = MEET_LISTEN_DEFAULT_MAXCONNECT;

		/**
		 * @brief 接收数据的数据缓冲区大小
		*/
		int _recvBuffSize = 1024;

		/**
		 * @brief 接收并处理数据的缓冲区
		*/
		char* _data_buffer = nullptr;

		WSADATA _wsaDat{};
		u_short _versionRequested = MAKEWORD(2, 2);
		
		struct sockaddr_storage _sock = {};
		//struct sockaddr_in _sock4 = {};
		//struct sockaddr_in6 _sock6 = {};

		/**
		 * @brief 服务端监听会话
		*/
		SOCKET _socket{};

		/**
		 * @brief 接收消息阻塞模式
		*/
		bool _blockingMode = true;

		/**
		 * @brief 连接的客户端列表
		*/
		std::vector<MeetClient> clientList;

		/**
		 * @brief 标志，防止重复初始化， 服务端是否启动
		*/
		bool _serverRunning = false;

		/**
		 * @brief 争对客户端列表的读写锁（可有多个线程拥有读锁，但仅能有一个线程拥有写锁）
		*/
		std::shared_mutex rw_mtx_clientList;

		// 回调
		NewClientConnectEvent _onNewClientConnectEvent = NULL;
		ClientDisConnectEvent _onClientDisConnectEvent = NULL;
		RecvDataEvent _recvDataEvent = NULL;
		RecvErrorEvent _recvErrorEvent = NULL;
	};//class TCPServer
#pragma endregion

#pragma region UDPClient
	//class UDPClient
	class UDPClient {

	public:
		UDPClient(){}
		~UDPClient() {
			close();
		}

		void close() {
			_init = false;
			if (_sockfd4) {
				socketv4bindAddr = false;
				closesocket(_sockfd4);
				WSACleanup();
				_sockfd4 = NULL;
			}
			if (_sockfd6) {
				socketv6bindAddr = false;
				closesocket(_sockfd6);
				WSACleanup();
				_sockfd6 = NULL;
			}
		}

	public:

		using RecvDataEvent = std::function<void(ULONG64, const char*, IP, u_short)>;
		using RemoteSockCloseEvent = std::function<void(IP, u_short)>;

	public:

		/**
		 * @brief 启用IPv4支持,启用表示会开启一个SOCKET用于IPv4通讯,如果确认不会需要IPv4可以关闭以节省开销
		 * @param ipv4support (IPv4Support默认开启)
		 * @return [Error::inited] [Error::noError]
		*/
		Error setIPv4Support(bool ipv4support) {
			if (_init) {
				return Error::inited;
			}
			supportipv4 = ipv4support;
			return Error::noError;
		}

		/**
		 * @brief 启用IPv6支持,启用表示会开启一个SOCKET用于IPv6通讯,如果确认不会需要IPv6可以关闭以节省开销
		 * @param ipv6support (IPv6Support默认关闭)
		 * @return [Error::inited] [Error::noError]
		*/
		Error setIPv6Support(bool ipv6support) {
			if (_init) {
				return Error::inited;
			}
			supportipv6 = ipv6support;
			return Error::noError;
		}

		/**
		 * @brief 设置阻塞模式(默认阻塞)
		 * @param blocking 
		 * @return [Error::noError] [Error::inited]
		*/
		Error setBlockingMode(bool blocking) {
			if (!_init) {
				_blockingmode = blocking;
				return Error::noError;
			}
			return Error::inited;
		}

		Error allowbroadcast(bool allow) {
			if (!_init) {
				_allowbroadcast = allow;
				return Error::noError;
			}
			return Error::inited;
		}

		/**
		 * @brief 默认 Only IPV4 ,请在init之前setIPv4Support(true)/setIPv6Support(true),以开启/关闭对ipv4,ipv6的支持
		*/
		Error Init() {
			if (_init) {
				return Error::inited;
			}
			if (!supportipv4 && !supportipv6) {
				// 请至少开启一处v4orv6支持, 未知的地址族
				return Error::unFamily;
			}
			//Initializing the socket library
			WSADATA wsadata; //Define a structure of type WSADATA to store the Windows Sockets data returned by the WSAStartup function call
			WORD sock_version = MAKEWORD(2, 0); //Set the version number
			if (WSAStartup(sock_version, &wsadata)) //Initialize the socket, start the build, and load "ws2_32.lib" into memory
			{
				return Error::initializationWinsockFailed;
			}

			if (supportipv4) {
				if ((_sockfd4 = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
					close();
					return Error::socketError;	// -1
				}
			}
			if (supportipv6) {
				if ((_sockfd6 = ::socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
					close();
					return Error::socketError;	// -1
				}
			}

			_init = true;

			// 阻塞模式(默认)与非阻塞模式
			if (!_blockingmode) {
				u_long iMode = 1;
				if (supportipv4) ::ioctlsocket(_sockfd4, FIONBIO, &iMode);
				if (supportipv6) ::ioctlsocket(_sockfd6, FIONBIO, &iMode);
			}

			if (_allowbroadcast) {
				if (supportipv4) setsockopt(_sockfd4, SOL_SOCKET, SO_BROADCAST, (const char*)&_allowbroadcast, sizeof(_allowbroadcast));
				if (supportipv6) setsockopt(_sockfd6, SOL_SOCKET, SO_BROADCAST, (const char*)&_allowbroadcast, sizeof(_allowbroadcast));
			}

			if (supportipv4) {
				_makeIPv4RecvThread().detach();
			}
			if (supportipv6) {
				_makeIPv6RecvThread().detach();
			}
			return Error::noError;
		}

		Error sendData(IP ip, u_short port,const char* data, int len) {
			if (!_init) {
				return Error::uninitialized;
			}
			if (!ip.isValid()) {
				return Error::ipInvalid;
			}
			//SOCKADDR_IN sendAddr4{};
			//SOCKADDR_IN6 sendAddr6{};
			int ret;
			auto toShrPtr = ip.toSockaddr(port);
			auto to = (sockaddr*)toShrPtr.get();
			
			if (ip.IPFamily == Family::IPV4) {
				if (!supportipv4) {
					return Error::unFamily;				// 没有写支持这个地址协议
				}
				ret = sendto(_sockfd4, data, len, 0, to, ip.toSockaddrLen());
			}
			else {
				if (!supportipv6) {
					return Error::unFamily;				// 没有写支持这个地址协议
				}
				ret = sendto(_sockfd6, data, len, 0, to, ip.toSockaddrLen());
			}
			if (ret == SOCKET_ERROR) {
				// 发送错误,应该关闭套接字
				printf_s("sendto ret=-1:%d\n", WSAGetLastError());
				return Error::sendFailed;
			}
			else
			{
				if (ip.IPFamily == Family::IPV4)
					socketv4bindAddr = true;
				else
					socketv6bindAddr = true;
			}
			return Error::noError;
		}
		
		// 红娘 通过中间人发送读取数据
		//void matchmaker() {

		//}
	private:
		/**
		 * @brief 处理接收消息返回-1的问题
		 * @param wsaLastError 
		 * @param socketbindAddr 类全局的判定开关,判断该socket是否传入了ip
		 * @return false 跳出, true 接着执行 
		*/
		bool _recvFromError(int wsaLastError, bool& socketbindAddr, sockaddr* address) {
			//说明本地套接字被关闭,不是错误
			if (wsaLastError == WSAEINTR) {
				socketbindAddr = false;
				return false;
			}
			
			if (!_blockingmode) {
				if (wsaLastError == WSAEWOULDBLOCK) {   // 非阻塞模式下 这个说明没有数据可接受,是正常的
					//Sleep(1);
					return false;						// 这里跳出
				}
			}

			// 远端SOCK被关闭
			if (wsaLastError == WSAECONNRESET) {
				if (_remoteSockCloseEvent != NULL) {
					_remoteSockCloseEvent(address, ntohs((address->sa_family == (ADDRESS_FAMILY)meet::Family::IPV4) ? ((sockaddr_in*)address)->sin_port : ((sockaddr_in6*)address)->sin6_port));
				}
				return false;
			}
			// 接收失败 并出现了错误
			printf_s("recvfrom v4 or v6: %d\n", wsaLastError);//10054 广播远程主机强制关闭了socket
			return true;
		}

		/**
		 * @brief 处理IPv4的数据接收
		 * @param buf 接受缓冲区
		 * @param buffsize 缓冲区大小
		 * @return false 跳出, true 接着执行 
		*/
		bool _handleIPv4AcceptData(char buf[], int buffsize) {
			sockaddr_in remoteAddr{};
			int len = sizeof(remoteAddr);
			int recvbytecount = ::recvfrom(_sockfd4, buf, buffsize - 1, 0, (SOCKADDR*)&remoteAddr, &len);
			if (recvbytecount == SOCKET_ERROR) {
				if (!_recvFromError(WSAGetLastError(), socketv4bindAddr, (SOCKADDR*)&remoteAddr)) {
					return false;
				}
			}
			if (recvbytecount >= 0) {
				buf[recvbytecount] = '\0';
				_recvDataEvent(recvbytecount, buf, remoteAddr.sin_addr, ntohs(remoteAddr.sin_port));
			}
			return true;
		}

		/**
		 * @brief 处理IPv6的数据接收
		 * @param buf 接收缓冲区
		 * @param buffsize 缓冲区大小
		 * @return false 跳出, true 接着执行 
		*/
		bool _handleIPv6AcceptData(char buf[], int buffsize) {
			sockaddr_in6 remoteAddr{};
			int len = sizeof(remoteAddr);
			int recvbytecount = ::recvfrom(_sockfd6, buf, buffsize - 1, 0, (SOCKADDR*)&remoteAddr, &len);
			if (recvbytecount == SOCKET_ERROR) {
				if (!_recvFromError(WSAGetLastError(), socketv6bindAddr, (SOCKADDR*)&remoteAddr)) {
					return false;
				}
			}
			if (recvbytecount >= 0) {
				buf[recvbytecount] = '\0';
				_recvDataEvent(recvbytecount, buf, remoteAddr.sin6_addr, ntohs(remoteAddr.sin6_port));
			}
			return true;
		}

		std::thread _makeIPv4RecvThread() {
			// 创建线程 从套接字接受数据
			return std::thread([&]() {
				char* buffer = new char[recvBuffSize];
				while (true)
				{
					if (!_init) {
						break;
					}
					if (!supportipv4 || !_sockfd4) {
						break;
					}
					// 你不接收数据那我就 sleep
					if (_recvDataEvent == NULL) {
						Sleep(10);
						continue;
					}
					// 处理IPV4
					if (socketv4bindAddr) {
						if (!_handleIPv4AcceptData(buffer, recvBuffSize)) {
							break;
						}
					}
				}
				delete[] buffer;
				});
		}

		std::thread _makeIPv6RecvThread() {
			// 创建线程 从套接字接受数据
			return std::thread([&]() {
				//char buffer[RecvBuffSize];
				char* buffer = new char[recvBuffSize];
				memset(buffer, '\0', recvBuffSize);
				while (true)
				{
					if (!_init) {
						break;
					}
					if (!supportipv6 || !_sockfd6) {
						break;
					}
					// 你不接收数据那我就 Sleep(10)
					if (_recvDataEvent == NULL) {
						Sleep(10);
						continue;
					}
					// 处理IPV6
					if (socketv6bindAddr) {
						if (!_handleIPv6AcceptData(buffer, recvBuffSize)) {
							break;
						}
					}
				}
				delete[] buffer;
				});
		}

	public:
		void OnRecvData(RecvDataEvent recvDataEvent) {
			_recvDataEvent = recvDataEvent;
		}

		void OnRemoteSockClose(RemoteSockCloseEvent remoteSockCloseEvent) {
			_remoteSockCloseEvent = remoteSockCloseEvent;
		}

	private:

		RecvDataEvent _recvDataEvent = NULL;
		RemoteSockCloseEvent _remoteSockCloseEvent = NULL;
		
	private:
		bool _init = false;
		bool socketv4bindAddr = false;
		bool socketv6bindAddr = false;
		bool supportipv4 = true;
		bool supportipv6 = false;
		bool _blockingmode = true;
		bool _allowbroadcast = false;
		int recvBuffSize = 102400;


		SOCKET _sockfd4{};
		SOCKET _sockfd6{};
	};
#pragma endregion

#pragma region UDPServer
	//class UDPServer
	class UDPServer {
	public:
		UDPServer(){}
		~UDPServer() {
			close();
		}

		void close() {
			_init = false;
			if (_sockfd) {
				closesocket(_sockfd);
				WSACleanup();
				_sockfd = NULL;
			}
		}
	public:

		using RecvDataEvent = std::function<void(ULONG64, const char*, IP, u_short)>;

	public:

		/**
		 * @brief 设置阻塞模式(默认阻塞)
		 * @param blocking
		 * @return [Error::noError] [Error::inited]
		*/
		Error setBlockingMode(bool blocking) {
			if (!_init) {
				_blockingmode = blocking;
				return Error::noError;
			}
			return Error::inited;
		}

	public:

		Error InitAndBind(IP ip, u_short port) {
			if (_init) {
				return Error::inited;
			}

			if (!ip.isValid()) {
				return Error::ipInvalid;
			}

			//Initializing the socket library
			WSADATA wsadata; //Define a structure of type WSADATA to store the Windows Sockets data returned by the WSAStartup function call
			WORD sock_version = MAKEWORD(2, 0); //Set the version number
			if (WSAStartup(sock_version, &wsadata)) //Initialize the socket, start the build, and load "ws2_32.lib" into memory
			{
				return Error::initializationWinsockFailed;
			}

			_sockfd = socket((int)ip.IPFamily, SOCK_DGRAM, IPPROTO_UDP);
			if (_sockfd == INVALID_SOCKET) {
				WSACleanup();
				return Error::socketError;
			}

			// 阻塞模式(默认)与非阻塞模式
			if (!_blockingmode) {
				u_long iMode = 1;
				::ioctlsocket(_sockfd, FIONBIO, &iMode);
			}

			sockaddr_storage sockAddr{};
			//sockaddr sockAddr{};
			memset(&sockAddr, '\0', sizeof(sockAddr));
			ip.toSockaddr(&sockAddr, port);
			if (::bind(_sockfd, (sockaddr*)&sockAddr, ip.toSockaddrLen()) == SOCKET_ERROR) {
				WSACleanup();
				return Error::bindError;
			}

			_init = true;

			_makeRecvThread().detach();

			return Error::noError;
		}
	public:
		std::thread _makeRecvThread() {
			// 创建线程 从套接字接受数据
			return std::thread([&]() {
				char* buffer = new char[recvBuffSize];
				sockaddr_storage sockAddr{};
				int sockAddrLen = sizeof(sockAddr);

				for (;;) {
					int recvDataLen = recvfrom(_sockfd, buffer, recvBuffSize - 1, 0, (sockaddr*)&sockAddr, &sockAddrLen);
					if (recvDataLen == SOCKET_ERROR) {
						int errid = WSAGetLastError();
						if (!_blockingmode) {
							if (errid == WSAEWOULDBLOCK) {			// 非阻塞模式下 这个说明没有数据可接受,是正常的
								continue;							// 跳过本次循环
							}
						}
						if (errid == WSAECONNRESET) {				// 远程主机关闭了这个SOCK // 得想办法发送到回调函数里 告诉用户
							// TODO: 想办法发送到回调函数里
							continue;
						}

						if (errid == WSAEMSGSIZE) {
							// RecvBuffSize 值设置太小
							perror("RecvBuffSize 值设置太小");
							break;
						}
						if (errid == WSAEINTR) {					// 这里应该是因为调用了close 里面的WSACleanup，因为recv阻塞所以线程还没清掉
							break;
						}
						printf_s("UDPServer - recvfrom错误: %d\n", errid);
						break;
					}
					// 接受函数
					if (recvDataLen >= 0) {
						if (_recvDataEvent != NULL) {
							buffer[recvDataLen] = '\0';
							_recvDataEvent(recvDataLen, buffer, &sockAddr, ntohs((sockAddr.ss_family == (ADDRESS_FAMILY)Family::IPV4) ? ((sockaddr_in*)&sockAddr)->sin_port : ((sockaddr_in6*)&sockAddr)->sin6_port));

						}
					}
				}
				delete[] buffer;
				});
		}

	public:
		Error sendData(IP ip, u_short port, const char* data, int len) {
			if (!_init) {
				return Error::uninitialized;
			}
			if (!ip.isValid()) {
				return Error::ipInvalid;
			}

			sockaddr_storage sendAddr{};
			int ret;
			ret = sendto(_sockfd, data, len, 0, (sockaddr*)ip.toSockaddr(&sendAddr, port), ip.toSockaddrLen());
			if (ret == SOCKET_ERROR) {
				// 发送错误,应该关闭套接字
				printf_s("sendto ret=-1:%d\n", WSAGetLastError());
				return Error::sendFailed;
			}
			return Error::noError;
		}


	public:
		void OnRecvData(RecvDataEvent recvDataEvent) {
			_recvDataEvent = recvDataEvent;
		}
	private:

		RecvDataEvent _recvDataEvent = NULL;

	private:
		bool _init = false;
		bool _blockingmode = true;

		int recvBuffSize = 102400;

		SOCKET _sockfd{};
	};
#pragma endregion

}//namespace meet


#endif // !MEETLIB