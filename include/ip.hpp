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
		IPV4 = 0,
		IPV6 = 1
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
			}
			else if (host->h_addrtype == AF_INET6) {
				IPFamily = Family::IPV6;
			}
			memcpy(&InAddr, host->h_addr, host->h_length);
		};

		/// <summary>
		/// 
		/// </summary>
		/// <param name="ipfamily">地址族</param>
		/// <param name="addr">字符串ip地址</param>
		IP(Family ipfamily,std::string addr) {
			IPFamily = ipfamily;
			INT a = 10;
			if (ipfamily == Family::IPV4){
				a = ::inet_pton(AF_INET, addr.c_str(), &InAddr);
			}
			else if (ipfamily == Family::IPV6){
				a = ::inet_pton(AF_INET6, addr.c_str(), &InAddr);
			}
		}

		IP(Family ipfamily, IN_ADDR inaddr) {
			IPFamily = ipfamily;
			InAddr = inaddr;
		}

		IP(sockaddr_in addr_in) {
			IPFamily = (addr_in.sin_family == AF_INET) ? Family::IPV4 : Family::IPV6;;
			InAddr = addr_in.sin_addr;
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
				return std::string(inet_ntop(AF_INET6, (void*)&InAddr, IPStrBuf, sizeof(IPStrBuf)));
			}
			
		}

	public:

		/// <summary>
		/// Get IP host information from hostname
		/// </summary>
		/// <param name="hostname">host.</param>
		/// <returns></returns>
		static hostent* gethostbyname(std::string hostname) {
			return gethostbyname(hostname.c_str());
		}
	public:
		/// <summary>
		/// IN_ADDR格式的IP地址
		/// </summary>
		IN_ADDR InAddr;
		in_addr6 InAddr6;
		/// <summary>
		/// IP协议族 V4/V6
		/// </summary>
		Family IPFamily = Family::IPV4;
	};//class IP

}//namespace meet

#endif //!___MIRACLEFOREST_MEET_IP___