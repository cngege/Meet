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
#include <WS2tcpip.h>

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
			if (sockfd){
				closesocket(sockfd);
			}
			connected = false;
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
			if (connected){
				return Error::connecting;
			}
			//Get IP address type v4/v6
			memset(&sock, 0, sizeof(sock));
			u_short iptype = ip.getHost()->h_addrtype;
			sock.sin_family = iptype;
			if (iptype == AF_INET){
				family = Family::IPV4;
			}
			else if (iptype == AF_INET6){
				//family = Family::IPV6;
				return Error::unsupportedOperations;
			}

			//Create sockets
			if ((sockfd = socket(sock.sin_family, SOCK_STREAM, 0)) == SOCKET_ERROR){
				return Error::socketError;
			}
			sock.sin_port = htons(port);
			memcpy(&sock.sin_addr, ip.getHost()->h_addr, ip.getHost()->h_length);
			if (::connect(sockfd, (struct sockaddr*)&sock, sizeof(sock)) == INVALID_SOCKET){
				closesocket(sockfd);
				return Error::connectFailed;
			}
			connected = true;

			///Open a thread Triggers a listening event after receiving and processing a message
			recv_thread = std::thread(startRecv, this);
			recv_thread.detach();
			return Error::noError;
		};
		
		/// <summary>
		/// Specify IPV4 IP for connection
		/// </summary>
		/// <param name="ip">Remote host ipv4 address, e.g. "127.0.0.1"</param>
		/// <param name="port">Remote host port</param>
		/// <returns></returns>
		Error connectV4(std::string ip, ushort port) {
			hostent* ipaddrA;
			if (::inet_pton(AF_INET, ip.c_str(), &ipaddrA->h_addr) != 1) //0:String is not a valid IP address,-1:Other error
			{
				return Error::changeError;
			};
			IP ipaddrB(ipaddrA);
			return connect(ipaddrB, port);
		}

		/// <summary>
		/// Specify IPV6 IP for connection
		/// </summary>
		/// <param name="ip">Remote host ipv6 address, such as "2000:0:0:0:0:0:0:0:1"</param>
		/// <param name="port">Remote host port</param>
		/// <returns></returns>
		Error connectV6(std::string ip, ushort port) {
			hostent* ipaddrA;
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
			if (connected && sockfd && closesocket(sockfd) == 0){
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
			if (!connected) {
				return Error::noConnected;
			}
			auto textlen = text.length();
			auto sendbyte = new char[textlen + 10]; // textlen \n:1 + size_t:8 + datatype:1 = 10
			memset(sendbyte, 0, textlen + 10);
			memcpy(sendbyte, &textlen, sizeof(size_t));
			sendbyte[8] = (byte)DataType::TEXT;
			sprintf(sendbyte + 9,text.c_str());
			auto sendcount = send(sockfd, sendbyte, strlen(sendbyte), 0);
			if (sendcount <= 0) {
				return Error::sendFailed;
			}
			return Error::noError;
		};

		/// <summary>
		/// 
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		Error sendFile(std::string){

		};

		/// <summary>
		/// Register callback events
		/// </summary>
		/// <param name="_disConnectEvent"></param>
		void onDisConnect(DisConnectEvent _disConnectEvent){
			disConnectEvent = _disConnectEvent;
		};

	public:

		/// <summary>
		/// Thread function, responsible for receiving network packets, and classify and analyze the processing
		/// </summary>
		/// <param name="c">TCPClient Instance</param>
		static void startRecv(TCPClient* c) {
			if (!c->connected){
				char buffer[5];
				memset(buffer, 0, sizeof(buffer));

				while (c->connected){
					int recvbytecount;
					if ((recvbytecount = recv(c->sockfd, buffer, sizeof(buffer), 0)) <= 0){
						//Return 0 Network Outage
						if (recvbytecount == 0){
							c->connected = false;
							c->disConnectEvent();
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
		SOCKET sockfd;
		sockaddr_in sock;
    	bool connected = false;
	    Family family;

		std::thread recv_thread;

		/// <summary>
		/// Event
		/// </summary>
		DisConnectEvent disConnectEvent;
	};//class TCPClient

}//namespace meet

#endif //!___MIRACLEFOREST_MEET_TCPCLIENT___