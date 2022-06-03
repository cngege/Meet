/*
*
* Copyright(C) 2022 MiracleForest Studio. All Rights Reserved.
*
* @filename:ip.hpp
* @creation time:2022.5.10.12:45
* @created by:Lovelylavender4
* -----------------------------------------------------------------------------
*
*
* -----------------------------------------------------------------------------
* If you have contact or find bugs,
* you can go to Github or email (MiracleForest@Outlook.com) to give feedback.
* We will try to do our best!
*/
#ifndef ___MIRACLEFOREST_MEET_IP___
#define ___MIRACLEFOREST_MEET_IP___

#pragma warning (disable: 4996)
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "error.hpp"
#pragma comment(lib,"ws2_32.lib")


namespace meet
{
	/// <summary>
	/// IP地址类型
	/// </summary>
	enum class Family{
		UNSPEC = 0,
		IPV4 = 2,
		IPV6 = 23
	};

	/// <summary>
	/// 废弃
	/// </summary>
	enum class DataType : byte {
		TEXT = 0x01,
		FILE = 0x02,
		MAXCONNECT = 0x03,		//只会在服务端发送给客户端 已到达最大的连接数,服务端将断开连接
	};

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
		IP(hostent* host){
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
		IP(Family ipfamily,std::string addr) {
			IPFamily = ipfamily;
			if (ipfamily == Family::IPV4){
				::inet_pton(AF_INET, addr.c_str(), &InAddr);
			}
			else if (ipfamily == Family::IPV6){
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

		IP(sockaddr addr) {
			if (addr.sa_family == AF_INET) {
				IPFamily = Family::IPV4;
				InAddr = ((struct sockaddr_in*)&addr)->sin_addr;
			}
			else {
				IPFamily = Family::IPV6;
				InAddr6 = ((struct sockaddr_in6*)&addr)->sin6_addr;
			}
		}

		IP(sockaddr_in addr_in) {
			IPFamily = Family::IPV4;
			InAddr = addr_in.sin_addr;
		}

		IP(sockaddr_in6 addr_in) {
			IPFamily = Family::IPV6;
			InAddr6 = addr_in.sin6_addr;
		}

		IP() {
			IP(Family::IPV4, "0.0.0.0");
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
			else{
				char IPStrBuf[INET6_ADDRSTRLEN] = { '\0' };
				return std::string(inet_ntop(AF_INET6, (void*)&InAddr6, IPStrBuf, sizeof(IPStrBuf)));
			}
			
		}

	public:

		static IP getaddrinfo(Family f,std::string dom) {
			WSADATA wsadata; //Define a structure of type WSADATA to store the Windows Sockets data returned by the WSAStartup function call
			if (WSAStartup(MAKEWORD(2, 0), &wsadata)) //Initialize the socket, start the build, and load "ws2_32.lib" into memory
			{
				return NULL;
			}
			struct addrinfo hints;
			memset(&hints, 0, sizeof(struct addrinfo));
			hints.ai_family = (int)f;     /* Allow IPv4 */
			hints.ai_flags = AI_CANONNAME;/* For wildcard IP address */
			hints.ai_protocol = 0;         /* Any protocol */
			hints.ai_socktype = SOCK_STREAM;

			struct addrinfo *res;
			auto ret = ::getaddrinfo(dom.c_str(), NULL, &hints, &res);
			if (ret == -1 || res == nullptr) {
				WSACleanup();
				return NULL;
			}
			WSACleanup();
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

}//namespace meet

#endif //!___MIRACLEFOREST_MEET_IP___