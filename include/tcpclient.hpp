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
#include "ip.hpp"
#include "error.hpp"

namespace meet {

	/// <summary>
	/// 
	/// </summary>
	class TCPClient {
	public:
		/// <summary>
		/// 
		/// </summary>
		/// <param name="f"></param>
		TCPClient(IPVFamily f = IPVFamily::IPV4) :family(f) {
			memset(&sock, 0, sizeof(sock));
			if (f == IPVFamily::IPV4) {
				sock.sin_family = AF_INET;
			}
			else if (f == IPVFamily::IPV6) {
				//temporarily not supported
				sock.sin_family = AF_INET6;
			}
		}

		~TCPClient()
		{
			if (sockfd)
			{
				closesocket(sockfd);
			}
		}
	public:
		/// <summary>
		/// init client
		/// </summary>
		/// <returns></returns>
		Error init() {
			WSADATA wsaData;
			if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
				return Error::initializationWinsockFailed;
			}
			if ((sockfd = socket(sock.sin_family, SOCK_STREAM, 0)) == SOCKET_ERROR) {
				return Error::socketError;
			}
			return Error::noError;
		};

		/// <summary>
		/// Connect to the original host.
		/// </summary>
		/// <param name="ip">The ip.</param>
		/// <param name="port">The port.</param>
		/// <returns></returns>
		Error connect(IP ip, unsigned short port) {
			sock.sin_port = htons(port);
			memcpy(&sock.sin_addr, ip.getHost()->h_addr, ip.getHost()->h_length);
			if (::connect(sockfd, (struct sockaddr*)&sock, sizeof(sock)) != 0) {
				//perror("connect");
				closesocket(sockfd);
				return Error::unkError;
			}
			connected = true;
			return Error::noError;
		};

		/// <summary>
		/// Dises the connect.
		/// </summary>
		/// <returns></returns>
		Error disConnect() {

		};

		/// <summary>
		/// 
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		Error sendText(std::string) {

		};

		/// <summary>
		/// Sends the file.
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		Error sendFile(std::string) {

		};

	private:
		/// <summary>
		/// Starts the recv.
		/// </summary>
		void StartRecv() {

		};

	private:
		SOCKET sockfd;
		sockaddr_in sock;
		bool connected = false;
		IPVFamily family;
	};//class TCPClient

}//namespace meet

#endif //!___MIRACLEFOREST_MEET_TCPCLIENT___

