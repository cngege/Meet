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

#include "../third-party/nlohmann/json.hpp"
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
				family = Family::IPV4;
			}
			else if (host->h_addrtype == AF_INET6) {
				family = Family::IPV6;
			}
			memcpy(&inaddr, host->h_addr, host->h_length);
		};

		/// <summary>
		/// 
		/// </summary>
		/// <param name="ipfamily">地址族</param>
		/// <param name="addr">字符串ip地址</param>
		IP(Family ipfamily,std::string addr) {
			family = ipfamily;
			INT a = 10;
			if (ipfamily == Family::IPV4){
				a = ::inet_pton(AF_INET, addr.c_str(), &inaddr);
			}
			else if (ipfamily == Family::IPV6){
				a = ::inet_pton(AF_INET6, addr.c_str(), &inaddr);
			}
		}
	public:
	public:
	public:

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
		IN_ADDR inaddr;
		/// <summary>
		/// IP协议族 V4/V6
		/// </summary>
		Family family;
	};//class IP

}//namespace meet

#endif //!___MIRACLEFOREST_MEET_IP___