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

#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include "error.hpp"
//#include "../third-party/nlohmann/json.hpp"
#pragma comment(lib,"ws2_32.lib")

namespace meet
{
	enum class IPVFamily{
		IPV4 = 0,
		IPV6 = 1
	};//enum class Family

	/// <summary>
	/// 
	/// </summary>
	class IP{
		IP(hostent* _host):host(_host) {};
	public:
		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		hostent* getHost() {
			return host;
		};
	public:		
		/// <summary>
		/// Get IP host information from hostname
		/// </summary>
		/// <param name="hostname"></param>
		/// <returns></returns>
		static auto gethostbyname(std::string hostname)->hostent*
		{
			return gethostbyname(hostname.c_str());
		};

	private:
		hostent* host;
	};//class IP

}//namespace meet

#endif //!___MIRACLEFOREST_MEET_IP___

