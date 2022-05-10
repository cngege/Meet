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

namespace meet
{
	enum class Family
	{
		IPV4 = 0,
		IPV6 = 1
	};

	class IP
	{
		IP(hostent* _host)
		{
			host = _host;
		};

	private:
		hostent* host;
	public:
		auto GetHost()->hostent*
		{
			return host;
		};
	public:

		//从主机名中获取IP主机信息
		static auto gethostbyname(std::string hostname)->hostent*
		{
			return gethostbyname(hostname.c_str());
		};
	};

}

#endif //!___MIRACLEFOREST_MEET_IP___

